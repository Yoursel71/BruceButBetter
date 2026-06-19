#include "RFMenu.h"
#include "core/display.h"
#include "core/settings.h"
#include "core/utils.h"
#include "modules/rf/record.h"
#include "modules/rf/rf_bruteforce.h"
#include "modules/rf/rf_jammer.h"
#include "modules/rf/rf_listen.h"
#include "modules/rf/rf_scan.h"
#include "modules/rf/rf_send.h"
#include "modules/rf/rf_spectrum.h"
#include "modules/rf/rf_waterfall.h"

void RFMenu::optionsMenu() {
    options = {
        {"Scan/copy",       [=]() { RFScan(); }       },
#if !defined(LITE_VERSION)
        {"Record RAW",      rf_raw_record             }, // Pablo-Ortiz-Lopez
        {"Custom SubGhz",   sendCustomRF              },
#endif
        {"Spectrum",        rf_spectrum               },
#if !defined(LITE_VERSION)
        {"RSSI Spectrum",   rf_CC1101_rssi            }, // @Pirata
        {"SquareWave Spec", rf_SquareWave             }, // @Pirata
        {"Spectogram",      rf_waterfall              }, // dev_eclipse
#if defined(BUZZ_PIN) or defined(HAS_NS4168_SPKR) and defined(RF_LISTEN_H)
        {"Listen",          rf_listen                 }, // dev_eclipse
#endif
        {"Bruteforce",      rf_bruteforce             }, // dev_eclipse
        {"Jammer",          [=]() { RFJammer(true); } },
        {"Jammer Bands",    [this]() { jammerBandsMenu(); }},
#endif
        {"Config",          [this]() { configMenu(); }},
    };
    addOptionToMainMenu();

    delay(200);
    String txt = "Radio Frequency";
    if (bruceConfigPins.rfModule == CC1101_SPI_MODULE) txt += " (CC1101)"; // Indicates if CC1101 is connected
    else txt += " Tx: " + String(bruceConfigPins.rfTx) + " Rx: " + String(bruceConfigPins.rfRx);

    loopOptions(options, MENU_TYPE_SUBMENU, txt.c_str());
}

// Frekans-bantlı jammer presetleri: önce frekansı ayarla, sonra jammer'ı başlat.
void RFMenu::jammerBandsMenu() {
    struct Band { const char *name; float mhz; };
    static const Band bands[] = {
        {"433.92 MHz", 433.92f}, // garaj, uzaktan kumanda, kapı
        {"315 MHz",    315.00f}, // ABD garaj/araba
        {"390 MHz",    390.00f}, // bazı kapı sistemleri
        {"433.42 MHz", 433.42f}, // bazı araç anahtarları
        {"868.35 MHz", 868.35f}, // Avrupa ISM (alarm, sensor)
        {"868 MHz",    868.00f},
        {"915 MHz",    915.00f}, // ABD ISM, LoRa
        {"303.87 MHz", 303.87f}, // bazi garaj kapilari
    };
    const int n = sizeof(bands) / sizeof(bands[0]);

    options.clear();
    for (int i = 0; i < n; i++) {
        float f = bands[i].mhz;
        options.push_back({bands[i].name, [f]() {
                               bruceConfigPins.setRfFreq(f);
                               RFJammer(true);
                           }});
    }
    options.push_back({"Back", [this]() { optionsMenu(); }});
    loopOptions(options, MENU_TYPE_SUBMENU, "Jammer Bands");
}

void RFMenu::configMenu() {
    options = {
        {"RF TX Pin", lambdaHelper(gsetRfTxPin, true)},
        {"RF RX Pin", lambdaHelper(gsetRfRxPin, true)},
        {"RF Module", setRFModuleMenu},
        {"RF Frequency", setRFFreqMenu},
        {"Back", [this]() { optionsMenu(); }},
    };

    loopOptions(options, MENU_TYPE_SUBMENU, "RF Config");
}

void RFMenu::drawIcon(float scale) {
    clearIconArea();
    int radius = scale * 7;
    int deltaRadius = scale * 10;
    int triangleSize = scale * 30;

    if (triangleSize % 2 != 0) triangleSize++;

    // Body
    tft.fillCircle(iconCenterX, iconCenterY - radius, radius, bruceConfig.priColor);
    tft.fillTriangle(
        iconCenterX,
        iconCenterY,
        iconCenterX - triangleSize / 2,
        iconCenterY + triangleSize,
        iconCenterX + triangleSize / 2,
        iconCenterY + triangleSize,
        bruceConfig.priColor
    );

    // Left Arcs
    tft.drawArc(
        iconCenterX,
        iconCenterY - radius,
        2.5 * radius,
        2 * radius,
        40,
        140,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    tft.drawArc(
        iconCenterX,
        iconCenterY - radius,
        2.5 * radius + deltaRadius,
        2 * radius + deltaRadius,
        40,
        140,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    tft.drawArc(
        iconCenterX,
        iconCenterY - radius,
        2.5 * radius + 2 * deltaRadius,
        2 * radius + 2 * deltaRadius,
        40,
        140,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );

    // Right Arcs
    tft.drawArc(
        iconCenterX,
        iconCenterY - radius,
        2.5 * radius,
        2 * radius,
        220,
        320,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    tft.drawArc(
        iconCenterX,
        iconCenterY - radius,
        2.5 * radius + deltaRadius,
        2 * radius + deltaRadius,
        220,
        320,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    tft.drawArc(
        iconCenterX,
        iconCenterY - radius,
        2.5 * radius + 2 * deltaRadius,
        2 * radius + 2 * deltaRadius,
        220,
        320,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
}
