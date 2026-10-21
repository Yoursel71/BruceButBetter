#include "compact_icons.h"

#if defined(UI_COMPACT) && UI_COMPACT
#include <globals.h>

void drawCompactMenuIcon(CompactMenuIcon icon, int centerX, int centerY, uint16_t fg, uint16_t bg) {
    const int x = centerX - 12;
    const int y = centerY - 12;
    switch (icon) {
        case CompactMenuIcon::Wifi:
            tft.drawArc(x + 12, y + 20, 17, 15, 138, 222, fg, bg, false);
            tft.drawArc(x + 12, y + 20, 11, 9, 138, 222, fg, bg, false);
            tft.fillCircle(x + 12, y + 20, 2, fg);
            break;
        case CompactMenuIcon::Bluetooth:
            tft.drawLine(x + 10, y + 2, x + 10, y + 21, fg);
            tft.drawLine(x + 10, y + 2, x + 18, y + 8, fg);
            tft.drawLine(x + 18, y + 8, x + 4, y + 18, fg);
            tft.drawLine(x + 4, y + 6, x + 18, y + 16, fg);
            tft.drawLine(x + 18, y + 16, x + 10, y + 21, fg);
            break;
        case CompactMenuIcon::Radio:
            tft.fillCircle(x + 12, y + 8, 2, fg);
            tft.drawArc(x + 12, y + 8, 7, 6, 45, 135, fg, bg, false);
            tft.drawArc(x + 12, y + 8, 11, 10, 45, 135, fg, bg, false);
            tft.drawArc(x + 12, y + 8, 7, 6, 225, 315, fg, bg, false);
            tft.drawArc(x + 12, y + 8, 11, 10, 225, 315, fg, bg, false);
            tft.drawLine(x + 12, y + 10, x + 12, y + 21, fg);
            tft.drawTriangle(x + 12, y + 12, x + 7, y + 22, x + 17, y + 22, fg);
            break;
        case CompactMenuIcon::LoRa:
            tft.drawRoundRect(x + 5, y + 5, 14, 14, 2, fg);
            tft.drawRect(x + 9, y + 9, 6, 6, fg);
            tft.drawFastHLine(x + 1, y + 9, 4, fg);
            tft.drawFastHLine(x + 1, y + 15, 4, fg);
            tft.drawFastHLine(x + 19, y + 9, 4, fg);
            tft.drawFastHLine(x + 19, y + 15, 4, fg);
            tft.drawFastVLine(x + 9, y + 1, 4, fg);
            tft.drawFastVLine(x + 15, y + 1, 4, fg);
            tft.drawFastVLine(x + 9, y + 19, 4, fg);
            tft.drawFastVLine(x + 15, y + 19, 4, fg);
            break;
        case CompactMenuIcon::Nfc:
            tft.drawRoundRect(x + 3, y + 2, 18, 20, 3, fg);
            tft.fillRect(x + 2, y + 13, 10, 10, bg);
            tft.fillRect(x + 15, y + 5, 3, 3, fg);
            tft.drawCircle(x + 6, y + 18, 2, fg);
            tft.drawArc(x + 6, y + 18, 6, 5, 180, 270, fg, bg, false);
            tft.drawArc(x + 6, y + 18, 10, 9, 180, 270, fg, bg, false);
            break;
        case CompactMenuIcon::Infrared:
            tft.drawRoundRect(x + 1, y + 4, 7, 17, 2, fg);
            tft.fillRect(x + 3, y + 7, 3, 2, fg);
            tft.fillRect(x + 3, y + 12, 3, 2, fg);
            tft.fillRect(x + 3, y + 17, 3, 2, fg);
            tft.drawArc(x + 12, y + 12, 4, 3, 220, 320, fg, bg, false);
            tft.drawArc(x + 12, y + 12, 8, 7, 220, 320, fg, bg, false);
            tft.drawArc(x + 12, y + 12, 11, 10, 220, 320, fg, bg, false);
            break;
        case CompactMenuIcon::Tools:
            tft.drawWideLine(x + 5, y + 18, x + 17, y + 6, 4, fg, bg);
            tft.fillCircle(x + 17, y + 6, 5, fg);
            tft.fillTriangle(x + 16, y, x + 23, y + 7, x + 15, y + 8, bg);
            tft.fillCircle(x + 5, y + 18, 4, fg);
            tft.fillCircle(x + 5, y + 18, 1, bg);
            break;
        case CompactMenuIcon::Nrf24:
            tft.drawRect(x + 3, y + 7, 14, 15, fg);
            tft.drawRect(x + 6, y + 11, 7, 7, fg);
            tft.fillRect(x + 1, y + 10, 2, 2, fg);
            tft.fillRect(x + 1, y + 14, 2, 2, fg);
            tft.fillRect(x + 1, y + 18, 2, 2, fg);
            tft.drawLine(x + 16, y + 14, x + 21, y + 14, fg);
            tft.drawLine(x + 21, y + 14, x + 21, y + 2, fg);
            tft.drawLine(x + 21, y + 2, x + 15, y + 2, fg);
            break;
        case CompactMenuIcon::Signal:
            tft.drawRoundRect(x + 1, y + 4, 18, 16, 2, fg);
            tft.drawLine(x + 3, y + 12, x + 5, y + 8, fg);
            tft.drawLine(x + 5, y + 8, x + 8, y + 8, fg);
            tft.drawLine(x + 8, y + 8, x + 12, y + 16, fg);
            tft.drawLine(x + 12, y + 16, x + 15, y + 16, fg);
            tft.drawLine(x + 15, y + 16, x + 17, y + 12, fg);
            tft.drawFastHLine(x + 19, y + 8, 5, fg);
            tft.drawFastHLine(x + 19, y + 12, 5, fg);
            tft.drawFastHLine(x + 19, y + 16, 5, fg);
            break;
        case CompactMenuIcon::Files:
            tft.drawRect(x + 9, y + 1, 12, 15, fg);
            tft.fillRect(x + 6, y + 5, 12, 15, bg);
            tft.drawRect(x + 6, y + 5, 12, 15, fg);
            tft.fillRect(x + 2, y + 9, 12, 14, bg);
            tft.drawRect(x + 2, y + 9, 12, 14, fg);
            tft.drawLine(x + 5, y + 13, x + 10, y + 13, fg);
            tft.drawLine(x + 5, y + 17, x + 10, y + 17, fg);
            break;
        case CompactMenuIcon::Scripts:
            tft.drawRoundRect(x + 2, y + 1, 20, 22, 2, fg);
            tft.drawFastHLine(x + 5, y + 5, 14, fg);
            tft.drawLine(x + 7, y + 10, x + 4, y + 13, fg);
            tft.drawLine(x + 4, y + 13, x + 7, y + 16, fg);
            tft.drawLine(x + 17, y + 10, x + 20, y + 13, fg);
            tft.drawLine(x + 20, y + 13, x + 17, y + 16, fg);
            tft.drawLine(x + 14, y + 9, x + 10, y + 18, fg);
            break;
        case CompactMenuIcon::Diagnostics:
            tft.drawRoundRect(x + 3, y + 3, 18, 20, 2, fg);
            tft.fillRect(x + 8, y, 8, 4, fg);
            tft.drawLine(x + 6, y + 8, x + 8, y + 10, fg);
            tft.drawLine(x + 8, y + 10, x + 10, y + 7, fg);
            tft.drawFastHLine(x + 13, y + 9, 5, fg);
            tft.drawLine(x + 6, y + 14, x + 8, y + 16, fg);
            tft.drawLine(x + 8, y + 16, x + 10, y + 13, fg);
            tft.drawFastHLine(x + 13, y + 15, 5, fg);
            tft.drawFastHLine(x + 6, y + 20, 12, fg);
            break;
        case CompactMenuIcon::Clock:
            tft.drawCircle(x + 12, y + 12, 10, fg);
            tft.drawLine(x + 12, y + 4, x + 12, y + 5, fg);
            tft.drawLine(x + 19, y + 12, x + 20, y + 12, fg);
            tft.drawLine(x + 12, y + 19, x + 12, y + 20, fg);
            tft.drawLine(x + 4, y + 12, x + 5, y + 12, fg);
            tft.drawLine(x + 12, y + 12, x + 12, y + 7, fg);
            tft.drawLine(x + 12, y + 12, x + 17, y + 15, fg);
            tft.fillCircle(x + 12, y + 12, 1, fg);
            break;
        case CompactMenuIcon::Settings:
            tft.fillRect(x + 10, y + 1, 5, 5, fg);
            tft.fillRect(x + 10, y + 19, 5, 4, fg);
            tft.fillRect(x + 1, y + 10, 5, 5, fg);
            tft.fillRect(x + 19, y + 10, 4, 5, fg);
            tft.fillRect(x + 4, y + 4, 4, 4, fg);
            tft.fillRect(x + 17, y + 4, 4, 4, fg);
            tft.fillRect(x + 4, y + 17, 4, 4, fg);
            tft.fillRect(x + 17, y + 17, 4, 4, fg);
            tft.fillCircle(x + 12, y + 12, 8, fg);
            tft.fillCircle(x + 12, y + 12, 5, bg);
            tft.drawCircle(x + 12, y + 12, 2, fg);
            break;
        case CompactMenuIcon::Fm:
            tft.drawRoundRect(x + 1, y + 7, 22, 15, 2, fg);
            tft.drawLine(x + 5, y + 7, x + 17, y + 1, fg);
            tft.drawCircle(x + 17, y + 14, 3, fg);
            tft.drawFastVLine(x + 5, y + 11, 7, fg);
            tft.drawFastVLine(x + 8, y + 11, 7, fg);
            tft.drawFastVLine(x + 11, y + 11, 7, fg);
            break;
        case CompactMenuIcon::Ethernet:
            tft.drawRect(x + 3, y + 2, 18, 14, fg);
            tft.drawRect(x + 7, y + 6, 10, 6, fg);
            tft.drawFastVLine(x + 10, y + 3, 4, fg);
            tft.drawFastVLine(x + 13, y + 3, 4, fg);
            tft.drawFastVLine(x + 12, y + 16, 6, fg);
            tft.drawFastHLine(x + 3, y + 21, 19, fg);
            break;
        case CompactMenuIcon::Gps:
            tft.fillCircle(x + 12, y + 8, 7, fg);
            tft.fillTriangle(x + 6, y + 11, x + 18, y + 11, x + 12, y + 23, fg);
            tft.fillCircle(x + 12, y + 8, 3, bg);
            break;
        case CompactMenuIcon::Connect:
            tft.drawLine(x + 5, y + 12, x + 19, y + 4, fg);
            tft.drawLine(x + 5, y + 12, x + 19, y + 20, fg);
            tft.drawLine(x + 19, y + 4, x + 19, y + 20, fg);
            tft.fillCircle(x + 4, y + 12, 3, fg);
            tft.fillCircle(x + 19, y + 4, 3, fg);
            tft.fillCircle(x + 19, y + 20, 3, fg);
            break;
    }
}
#endif
