#include "SelfTestMenu.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/sd_functions.h"
#include "core/utils.h"
#include "modules/NRF24/nrf_common.h"
#include <Wire.h>
#include <globals.h>

// Friendly name for the well-known I2C addresses this firmware cares about.
static const char *i2cLabel(uint8_t a) {
    switch (a) {
        case 0x3C:
        case 0x3D: return "OLED (SSD1306/SH1106)";
        case 0x24: return "PN532 NFC";
        case 0x60:
        case 0x61: return "Si5351 clock-gen";
        case 0x68: return "RTC / IMU";
        case 0x76:
        case 0x77: return "BMx sensor";
        case 0x0B:
        case 0x55: return "fuel gauge / PMIC";
        default: return "unknown";
    }
}

void SelfTestMenu::optionsMenu() {
    // Probe everything first, then draw once — a shared-SPI probe can't glitch
    // the screen mid-render this way.
    Wire.begin(bruceConfigPins.i2c_bus.sda, bruceConfigPins.i2c_bus.scl);

    String i2cList;
    int found = 0;
    for (uint8_t a = 0x01; a <= 0x7F; a++) {
        Wire.beginTransmission(a);
        if (Wire.endTransmission() == 0) {
            found++;
            char line[52];
            snprintf(line, sizeof(line), "  [OK] 0x%02X %s", a, i2cLabel(a));
            i2cList += String(line) + "\n";
        }
    }

    // SPI modules — display-free live probes (CC1101's own probe pops a dialog,
    // so it's left to the RF menu; it answers on the same SPI bus as these).
    bool sdOk = setupSdCard();

    bool nrf1Ok = false;
    if (nrf_start(NRF_MODE_SPI)) {
        nrf1Ok = NRFradio.isChipConnected();
        NRFradio.powerDown();
    }
#ifdef NRF24_2_CE_PIN
    bool nrf2Ok = false;
    if (nrf_start2()) {
        nrf2Ok = NRFradio2.isChipConnected();
        NRFradio2.powerDown();
    }
#endif

    drawMainBorderWithTitle("Hardware Self-Test");
    padprintln("");
    padprintln("I2C bus:");
    if (found == 0) {
        padprintln("  (none answered)");
    } else {
        int s = 0;
        while (s < (int)i2cList.length()) {
            int nl = i2cList.indexOf('\n', s);
            if (nl < 0) nl = i2cList.length();
            padprintln(i2cList.substring(s, nl));
            s = nl + 1;
        }
    }
    padprintln("");
    padprintln("SPI modules:");
    padprintln(String("  microSD  : ") + (sdOk ? "[OK]" : "[--]"));
    padprintln(String("  NRF24 #1 : ") + (nrf1Ok ? "[OK]" : "[--]"));
#ifdef NRF24_2_CE_PIN
    padprintln(String("  NRF24 #2 : ") + (nrf2Ok ? "[OK]" : "[--]"));
#endif
    padprintln("");
    padprintln("ESC / OK to exit");

    while (1) {
        if (check(EscPress) || check(SelPress)) {
            returnToMenu = true;
            break;
        }
        delay(20);
    }
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
