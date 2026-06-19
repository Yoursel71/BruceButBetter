/**
 * @file ir_replay.cpp
 * @brief IR auto-replay and brute-force tools.
 */
#include "ir_replay.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include <globals.h>

IrLastCapture g_irLastCapture;

void irReplay_store(const IRCode &code) {
    g_irLastCapture.valid = true;
    g_irLastCapture.code  = code;
}

void ir_replay_last() {
    if (!g_irLastCapture.valid) {
        displayError("No signal in memory.\nCapture one with IR Read first.", true);
        return;
    }

    tft.fillScreen(bruceConfig.bgColor);
    drawMainBorderWithTitle("IR REPLAY");
    tft.setTextSize(FP);
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);

    int y = BORDER_PAD_Y + FM * LH + 4;
    IRCode &c = g_irLastCapture.code;

    if (c.type == "parsed") {
        tft.drawString("Proto: " + c.protocol, 12, y, 1); y += 10;
        tft.drawString("Addr:  " + c.address,  12, y, 1); y += 10;
        tft.drawString("Cmd:   " + c.command,  12, y, 1); y += 10;
    } else {
        String preview = c.data.substring(0, 20) + "...";
        tft.drawString("RAW: " + preview, 12, y, 1); y += 10;
    }

    tft.setTextColor(TFT_DARKGREY, bruceConfig.bgColor);
    tft.drawCentreString("[OK]Send  [ESC]Back", tftWidth / 2, tftHeight - 9, 1);

    bool running = true;
    while (running) {
        if (check(EscPress)) break;
        if (check(SelPress)) {
            tft.setTextColor(TFT_GREEN, bruceConfig.bgColor);
            tft.fillRect(12, y, tftWidth - 24, 10, bruceConfig.bgColor);
            tft.drawCentreString("Sending...", tftWidth / 2, y, 1);
            sendIRCommand(&c, true);
            delay(200);
            tft.fillRect(12, y, tftWidth - 24, 10, bruceConfig.bgColor);
            tft.drawCentreString("Sent! [OK]Again", tftWidth / 2, y, 1);
        }
        delay(50);
    }
}

void ir_brute_force() {
    // Select protocol
    uint8_t proto = 0;
    options.clear();
    options.push_back({"NEC",     [&]() { proto = 1; }});
    options.push_back({"Samsung", [&]() { proto = 2; }});
    options.push_back({"Sony",    [&]() { proto = 3; }});
    options.push_back({"Back",    [=]() { returnToMenu = true; }});
    loopOptions(options, MENU_TYPE_SUBMENU, "IR Protocol");
    if (returnToMenu || proto == 0) return;

    // Address input (hex 0x00-0xFF)
    uint8_t addr = 0;
    {
        options.clear();
        uint8_t tmp = 0;
        for (uint8_t a = 0; a <= 0x1F; a++) {
            uint8_t av = a;
            char label[8]; snprintf(label, sizeof(label), "0x%02X", a);
            options.push_back({label, [&addr, av]() { addr = av; }});
        }
        options.push_back({"Back", [=]() { returnToMenu = true; }});
        loopOptions(options, MENU_TYPE_SUBMENU, "Address (lo byte)");
        if (returnToMenu) return;
    }

    tft.fillScreen(bruceConfig.bgColor);
    drawMainBorderWithTitle("IR BRUTE");

    int y = BORDER_PAD_Y + FM * LH + 4;
    int lh = max(10, tftHeight / 8);

    const char *protoName[] = {"", "NEC", "Samsung", "SIRC"};
    tft.setTextSize(FP);
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    char buf[32];
    snprintf(buf, sizeof(buf), "Proto:%s Addr:0x%02X", protoName[proto], addr);
    tft.drawString(buf, 12, y, 1);
    y += lh;
    tft.setTextColor(TFT_DARKGREY, bruceConfig.bgColor);
    tft.drawCentreString("[ESC]Stop [OK]Pause", tftWidth / 2, tftHeight - 9, 1);

    bool paused = false;
    for (uint16_t cmd = 0; cmd <= 0xFF; cmd++) {
        if (check(EscPress)) break;
        if (check(SelPress)) {
            paused = !paused;
            delay(200);
        }
        while (paused) {
            if (check(EscPress)) { paused = false; cmd = 256; break; }
            if (check(SelPress)) { paused = false; delay(200); break; }
            delay(50);
        }
        if (cmd > 0xFF) break;

        IRCode code;
        char addrS[8], cmdS[8];
        snprintf(addrS, sizeof(addrS), "%02X 00 00 00", addr);
        snprintf(cmdS,  sizeof(cmdS),  "%02X 00 00 00", (uint8_t)cmd);
        code.address = addrS;
        code.command = cmdS;
        code.bits    = 32;

        switch (proto) {
            case 1: code.protocol = "NEC"; code.type = "parsed"; break;
            case 2: code.protocol = "Samsung32"; code.type = "parsed"; break;
            case 3:
                code.protocol = "SIRC";
                code.bits     = 12;
                code.type     = "parsed";
                break;
        }

        sendIRCommand(&code, true);

        // Update progress display
        tft.fillRect(12, y, tftWidth - 24, lh, bruceConfig.bgColor);
        tft.setTextColor(TFT_YELLOW, bruceConfig.bgColor);
        snprintf(buf, sizeof(buf), "Cmd: 0x%02X (%d/255)", cmd, cmd);
        tft.drawString(buf, 12, y, 1);

        // Progress bar
        int bW = tftWidth - 24;
        int px  = 12 + (cmd * bW) / 255;
        tft.fillRect(12, y + lh + 1, px - 12, 4, TFT_GREEN);
        tft.drawRect(12, y + lh + 1, bW, 4, bruceConfig.priColor);

        delay(120); // IR gap between codes
    }
}
