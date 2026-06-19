/**
 * @file ble_tracker.cpp
 * @brief BLE tracker detector — find AirTag, SmartTag, Tile, Chipolo nearby.
 *
 * Passively scans BLE advertisements and classifies devices by their
 * manufacturer company ID and advertisement payload signatures.
 */
#if !defined(LITE_VERSION)
#include "ble_tracker.h"
#include "ble_common.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include <globals.h>

#define MAX_TRACKERS 12

enum TrackerType : uint8_t {
    T_AIRTAG = 0,
    T_APPLE_FINDMY,
    T_SAMSUNG,
    T_TILE,
    T_CHIPOLO,
    T_NUT,
    T_UNKNOWN_TRACKER
};

static const char *TRACKER_NAMES[] = {
    "AirTag",
    "FindMy",
    "SmartTag",
    "Tile",
    "Chipolo",
    "Nut",
    "Unknown"
};

struct TrackerEntry {
    TrackerType type;
    char        mac[18];
    int8_t      rssi;
    uint32_t    lastSeen;
    bool        active;
};

static TrackerEntry trackers[MAX_TRACKERS];
static uint8_t trackerCount = 0;
static volatile bool newEntry = false;

static void addOrUpdate(TrackerType t, const char *mac, int8_t rssi) {
    for (int i = 0; i < trackerCount; i++) {
        if (strcmp(trackers[i].mac, mac) == 0) {
            trackers[i].rssi     = rssi;
            trackers[i].lastSeen = millis();
            newEntry = true;
            return;
        }
    }
    if (trackerCount < MAX_TRACKERS) {
        TrackerEntry &e = trackers[trackerCount++];
        e.type     = t;
        e.rssi     = rssi;
        e.lastSeen = millis();
        e.active   = true;
        strncpy(e.mac, mac, 17);
        e.mac[17] = '\0';
        newEntry   = true;
    }
}

class TrackerCallbacks : public NimBLEScanCallbacks {
    void onResult(const NimBLEAdvertisedDevice *dev) override {
        if (!dev->haveManufacturerData()) return;

        std::string mfr = dev->getManufacturerData();
        if (mfr.size() < 2) return;

        uint16_t cid = (uint8_t)mfr[0] | ((uint8_t)mfr[1] << 8);
        std::string mac = dev->getAddress().toString();
        int8_t rssi     = dev->getRSSI();

        // Apple: company 0x004C
        if (cid == 0x004C && mfr.size() >= 3) {
            uint8_t subtype = (uint8_t)mfr[2];
            if (subtype == 0x12) {
                addOrUpdate(T_AIRTAG, mac.c_str(), rssi);
            } else if (subtype == 0x0F) {
                addOrUpdate(T_APPLE_FINDMY, mac.c_str(), rssi);
            }
        }
        // Samsung: company 0x0075
        else if (cid == 0x0075) {
            addOrUpdate(T_SAMSUNG, mac.c_str(), rssi);
        }
        // Tile: company 0x0178
        else if (cid == 0x0178) {
            addOrUpdate(T_TILE, mac.c_str(), rssi);
        }
        // Chipolo: company 0x01F7
        else if (cid == 0x01F7) {
            addOrUpdate(T_CHIPOLO, mac.c_str(), rssi);
        }
        // Nut: company 0x0271
        else if (cid == 0x0271) {
            addOrUpdate(T_NUT, mac.c_str(), rssi);
        }
    }
};

static void drawTrackerUI(bool initial) {
    if (initial) {
        tft.fillScreen(bruceConfig.bgColor);
        tft.setTextSize(FP);
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.drawCentreString("TRACKER DETECT", tftWidth / 2, 0, 1);
        tft.drawFastHLine(0, 9, tftWidth, bruceConfig.priColor);
    }

    // Clear body
    tft.fillRect(0, 11, tftWidth, tftHeight - 20, bruceConfig.bgColor);
    tft.setTextSize(FP);

    if (trackerCount == 0) {
        tft.setTextColor(TFT_DARKGREY, bruceConfig.bgColor);
        tft.drawCentreString("No trackers found", tftWidth / 2, tftHeight / 2 - 4, 1);
        tft.drawCentreString("Scanning...", tftWidth / 2, tftHeight / 2 + 5, 1);
    } else {
        int y = 13;
        int lh = 9;
        for (int i = 0; i < trackerCount && y < tftHeight - lh; i++) {
            TrackerEntry &e = trackers[i];
            uint32_t age = (millis() - e.lastSeen) / 1000;

            // Colour by recency
            uint16_t col = (age < 5) ? TFT_GREEN : (age < 30) ? TFT_YELLOW : TFT_DARKGREY;
            tft.setTextColor(col, bruceConfig.bgColor);

            char buf[32];
            snprintf(buf, sizeof(buf), "%-8s %ddBm %lus",
                     TRACKER_NAMES[e.type], e.rssi, (unsigned long)age);
            tft.drawString(buf, 2, y, 1);
            y += lh;

            // MAC (truncated)
            tft.setTextColor(TFT_DARKGREY, bruceConfig.bgColor);
            snprintf(buf, sizeof(buf), "%.11s", e.mac);
            tft.drawString(buf, 2, y, 1);
            y += lh + 1;
        }
    }

    // Footer
    tft.setTextColor(TFT_DARKGREY, bruceConfig.bgColor);
    char fbuf[24];
    snprintf(fbuf, sizeof(fbuf), "Found:%d  [ESC]Stop", trackerCount);
    tft.drawCentreString(fbuf, tftWidth / 2, tftHeight - 9, 1);
}

void ble_tracker_detect() {
    trackerCount = 0;
    newEntry     = false;
    memset(trackers, 0, sizeof(trackers));

    stopBLEStack();
    delay(100);
    BLEDevice::init("");
    BLEScan *scan = BLEDevice::getScan();
    scan->setScanCallbacks(new TrackerCallbacks(), false);
    scan->setActiveScan(false);
    scan->setInterval(40);
    scan->setWindow(30);
    scan->start(0, false, false); // continuous

    drawTrackerUI(true);
    unsigned long lastDraw = 0;

    while (true) {
        if (check(EscPress)) break;

        unsigned long now = millis();
        if (newEntry || now - lastDraw >= 1000) {
            drawTrackerUI(false);
            lastDraw = now;
            newEntry  = false;
        }
        delay(50);
    }

    scan->stop();
    stopBLEStack();
}
#endif
