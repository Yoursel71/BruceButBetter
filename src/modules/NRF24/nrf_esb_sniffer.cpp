/**
 * @file nrf_esb_sniffer.cpp
 * @brief Passive Enhanced ShockBurst sniffer for 2.4 GHz wireless peripherals.
 *
 * Technique: sets NRF24 address width=2, disables CRC/ACK, listens on
 * the 0xAA prefix (preamble detection trick). Any ESB-format packet on
 * any channel is captured and displayed as raw bytes.
 *
 * Identifies: Logitech (addr prefix ~0x40-0x5F), Microsoft, generic.
 */
#include "nrf_esb_sniffer.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include <globals.h>

#define ESB_RING_SIZE 4

struct EsbPkt {
    uint8_t  ch;
    uint8_t  data[10]; // show first 10 bytes
    uint32_t ts;
};

static EsbPkt ring[ESB_RING_SIZE];
static uint8_t ringHead = 0;
static uint8_t ringFill = 0;
static uint32_t totalPkts = 0;

// Guess device type from first address byte
static const char *guessDevice(uint8_t b0) {
    if (b0 >= 0x40 && b0 <= 0x5F) return "Logi";
    if (b0 >= 0xA0 && b0 <= 0xBF) return "MSFT";
    if (b0 >= 0xC0 && b0 <= 0xCF) return "KBRD";
    if (b0 == 0xAA || b0 == 0x55) return "NOIS";
    return "UNKN";
}

static void setupPromiscuous() {
    NRFradio.setAutoAck(false);
    NRFradio.disableCRC();
    NRFradio.setAddressWidth(2);
    NRFradio.setPayloadSize(32);
    NRFradio.setDataRate(RF24_2MBPS);
    NRFradio.setPALevel(RF24_PA_MAX);
    NRFradio.setRetries(0, 0);

    // Address 0xAA 0xAA matches the alternating-bit ESB preamble
    const uint8_t addr[] = {0xAA, 0xAA};
    NRFradio.openReadingPipe(1, addr);
    NRFradio.startListening();
}

static void drawESBui(uint8_t curCh) {
    tft.fillScreen(bruceConfig.bgColor);
    tft.setTextSize(FP);

    // Title + stats
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    tft.drawCentreString("ESB SNIFFER", tftWidth / 2, 0, 1);

    char buf[40];
    tft.setTextColor(TFT_YELLOW, bruceConfig.bgColor);
    snprintf(buf, sizeof(buf), "CH:%3d  PKT:%lu", curCh, totalPkts);
    tft.drawString(buf, 2, 9, 1);

    // Divider
    tft.drawFastHLine(0, 18, tftWidth, bruceConfig.priColor);

    // Packet list (newest first)
    int y = 21;
    int lineH = 9;
    for (int i = 0; i < (int)min((uint8_t)ESB_RING_SIZE, ringFill); i++) {
        int idx = ((int)ringHead - 1 - i + ESB_RING_SIZE) % ESB_RING_SIZE;
        EsbPkt &p = ring[idx];

        const char *dev = guessDevice(p.data[0]);
        snprintf(buf, sizeof(buf), "%s C%02d %02X%02X%02X%02X%02X",
                 dev, p.ch,
                 p.data[0], p.data[1], p.data[2], p.data[3], p.data[4]);

        tft.setTextColor(i == 0 ? TFT_GREEN : TFT_DARKGREY, bruceConfig.bgColor);
        tft.drawString(buf, 2, y, 1);
        y += lineH;
        if (y > tftHeight - lineH - 2) break;
    }

    // Footer
    tft.setTextColor(TFT_DARKGREY, bruceConfig.bgColor);
    tft.drawCentreString("[ESC] Stop", tftWidth / 2, tftHeight - 9, 1);
}

void nrf_esb_sniffer() {
    NRF24_MODE mode = nrf_setMode();
    if (returnToMenu || mode == NRF_MODE_DISABLED) return;

    if (!nrf_start(mode)) {
        displayError("NRF24 not found");
        delay(500);
        return;
    }
    if (!CHECK_NRF_SPI(mode)) {
        displayError("Needs SPI mode");
        delay(500);
        return;
    }

    ringHead = 0; ringFill = 0; totalPkts = 0;
    setupPromiscuous();

    uint8_t curCh = 0;
    unsigned long lastDraw = 0;
    bool redraw = true;

    while (true) {
        if (check(EscPress)) break;

        NRFradio.setChannel(curCh);
        delayMicroseconds(400);

        if (NRFradio.available()) {
            uint8_t buf[32];
            NRFradio.read(buf, 32);

            // Filter obvious noise (all same byte)
            bool noise = true;
            for (int i = 1; i < 8; i++) {
                if (buf[i] != buf[0]) { noise = false; break; }
            }
            if (!noise) {
                EsbPkt &p = ring[ringHead];
                p.ch = curCh;
                p.ts = millis();
                memcpy(p.data, buf, 10);
                ringHead = (ringHead + 1) % ESB_RING_SIZE;
                if (ringFill < ESB_RING_SIZE) ringFill++;
                totalPkts++;
                redraw = true;
            }
        }

        curCh = (curCh + 1) % 84; // 0–83: covers BLE+ESB bands

        unsigned long now = millis();
        if (redraw || now - lastDraw >= 400) {
            drawESBui(curCh);
            lastDraw = now;
            redraw = false;
        }
    }

    NRFradio.stopListening();
    NRFradio.powerDown();
}
