#include "SelfTestMenu.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/sd_functions.h"
#include "core/utils.h"
#include "modules/NRF24/nrf_common.h"
#include "modules/rf/rf_utils.h"
#include <Wire.h>
#include <globals.h>

// Short name for the well-known I2C addresses this firmware cares about.
static const char *i2cLabel(uint8_t a) {
    switch (a) {
        case 0x3C:
        case 0x3D: return "OLED";
        case 0x24: return "PN532 NFC";
        case 0x60:
        case 0x61: return "Si5351";
        case 0x68: return "RTC/IMU";
        case 0x76:
        case 0x77: return "BMx";
        case 0x0B:
        case 0x55: return "Gauge";
        default: return "?";
    }
}

// One result row: the name, then OK or -- in a fixed column so they line up (the compact list
// shows 17 characters per row).
static Option resultRow(const String &name, bool ok) {
    String label = name.substring(0, 13);
    while (label.length() < 14) label += ' ';
    label += ok ? "OK" : "--";
    return Option(label, nullptr);
}

static bool i2cFound(const std::vector<uint8_t> &found, uint8_t a, uint8_t b = 0) {
    for (uint8_t f : found) {
        if (f == a || (b && f == b)) return true;
    }
    return false;
}

// Probes everything first and returns one row per module. The PN532, the Si5351 (on boards that
// have it) and the SPI modules always get a row, so a missing one shows "--" instead of
// disappearing; any other I2C device that answers is listed by its address.
static std::vector<Option> runChecks() {
    std::vector<Option> rows;
    rows.push_back(Option("Run again", nullptr));

    Wire.begin(bruceConfigPins.i2c_bus.sda, bruceConfigPins.i2c_bus.scl);
    std::vector<uint8_t> found;
    for (uint8_t a = 0x01; a <= 0x7F; a++) {
        Wire.beginTransmission(a);
        if (Wire.endTransmission() == 0) found.push_back(a);
    }
    rows.push_back(resultRow("PN532 NFC", i2cFound(found, 0x24)));
#ifdef HAS_SI5351
    rows.push_back(resultRow("Si5351", i2cFound(found, 0x60, 0x61)));
#endif
    for (uint8_t a : found) {
        if (a == 0x24 || a == 0x60 || a == 0x61) continue;
        char name[16];
        snprintf(name, sizeof(name), "0x%02X %s", a, i2cLabel(a));
        rows.push_back(resultRow(name, true));
    }

    // SPI modules: quiet probes, none of them draws on the screen.
    rows.push_back(resultRow("SD card", setupSdCard()));
    rows.push_back(resultRow("CC1101", cc1101Present()));
    bool nrf1Ok = false;
    if (nrf_start(NRF_MODE_SPI)) {
        nrf1Ok = NRFradio.isChipConnected();
        NRFradio.powerDown();
    }
    rows.push_back(resultRow("NRF24 #1", nrf1Ok));
#ifdef NRF24_2_CE_PIN
    bool nrf2Ok = false;
    if (nrf_start2()) {
        nrf2Ok = NRFradio2.isChipConnected();
        NRFradio2.powerDown();
    }
    rows.push_back(resultRow("NRF24 #2", nrf2Ok));
#endif
    return rows;
}

// Results are a normal list: UP/DOWN scroll, OK on any row runs the checks again, BACK leaves.
void SelfTestMenu::optionsMenu() {
    int index = 0;
    while (true) {
        displayInfo("Testing...");
        std::vector<Option> rows = runChecks();
        bool again = false;
        for (Option &row : rows) row.operation = [&again]() { again = true; };
        const int chosen = loopOptions(rows, MENU_TYPE_SUBMENU, "Self-Test", index);
        if (!again) break;
        index = chosen < 0 ? 0 : chosen;
    }
    returnToMenu = true;
}

void SelfTestMenu::drawIcon(float scale) {
    clearIconArea();

    int w = scale * 58;
    int h = scale * 66;
    int cx = iconCenterX;
    int cy = iconCenterY;
    if (w % 2) w++;
    if (h % 2) h++;

    // clipboard / checklist body
    tft.drawRoundRect(cx - w / 2, cy - h / 2, w, h, w / 8, bruceConfig.priColor);
    // clip at the top
    tft.drawRoundRect(cx - w / 6, cy - h / 2 - h / 14, w / 3, h / 8, 2, bruceConfig.priColor);

    // three checklist rows: check mark + line
    for (int i = 0; i < 3; i++) {
        int y = cy - h / 5 + i * (h / 5);
        tft.drawLine(cx - w / 4, y, cx - w / 8, y + h / 16, bruceConfig.priColor);
        tft.drawLine(cx - w / 8, y + h / 16, cx, y - h / 14, bruceConfig.priColor);
        tft.drawFastHLine(cx + w / 12, y, w / 3, bruceConfig.priColor);
    }
}
