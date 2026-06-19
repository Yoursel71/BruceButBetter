/**
 * @file si5351_module.cpp
 * @brief Si5351 clock generator module for Bruce firmware.
 *
 * Hardware: Si5351A on I2C bus (SDA=IO8, SCL=IO18, addr=0x60).
 * Shares the same I2C bus as OLED (0x3C) and PN532 (0x24).
 * Wire.begin() is already called in _setup_gpio() before this runs.
 *
 * Frequency library units: hundredths of Hz (×100).
 *   1 MHz  → 100 000 000 ULL
 *   10 MHz → 1 000 000 000 ULL
 */
#include <Arduino.h>
#ifdef HAS_SI5351
#include "si5351_module.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include <si5351.h>
#include <globals.h>

// ── Singleton & state ─────────────────────────────────────────────────────────
static Si5351  si;
static bool    si_inited     = false;
static uint8_t si_clk_active = 0b000; // bit0=CLK0, bit1=CLK1, bit2=CLK2
static uint64_t si_freq[3]   = {10000000ULL, 10000000ULL, 10000000ULL}; // Hz

static bool si_ensure_init() {
    if (si_inited) return true;
    // Wire already started; don't call Wire.begin() again
    bool ok = si.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
    if (!ok) {
        displayError("Si5351 not found\nCheck I2C wiring", true);
        return false;
    }
    si.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
    si.drive_strength(SI5351_CLK1, SI5351_DRIVE_8MA);
    si.drive_strength(SI5351_CLK2, SI5351_DRIVE_8MA);
    // All outputs off at start
    si.output_enable(SI5351_CLK0, 0);
    si.output_enable(SI5351_CLK1, 0);
    si.output_enable(SI5351_CLK2, 0);
    si_inited = true;
    return true;
}

// ── Helpers ───────────────────────────────────────────────────────────────────
static String freqLabel(uint64_t hz) {
    char buf[24];
    if (hz >= 1000000ULL) {
        // MHz with 3 decimal places
        uint32_t mhz  = hz / 1000000ULL;
        uint32_t khz  = (hz % 1000000ULL) / 1000ULL;
        snprintf(buf, sizeof(buf), "%lu.%03lu MHz", (unsigned long)mhz, (unsigned long)khz);
    } else if (hz >= 1000ULL) {
        uint32_t khz = hz / 1000ULL;
        uint32_t hz3 = hz % 1000ULL;
        snprintf(buf, sizeof(buf), "%lu.%03lu kHz", (unsigned long)khz, (unsigned long)hz3);
    } else {
        snprintf(buf, sizeof(buf), "%lu Hz", (unsigned long)hz);
    }
    return String(buf);
}

static si5351_clock clkEnum(uint8_t clk) {
    if (clk == 1) return SI5351_CLK1;
    if (clk == 2) return SI5351_CLK2;
    return SI5351_CLK0;
}

static void si_apply(uint8_t clk, uint64_t hz) {
    if (hz > 160000000ULL) hz = 160000000ULL;
    if (hz < 8000ULL)      hz = 8000ULL;
    si_freq[clk] = hz;
    si.set_freq(hz * 100ULL, clkEnum(clk));
    si.output_enable(clkEnum(clk), 1);
    si_clk_active |= (1 << clk);
}

// ── Frequency dial UI ─────────────────────────────────────────────────────────
// UP/DOWN = change step, LEFT/RIGHT = change value, OK = apply, ESC = cancel
static const uint64_t STEPS[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000};
static const char *STEP_LABELS[] = {"1 Hz","10 Hz","100 Hz","1 kHz","10 kHz","100 kHz","1 MHz","10 MHz"};
#define N_STEPS 8

static uint64_t frequencyDial(uint64_t startHz, uint8_t clk) {
    uint64_t hz     = startHz;
    int      stepIdx = 5; // default: 100 kHz
    bool     running = true;
    bool     redraw  = true;

    while (running) {
        if (redraw) {
            tft.fillScreen(bruceConfig.bgColor);
            drawMainBorderWithTitle("SET FREQUENCY");

            int y  = BORDER_PAD_Y + FM * LH + 4;
            int lh = max(10, tftHeight / 7);

            tft.setTextSize(FP);

            // CLK label
            tft.setTextColor(TFT_YELLOW, bruceConfig.bgColor);
            char buf[16]; snprintf(buf, sizeof(buf), "CLK%d", clk);
            tft.drawCentreString(buf, tftWidth / 2, y, 1);
            y += lh + 2;

            // Frequency — big and centred
            tft.setTextSize(FM);
            tft.setTextColor(TFT_GREEN, bruceConfig.bgColor);
            String fl = freqLabel(hz);
            tft.drawCentreString(fl.c_str(), tftWidth / 2, y, 1);
            y += FM * LH + 4;

            tft.setTextSize(FP);

            // Step
            tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
            snprintf(buf, sizeof(buf), "Step: %s", STEP_LABELS[stepIdx]);
            tft.drawCentreString(buf, tftWidth / 2, y, 1);
            y += lh;

            // Range hint
            tft.setTextColor(TFT_DARKGREY, bruceConfig.bgColor);
            tft.drawCentreString("8kHz - 160MHz", tftWidth / 2, y, 1);

            // Footer
            int footY = tftHeight - BORDER_PAD_X - FP * LH - 2;
            tft.setTextColor(TFT_DARKGREY, bruceConfig.bgColor);
            tft.drawCentreString("<> Val  ^v Step  OK Set", tftWidth / 2, footY, 1);

            redraw = false;
        }

        if (check(EscPress)) { hz = 0; break; } // 0 = cancelled
        if (check(SelPress)) break;              // confirm

        if (check(NextPress)) {
            uint64_t nxt = hz + STEPS[stepIdx];
            if (nxt <= 160000000ULL) hz = nxt;
            redraw = true;
        }
        if (check(PrevPress)) {
            if (hz > STEPS[stepIdx] + 8000ULL)
                hz -= STEPS[stepIdx];
            else
                hz = 8000ULL;
            redraw = true;
        }
        // UP/DOWN change step size — map to whichever buttons those are
        if (check(UpPress)) {
            if (stepIdx < N_STEPS - 1) stepIdx++;
            redraw = true;
        }
        if (check(DownPress)) {
            if (stepIdx > 0) stepIdx--;
            redraw = true;
        }

        delay(60);
    }
    return hz;
}

// ── Public feature: Set Frequency ─────────────────────────────────────────────
void si5351_set_frequency_ui() {
    if (!si_ensure_init()) return;

    // Choose CLK
    uint8_t clk = 0;
    bool    picked = false;
    options.clear();
    options.push_back({"CLK0", [&]() { clk = 0; picked = true; }});
    options.push_back({"CLK1", [&]() { clk = 1; picked = true; }});
    options.push_back({"CLK2", [&]() { clk = 2; picked = true; }});
    options.push_back({"Back", [=]() { returnToMenu = true; }});
    loopOptions(options, MENU_TYPE_SUBMENU, "Select CLK");
    if (returnToMenu || !picked) return;

    uint64_t hz = frequencyDial(si_freq[clk], clk);
    if (hz == 0) return; // cancelled

    si_apply(clk, hz);

    // Confirm on screen
    tft.fillScreen(bruceConfig.bgColor);
    drawMainBorderWithTitle("RF GEN");
    tft.setTextSize(FP);
    tft.setTextColor(TFT_GREEN, bruceConfig.bgColor);
    int y = BORDER_PAD_Y + FM * LH + 8;
    char buf[16]; snprintf(buf, sizeof(buf), "CLK%d ON", clk);
    tft.drawCentreString(buf, tftWidth / 2, y, 1);
    y += 12;
    tft.setTextSize(FM);
    tft.drawCentreString(freqLabel(hz).c_str(), tftWidth / 2, y, 1);
    tft.setTextSize(FP);
    tft.setTextColor(TFT_DARKGREY, bruceConfig.bgColor);
    tft.drawCentreString("[ESC] Back", tftWidth / 2, tftHeight - 9, 1);

    while (!check(EscPress)) delay(50);
}

// ── Public feature: Continuous Output (run in background) ─────────────────────
void si5351_continuous_output() {
    if (!si_ensure_init()) return;

    uint8_t clk   = 0;
    bool    picked = false;
    options.clear();
    options.push_back({"CLK0", [&]() { clk = 0; picked = true; }});
    options.push_back({"CLK1", [&]() { clk = 1; picked = true; }});
    options.push_back({"CLK2", [&]() { clk = 2; picked = true; }});
    options.push_back({"Back", [=]() { returnToMenu = true; }});
    loopOptions(options, MENU_TYPE_SUBMENU, "Select CLK");
    if (returnToMenu || !picked) return;

    uint64_t hz = frequencyDial(si_freq[clk], clk);
    if (hz == 0) return;

    si_apply(clk, hz);

    // Status loop — show live, allow freq change with L/R while running
    bool    running = true;
    bool    redraw  = true;
    int     stepIdx = 5;

    while (running) {
        if (check(EscPress)) { running = false; break; }

        if (check(NextPress)) {
            uint64_t n = hz + STEPS[stepIdx];
            if (n <= 160000000ULL) { hz = n; si_apply(clk, hz); }
            redraw = true;
        }
        if (check(PrevPress)) {
            if (hz > STEPS[stepIdx] + 8000ULL) hz -= STEPS[stepIdx];
            else hz = 8000ULL;
            si_apply(clk, hz);
            redraw = true;
        }
        if (check(UpPress))   { if (stepIdx < N_STEPS-1) stepIdx++; redraw = true; }
        if (check(DownPress)) { if (stepIdx > 0) stepIdx--;          redraw = true; }

        if (redraw) {
            tft.fillScreen(bruceConfig.bgColor);
            drawMainBorderWithTitle("RF GEN - LIVE");
            int y  = BORDER_PAD_Y + FM * LH + 4;
            int lh = max(10, tftHeight / 7);

            tft.setTextSize(FP);
            tft.setTextColor(TFT_YELLOW, bruceConfig.bgColor);
            char buf[10]; snprintf(buf, sizeof(buf), "CLK%d  ON", clk);
            tft.drawCentreString(buf, tftWidth / 2, y, 1);
            y += lh + 2;

            tft.setTextSize(FM);
            tft.setTextColor(TFT_GREEN, bruceConfig.bgColor);
            tft.drawCentreString(freqLabel(hz).c_str(), tftWidth / 2, y, 1);
            y += FM * LH + 4;

            tft.setTextSize(FP);
            tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
            char step[16]; snprintf(step, sizeof(step), "Step: %s", STEP_LABELS[stepIdx]);
            tft.drawCentreString(step, tftWidth / 2, y, 1);

            int footY = tftHeight - BORDER_PAD_X - FP * LH - 2;
            tft.setTextColor(TFT_DARKGREY, bruceConfig.bgColor);
            tft.drawCentreString("<>Freq ^vStep [ESC]Off", tftWidth / 2, footY, 1);

            redraw = false;
        }
        delay(60);
    }

    // Turn off the clock when exiting
    si.output_enable(clkEnum(clk), 0);
    si_clk_active &= ~(1 << clk);
}

// ── Public feature: Output Toggle ────────────────────────────────────────────
void si5351_output_toggle() {
    if (!si_ensure_init()) return;

    uint8_t sel = 0xFF;
    options.clear();

    char lbl[3][16];
    for (int i = 0; i < 3; i++) {
        bool on = si_clk_active & (1 << i);
        snprintf(lbl[i], sizeof(lbl[i]), "CLK%d %s  %s", i,
                 on ? "ON " : "OFF",
                 freqLabel(si_freq[i]).c_str());
        int ii = i;
        options.push_back({lbl[i], [&sel, ii]() { sel = ii; }});
    }
    options.push_back({"All OFF", [&sel]() { sel = 10; }});
    options.push_back({"Back", [=]() { returnToMenu = true; }});
    loopOptions(options, MENU_TYPE_SUBMENU, "Toggle Output");
    if (returnToMenu) return;

    if (sel == 10) {
        for (int i = 0; i < 3; i++) {
            si.output_enable(clkEnum(i), 0);
        }
        si_clk_active = 0;
        displaySuccess("All CLK OFF", true);
        return;
    }
    if (sel <= 2) {
        bool now_on = !(si_clk_active & (1 << sel));
        si.output_enable(clkEnum(sel), now_on ? 1 : 0);
        if (now_on) {
            si.set_freq(si_freq[sel] * 100ULL, clkEnum(sel));
            si_clk_active |= (1 << sel);
        } else {
            si_clk_active &= ~(1 << sel);
        }
        char msg[24];
        snprintf(msg, sizeof(msg), "CLK%d %s", sel, now_on ? "ON" : "OFF");
        displaySuccess(msg, true);
    }
}

// ── Public feature: Frequency Sweep ──────────────────────────────────────────
void si5351_sweep() {
    if (!si_ensure_init()) return;

    uint8_t  clk      = 0;
    uint64_t startHz  = 88000000ULL;
    uint64_t stopHz   = 108000000ULL;
    uint64_t stepHz   = 100000ULL;
    uint32_t dwellMs  = 50;
    bool     running  = false;

    // Config menu
    int menuIdx   = 0;
    bool editMode = false;
    bool redraw   = true;

    while (true) {
        if (check(EscPress)) return;

        if (redraw) {
            drawMainBorderWithTitle("SWEEP CONFIG");
            tft.setTextSize(FP);

            int y  = BORDER_PAD_Y + FM * LH + 4;
            int lh = max(11, tftHeight / 8);

            const char *labels[] = {"CLK", "Start", "Stop", "Step", "Dwell", "START", "Back"};
            String vals[] = {
                "CLK" + String(clk),
                freqLabel(startHz),
                freqLabel(stopHz),
                freqLabel(stepHz),
                String(dwellMs) + "ms",
                "", ""
            };

            for (int i = 0; i < 7; i++) {
                uint16_t fg = (i == menuIdx) ? bruceConfig.bgColor : bruceConfig.priColor;
                uint16_t bg = (i == menuIdx) ? bruceConfig.priColor : bruceConfig.bgColor;
                tft.fillRect(7, y, tftWidth - 14, lh - 2, bg);
                tft.setTextColor(fg, bg);
                String line = String(labels[i]);
                if (vals[i].length()) line += ": " + vals[i];
                tft.drawString(line, 12, y + 2, 1);

                if (editMode && i == menuIdx && i < 5) {
                    tft.setTextColor(TFT_YELLOW, bg);
                    tft.drawRightString("<>", tftWidth - 12, y + 2, 1);
                }
                y += lh;
            }
            redraw = false;
        }

        if (check(NextPress)) {
            if (editMode) {
                switch (menuIdx) {
                    case 0: clk = (clk + 1) % 3; break;
                    case 1: startHz  = min(stopHz - stepHz, startHz + stepHz); break;
                    case 2: stopHz   = min(160000000ULL,     stopHz  + stepHz); break;
                    case 3: { static const uint64_t ss[] = {1000,10000,100000,1000000,10000000};
                              static int si2=2; si2=(si2+1)%5; stepHz=ss[si2]; } break;
                    case 4: dwellMs  = min((uint32_t)2000, dwellMs + 10); break;
                }
            } else {
                menuIdx = (menuIdx + 1) % 7;
            }
            redraw = true;
        }
        if (check(PrevPress)) {
            if (editMode) {
                switch (menuIdx) {
                    case 0: clk = (clk + 2) % 3; break;
                    case 1: startHz  = max(8000ULL,          startHz  - stepHz); break;
                    case 2: stopHz   = max(startHz + stepHz, stopHz   - stepHz); break;
                    case 3: { static const uint64_t ss[] = {1000,10000,100000,1000000,10000000};
                              static int si2=2; si2=(si2+4)%5; stepHz=ss[si2]; } break;
                    case 4: dwellMs  = max((uint32_t)10, dwellMs - 10); break;
                }
            } else {
                menuIdx = (menuIdx + 6) % 7;
            }
            redraw = true;
        }

        if (check(SelPress)) {
            if (menuIdx == 5) { running = true; break; }
            if (menuIdx == 6) return;
            if (menuIdx < 5) editMode = !editMode;
            redraw = true;
            delay(100);
        }
        delay(50);
    }

    // ── Sweep execution ───────────────────────────────────────────────────────
    si.output_enable(clkEnum(clk), 1);

    drawMainBorderWithTitle("SWEEPING...");
    int bX  = 12, bW = tftWidth - 24;
    int bY  = tftHeight / 2 - 3;
    tft.drawRect(bX, bY, bW, 6, bruceConfig.priColor);
    tft.setTextSize(FP);

    uint64_t hz = startHz;
    while (running) {
        if (check(EscPress)) break;

        si.set_freq(hz * 100ULL, clkEnum(clk));

        // Update display
        int prog = (int)(((hz - startHz) * (uint64_t)bW) / (stopHz - startHz));
        tft.fillRect(bX + 1, bY + 1, prog, 4, TFT_GREEN);

        tft.fillRect(7, bY + 10, tftWidth - 14, 10, bruceConfig.bgColor);
        tft.setTextColor(TFT_YELLOW, bruceConfig.bgColor);
        tft.drawCentreString(freqLabel(hz).c_str(), tftWidth / 2, bY + 10, 1);

        delay(dwellMs);

        hz += stepHz;
        if (hz > stopHz) {
            hz = startHz;
            // clear bar on wrap
            tft.fillRect(bX + 1, bY + 1, bW - 2, 4, TFT_DARKGREY);
        }
    }

    si.output_enable(clkEnum(clk), 0);
    si_clk_active &= ~(1 << clk);
}

// ── Public feature: AM Band scanner ──────────────────────────────────────────
void si5351_am_band() {
    if (!si_ensure_init()) return;

    // AM band: 530 kHz – 1700 kHz, typical 10kHz step
    uint64_t hz      = 530000ULL;
    uint32_t dwellMs = 100;
    uint8_t  clk     = 0;

    tft.fillScreen(bruceConfig.bgColor);
    drawMainBorderWithTitle("AM BAND");
    tft.setTextSize(FP);
    tft.setTextColor(TFT_YELLOW, bruceConfig.bgColor);
    int y = BORDER_PAD_Y + FM * LH + 4;
    int lh = max(10, tftHeight / 7);
    tft.drawCentreString("530kHz - 1700kHz", tftWidth / 2, y, 1);
    y += lh;
    tft.setTextColor(TFT_DARKGREY, bruceConfig.bgColor);
    tft.drawCentreString("<> Tune  OK Scan  ESC Back", tftWidth / 2, tftHeight - 9, 1);

    si_apply(clk, hz);
    bool scanning = false;
    bool redraw   = true;

    while (true) {
        if (check(EscPress)) break;

        if (check(SelPress)) {
            scanning = !scanning;
            redraw = true;
            delay(200);
        }
        if (!scanning) {
            if (check(NextPress)) { hz = min(1700000ULL, hz + 10000ULL); si_apply(clk, hz); redraw = true; }
            if (check(PrevPress)) { hz = max(530000ULL,  hz - 10000ULL); si_apply(clk, hz); redraw = true; }
        }

        if (scanning) {
            hz += 10000ULL;
            if (hz > 1700000ULL) hz = 530000ULL;
            si_apply(clk, hz);
            delay(dwellMs);
            redraw = true;
        }

        if (redraw) {
            // Frequency display
            tft.fillRect(7, y, tftWidth - 14, lh * 2, bruceConfig.bgColor);
            tft.setTextSize(FM);
            tft.setTextColor(TFT_GREEN, bruceConfig.bgColor);
            tft.drawCentreString(freqLabel(hz).c_str(), tftWidth / 2, y, 1);

            tft.setTextSize(FP);
            tft.setTextColor(scanning ? TFT_YELLOW : TFT_DARKGREY, bruceConfig.bgColor);
            tft.drawCentreString(scanning ? "SCANNING..." : "MANUAL", tftWidth / 2, y + FM * LH + 2, 1);

            // Visualise position in band
            int bX = 12, bW = tftWidth - 24, bH = 5;
            int bY = tftHeight - 9 - bH - 12;
            int px = bX + (int)(((hz - 530000ULL) * (uint64_t)bW) / (1700000ULL - 530000ULL));
            tft.fillRect(bX, bY, bW, bH, TFT_DARKGREY);
            tft.fillRect(max(bX, px - 2), bY, 4, bH, TFT_GREEN);
            tft.drawRect(bX, bY, bW, bH, bruceConfig.priColor);

            redraw = false;
        }

        if (!scanning) delay(60);
    }

    si.output_enable(clkEnum(clk), 0);
    si_clk_active &= ~(1 << clk);
}

// ── Public feature: Presets ───────────────────────────────────────────────────
void si5351_presets() {
    if (!si_ensure_init()) return;

    struct Preset { const char *name; uint64_t hz; };
    static const Preset PRESETS[] = {
        {"CB (27 MHz)",     27000000ULL},
        {"Toy RC (49 MHz)", 49000000ULL},
        {"FM Low (88 MHz)", 88000000ULL},
        {"FM Mid (100MHz)", 100000000ULL},
        {"2m Ham (144MHz)", 144000000ULL},
        {"WWV (10 MHz)",    10000000ULL},
        {"GPS L1 (1575M)",  1575420000ULL}, // ≥160MHz → clamped to 160MHz
        {"Custom...",       0},
    };
    const int N = sizeof(PRESETS) / sizeof(PRESETS[0]);

    uint8_t sel = 0xFF;
    options.clear();
    for (int i = 0; i < N; i++) {
        int ii = i;
        options.push_back({PRESETS[i].name, [&sel, ii]() { sel = ii; }});
    }
    options.push_back({"Back", [=]() { returnToMenu = true; }});
    loopOptions(options, MENU_TYPE_SUBMENU, "Presets");
    if (returnToMenu || sel == 0xFF) return;

    uint8_t clk = 0;
    bool picked = false;
    options.clear();
    options.push_back({"CLK0", [&]() { clk = 0; picked = true; }});
    options.push_back({"CLK1", [&]() { clk = 1; picked = true; }});
    options.push_back({"CLK2", [&]() { clk = 2; picked = true; }});
    options.push_back({"Back", [=]() { returnToMenu = true; }});
    loopOptions(options, MENU_TYPE_SUBMENU, "Select CLK");
    if (returnToMenu || !picked) return;

    uint64_t hz = PRESETS[sel].hz;
    if (hz == 0) {
        // Custom
        hz = frequencyDial(si_freq[clk], clk);
        if (hz == 0) return;
    }

    si_apply(clk, hz);

    tft.fillScreen(bruceConfig.bgColor);
    drawMainBorderWithTitle("RF GEN");
    tft.setTextSize(FP);
    tft.setTextColor(TFT_GREEN, bruceConfig.bgColor);
    int y = BORDER_PAD_Y + FM * LH + 8;
    char buf[10]; snprintf(buf, sizeof(buf), "CLK%d ON", clk);
    tft.drawCentreString(buf, tftWidth / 2, y, 1); y += 12;
    tft.setTextSize(FM);
    tft.drawCentreString(freqLabel(min(hz, 160000000ULL)).c_str(), tftWidth / 2, y, 1);
    tft.setTextSize(FP);
    tft.setTextColor(TFT_DARKGREY, bruceConfig.bgColor);
    tft.drawCentreString("[ESC] Back", tftWidth / 2, tftHeight - 9, 1);
    while (!check(EscPress)) delay(50);
}

#endif // HAS_SI5351
