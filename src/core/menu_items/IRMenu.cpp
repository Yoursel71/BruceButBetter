#include "IRMenu.h"
#include "core/display.h"
#include "core/settings.h"
#include "core/utils.h"
#include "modules/ir/TV-B-Gone.h"
#include "modules/ir/custom_ir.h"
#include "modules/ir/ir_jammer.h"
// custom_ir.h: sendNECCommand / sendSamsungCommand / sendSonyCommand / sendRC5Command
#include "modules/ir/ir_read.h"
#include "modules/ir/ir_replay.h"

// Yaygın marka TV power (güç) kodları.
// ÖNEMLİ: bu fonksiyonlar argümanı address.substring(0,2) ile, yani "0x" ÖNEKİ
// OLMADAN tek bayt çıplak hex olarak parse ediyor ("04" -> 0x04). "0x.." verirsek
// "0x" -> 0 olur ve yanlış kod gider. Kodlar en-iyi-tahmin; çalışmayan markayı
// IR Read ile yakalayıp güncelleyin.
void IRMenu::powerOffMenu() {
    options = {
        {"Samsung TV",   []() { sendSamsungCommand("07", "02"); }},
        {"LG TV",        []() { sendNECCommand("04", "08"); }    },
        {"Sony TV",      []() { sendSonyCommand("01", "15", 12); }},
        {"Philips TV",   []() { sendRC5Command("00", "0C"); }    },
        {"Toshiba TV",   []() { sendNECCommand("40", "12"); }    },
        {"NEC Power",    []() { sendNECCommand("04", "08"); }    },
        {"Back",         [this]() { optionsMenu(); }             },
    };
    loopOptions(options, MENU_TYPE_SUBMENU, "Power Off Hub");
}

void IRMenu::optionsMenu() {
#if defined(ARDUINO_M5STICK_S3)
    bool prevPower = M5.Power.getExtOutput();
    M5.Power.setExtOutput(true); // ENABLE 5V OUTPUT
#endif
    options = {
        {"TV-B-Gone",    StartTvBGone                },
        {"Power Off Hub", [this]() { powerOffMenu(); }},
        {"Custom IR",    otherIRcodes                },
        {"IR Read",      [=]() { IrRead(); }         },
        {"Replay Last",  ir_replay_last              },
        {"IR Brute",     ir_brute_force              },
#if !defined(LITE_VERSION)
        {"IR Jammer",    startIrJammer               },
#endif
        {"Config",       [this]() { configMenu(); }  },
    };
    addOptionToMainMenu();

    String txt = "Infrared";
    txt += " Tx: " + String(bruceConfigPins.irTx) + " Rx: " + String(bruceConfigPins.irRx) +
           " Rpts: " + String(bruceConfigPins.irTxRepeats);
    loopOptions(options, MENU_TYPE_SUBMENU, txt.c_str());
#if defined(ARDUINO_M5STICK_S3)
    M5.Power.setExtOutput(prevPower);
#endif
}

void IRMenu::configMenu() {
    options = {
        {"Ir TX Pin", lambdaHelper(gsetIrTxPin, true)},
        {"Ir RX Pin", lambdaHelper(gsetIrRxPin, true)},
        {"Ir TX Repeats", setIrTxRepeats},
        {"Back", [this]() { optionsMenu(); }},
    };

    loopOptions(options, MENU_TYPE_SUBMENU, "IR Config");
}

void IRMenu::drawIcon(float scale) {
    clearIconArea();
    int iconSize = scale * 60;
    int radius = scale * 7;
    int deltaRadius = scale * 10;

    if (iconSize % 2 != 0) iconSize++;

    tft.fillRect(
        iconCenterX - iconSize / 2, iconCenterY - iconSize / 2, iconSize / 6, iconSize, bruceConfig.priColor
    );
    tft.fillRect(
        iconCenterX - iconSize / 3,
        iconCenterY - iconSize / 3,
        iconSize / 6,
        2 * iconSize / 3,
        bruceConfig.priColor
    );

    tft.drawCircle(iconCenterX - iconSize / 6, iconCenterY, radius, bruceConfig.priColor);

    tft.drawArc(
        iconCenterX - iconSize / 6,
        iconCenterY,
        2.5 * radius,
        2 * radius,
        220,
        320,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    tft.drawArc(
        iconCenterX - iconSize / 6,
        iconCenterY,
        2.5 * radius + deltaRadius,
        2 * radius + deltaRadius,
        220,
        320,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    tft.drawArc(
        iconCenterX - iconSize / 6,
        iconCenterY,
        2.5 * radius + 2 * deltaRadius,
        2 * radius + 2 * deltaRadius,
        220,
        320,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
}
