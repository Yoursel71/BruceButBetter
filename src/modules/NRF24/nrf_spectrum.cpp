#include "nrf_spectrum.h"
#include "core/display.h"
#include "core/mykeyboard.h"

#define CHANNELS 80
#define RGB565(r, g, b) ((((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)))
uint8_t channel[CHANNELS];

// scanning channels
#define _BW tftWidth / CHANNELS
String scanChannels(bool web, bool dual) {
    String result = "{";

    uint8_t rpdValues[CHANNELS] = {0};
    digitalWrite(bruceConfigPins.NRF24_bus.io0, LOW);

    int step = 1;
#ifdef NRF24_2_CE_PIN
    if (dual) step = 2; // radio1 takes even channels, radio2 the odd ones
#else
    (void)dual;
#endif

    for (int i = 0; i < CHANNELS; i += step) {
        NRFradio.setChannel(i);
        NRFradio.startListening();
#ifdef NRF24_2_CE_PIN
        bool useR2 = dual && (i + 1 < CHANNELS);
        if (useR2) {
            NRFradio2.setChannel(i + 1);
            NRFradio2.startListening();
        }
#endif
        delayMicroseconds(128);
        NRFradio.stopListening();

        int rpd = NRFradio.testRPD() ? 1 : 0;
        channel[i] = (channel[i] * 3 + rpd * 125) / 4;
        rpdValues[i] = channel[i];

#ifdef NRF24_2_CE_PIN
        if (useR2) {
            NRFradio2.stopListening();
            int rpd2 = NRFradio2.testRPD() ? 1 : 0;
            channel[i + 1] = (channel[i + 1] * 3 + rpd2 * 125) / 4;
            rpdValues[i + 1] = channel[i + 1];
        }
#endif
    }

    digitalWrite(bruceConfigPins.NRF24_bus.io0, HIGH);

    for (int i = 0; i < CHANNELS; i++) {
        int level = rpdValues[i];
        int x = i * _BW;
        int c = i;

        tft.drawFastVLine(
            x, tftHeight - (10 + level), level, (i % 2 == 0) ? bruceConfig.priColor : TFT_DARKGREY
        ); // for level display

        tft.drawFastVLine(
            x, 0, tftHeight - (9 + level), (i % 8) ? TFT_BLACK : RGB565(25, 25, 25)
        );                                                    /// for clearing
        tft.drawFastVLine(x, 0, level, bruceConfig.secColor); /// for top display
        // show 5 channel gap only
        if (c % 5 == 0 && c != 0) { tft.drawCentreString(String(c).c_str(), x, tftHeight / 2, 1); }

        if (web) {
            if (i > 0) result += ",";
            result += String(level);
        }
    }

    if (web) result += "}";
    return result; // return a string in this format "{1,32,45,32,84,32 .... 12,54,65}" with 80 values to be
                   // used in the WebUI (Future)
}

void nrf_spectrum() {
    tft.fillScreen(bruceConfig.bgColor);
    tft.setTextSize(FP);
    tft.drawString("2.40Ghz", 0, tftHeight - LH);
    tft.drawCentreString("2.44Ghz", tftWidth / 2, tftHeight - LH, 1);
    tft.drawRightString("2.48Ghz", tftWidth, tftHeight - LH, 1);

    if (nrf_start(NRF_MODE_SPI)) { // This function only works on SPI
        NRFradio.setAutoAck(false);
        NRFradio.disableCRC();       // accept any signal we find
        NRFradio.setAddressWidth(2); // a reverse engineering tactic (not typically recommended)
        const uint8_t noiseAddress[][2] = {
            {0x55, 0x55},
            {0xAA, 0xAA},
            {0xA0, 0xAA},
            {0xAB, 0xAA},
            {0xAC, 0xAA},
            {0xAD, 0xAA}
        };
        for (uint8_t i = 0; i < 6; ++i) { NRFradio.openReadingPipe(i, noiseAddress[i]); }
        NRFradio.setDataRate(RF24_1MBPS);

        bool dual = false;
#ifdef NRF24_2_CE_PIN
        // Bring up the second radio (if populated) so the sweep can scan two
        // channels at once. NOTE: hardware-untested — validate on a real dual-NRF24
        // board; falls back to single-radio automatically when radio2 is absent.
        if (nrf_start2()) {
            NRFradio2.setAutoAck(false);
            NRFradio2.disableCRC();
            NRFradio2.setAddressWidth(2);
            for (uint8_t i = 0; i < 6; ++i) { NRFradio2.openReadingPipe(i, noiseAddress[i]); }
            NRFradio2.setDataRate(RF24_1MBPS);
            dual = true;
            tft.setTextSize(FP);
            tft.setTextColor(bruceConfig.secColor, bruceConfig.bgColor);
            tft.drawString("2x", tftWidth - 18, 2);
        }
#endif

        while (!check(EscPress)) { scanChannels(false, dual); }
        NRFradio.stopListening();
        NRFradio.powerDown();
#ifdef NRF24_2_CE_PIN
        if (dual) {
            NRFradio2.stopListening();
            NRFradio2.powerDown();
        }
#endif
        delay(250);
        return;

    } else {
        Serial.println("Fail Starting radio");
        displayError("NRF24 not found");
        delay(500);
        return;
    }
}
