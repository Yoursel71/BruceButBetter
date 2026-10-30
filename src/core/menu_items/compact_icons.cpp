#include "compact_icons.h"

#if defined(UI_COMPACT) && UI_COMPACT
#include <globals.h>

namespace {
// Maps 24 x 24 design units to the requested cell size. At size 24 every value is unchanged.
struct IconPen {
    int x0;
    int y0;
    int size;
    int s(int v) const { return (v * size + 12) / 24; }
    int x(int v) const { return x0 + s(v); }
    int y(int v) const { return y0 + s(v); }
    // Span that starts at design unit `a`: both ends are scaled, so adjoining shapes stay joined.
    int len(int a, int n) const {
        const int l = s(a + n) - s(a);
        return l > 0 ? l : 1;
    }
    int r(int v) const {
        const int scaled = s(v);
        return scaled > 0 ? scaled : 1;
    }
    // Inner radius of a ring; the ring keeps at least one pixel of thickness when scaled down.
    int ir(int outer, int inner) const {
        const int o = r(outer);
        const int i = s(inner);
        return i < o ? i : o - 1;
    }
    float w(float v) const { return v * size / 24.0f; }
};
} // namespace

void drawCompactMenuIcon(CompactMenuIcon icon, int centerX, int centerY, uint16_t fg, uint16_t bg, int size) {
    if (size < 8) size = 8;
    const IconPen p{centerX - size / 2, centerY - size / 2, size};
    switch (icon) {
        case CompactMenuIcon::Wifi:
            tft.drawArc(p.x(12), p.y(20), p.r(17), p.ir(17, 15), 138, 222, fg, bg, false);
            tft.drawArc(p.x(12), p.y(20), p.r(11), p.ir(11, 9), 138, 222, fg, bg, false);
            tft.fillCircle(p.x(12), p.y(20), p.r(2), fg);
            break;
        case CompactMenuIcon::Bluetooth:
            tft.drawLine(p.x(10), p.y(2), p.x(10), p.y(21), fg);
            tft.drawLine(p.x(10), p.y(2), p.x(18), p.y(8), fg);
            tft.drawLine(p.x(18), p.y(8), p.x(4), p.y(18), fg);
            tft.drawLine(p.x(4), p.y(6), p.x(18), p.y(16), fg);
            tft.drawLine(p.x(18), p.y(16), p.x(10), p.y(21), fg);
            break;
        case CompactMenuIcon::Radio:
            // Ring radii leave a gap that survives scaling to 16 px.
            tft.fillCircle(p.x(12), p.y(8), p.r(2), fg);
            tft.drawArc(p.x(12), p.y(8), p.r(6), p.ir(6, 5), 45, 135, fg, bg, false);
            tft.drawArc(p.x(12), p.y(8), p.r(11), p.ir(11, 10), 45, 135, fg, bg, false);
            tft.drawArc(p.x(12), p.y(8), p.r(6), p.ir(6, 5), 225, 315, fg, bg, false);
            tft.drawArc(p.x(12), p.y(8), p.r(11), p.ir(11, 10), 225, 315, fg, bg, false);
            tft.drawLine(p.x(12), p.y(10), p.x(12), p.y(21), fg);
            tft.drawTriangle(p.x(12), p.y(12), p.x(7), p.y(22), p.x(17), p.y(22), fg);
            break;
        case CompactMenuIcon::LoRa:
            tft.drawRoundRect(p.x(5), p.y(5), p.len(5, 14), p.len(5, 14), p.r(2), fg);
            tft.drawRect(p.x(9), p.y(9), p.len(9, 6), p.len(9, 6), fg);
            tft.drawFastHLine(p.x(1), p.y(9), p.len(1, 4), fg);
            tft.drawFastHLine(p.x(1), p.y(15), p.len(1, 4), fg);
            tft.drawFastHLine(p.x(19), p.y(9), p.len(19, 4), fg);
            tft.drawFastHLine(p.x(19), p.y(15), p.len(19, 4), fg);
            tft.drawFastVLine(p.x(9), p.y(1), p.len(1, 4), fg);
            tft.drawFastVLine(p.x(15), p.y(1), p.len(1, 4), fg);
            tft.drawFastVLine(p.x(9), p.y(19), p.len(19, 4), fg);
            tft.drawFastVLine(p.x(15), p.y(19), p.len(19, 4), fg);
            break;
        case CompactMenuIcon::Nfc:
            tft.drawRoundRect(p.x(3), p.y(2), p.len(3, 18), p.len(2, 20), p.r(3), fg);
            tft.fillRect(p.x(2), p.y(13), p.len(2, 10), p.len(13, 10), bg);
            tft.fillRect(p.x(15), p.y(5), p.len(15, 3), p.len(5, 3), fg);
            tft.drawCircle(p.x(6), p.y(18), p.r(2), fg);
            tft.drawArc(p.x(6), p.y(18), p.r(6), p.ir(6, 5), 180, 270, fg, bg, false);
            tft.drawArc(p.x(6), p.y(18), p.r(10), p.ir(10, 9), 180, 270, fg, bg, false);
            break;
        case CompactMenuIcon::Infrared:
            tft.drawRoundRect(p.x(1), p.y(4), p.len(1, 7), p.len(4, 17), p.r(2), fg);
            tft.fillRect(p.x(3), p.y(7), p.len(3, 3), p.len(7, 2), fg);
            tft.fillRect(p.x(3), p.y(12), p.len(3, 3), p.len(12, 2), fg);
            tft.fillRect(p.x(3), p.y(17), p.len(3, 3), p.len(17, 2), fg);
            // Two beams: three rings merge into one block at 16 px.
            tft.drawArc(p.x(12), p.y(12), p.r(5), p.ir(5, 4), 220, 320, fg, bg, false);
            tft.drawArc(p.x(12), p.y(12), p.r(10), p.ir(10, 9), 220, 320, fg, bg, false);
            break;
        case CompactMenuIcon::Tools:
            tft.drawWideLine(p.x(5), p.y(18), p.x(17), p.y(6), p.w(4), fg, bg);
            tft.fillCircle(p.x(17), p.y(6), p.r(5), fg);
            tft.fillTriangle(p.x(16), p.y(0), p.x(23), p.y(7), p.x(15), p.y(8), bg);
            tft.fillCircle(p.x(5), p.y(18), p.r(4), fg);
            tft.fillCircle(p.x(5), p.y(18), p.r(1), bg);
            break;
        case CompactMenuIcon::Nrf24:
            // Two antennas on one board: the build carries two NRF24 radios.
            tft.drawRect(p.x(2), p.y(15), p.len(2, 20), p.len(15, 7), fg);
            tft.fillRect(p.x(9), p.y(17), p.len(9, 6), p.len(17, 3), fg);
            tft.drawFastVLine(p.x(6), p.y(5), p.len(5, 10), fg);
            tft.drawLine(p.x(6), p.y(8), p.x(2), p.y(3), fg);
            tft.drawLine(p.x(6), p.y(8), p.x(10), p.y(3), fg);
            tft.drawFastVLine(p.x(17), p.y(5), p.len(5, 10), fg);
            tft.drawLine(p.x(17), p.y(8), p.x(13), p.y(3), fg);
            tft.drawLine(p.x(17), p.y(8), p.x(21), p.y(3), fg);
            break;
        case CompactMenuIcon::Signal:
            tft.drawRoundRect(p.x(1), p.y(4), p.len(1, 18), p.len(4, 16), p.r(2), fg);
            tft.drawLine(p.x(3), p.y(12), p.x(5), p.y(8), fg);
            tft.drawLine(p.x(5), p.y(8), p.x(8), p.y(8), fg);
            tft.drawLine(p.x(8), p.y(8), p.x(12), p.y(16), fg);
            tft.drawLine(p.x(12), p.y(16), p.x(15), p.y(16), fg);
            tft.drawLine(p.x(15), p.y(16), p.x(17), p.y(12), fg);
            tft.drawFastHLine(p.x(19), p.y(8), p.len(19, 5), fg);
            tft.drawFastHLine(p.x(19), p.y(12), p.len(19, 5), fg);
            tft.drawFastHLine(p.x(19), p.y(16), p.len(19, 5), fg);
            break;
        case CompactMenuIcon::Files:
            // Folder: tab, body and the front flap line.
            tft.fillRect(p.x(2), p.y(4), p.len(2, 9), p.len(4, 4), fg);
            tft.drawRoundRect(p.x(2), p.y(7), p.len(2, 20), p.len(7, 14), p.r(2), fg);
            tft.drawFastHLine(p.x(2), p.y(11), p.len(2, 20), fg);
            break;
        case CompactMenuIcon::Scripts:
            tft.drawRoundRect(p.x(2), p.y(1), p.len(2, 20), p.len(1, 22), p.r(2), fg);
            tft.drawFastHLine(p.x(5), p.y(5), p.len(5, 14), fg);
            tft.drawLine(p.x(7), p.y(10), p.x(4), p.y(13), fg);
            tft.drawLine(p.x(4), p.y(13), p.x(7), p.y(16), fg);
            tft.drawLine(p.x(17), p.y(10), p.x(20), p.y(13), fg);
            tft.drawLine(p.x(20), p.y(13), p.x(17), p.y(16), fg);
            tft.drawLine(p.x(14), p.y(9), p.x(10), p.y(18), fg);
            break;
        case CompactMenuIcon::Diagnostics:
            tft.drawRoundRect(p.x(3), p.y(3), p.len(3, 18), p.len(3, 20), p.r(2), fg);
            tft.fillRect(p.x(8), p.y(0), p.len(8, 8), p.len(0, 4), fg);
            tft.drawLine(p.x(6), p.y(8), p.x(8), p.y(10), fg);
            tft.drawLine(p.x(8), p.y(10), p.x(10), p.y(7), fg);
            tft.drawFastHLine(p.x(13), p.y(9), p.len(13, 5), fg);
            tft.drawLine(p.x(6), p.y(14), p.x(8), p.y(16), fg);
            tft.drawLine(p.x(8), p.y(16), p.x(10), p.y(13), fg);
            tft.drawFastHLine(p.x(13), p.y(15), p.len(13, 5), fg);
            tft.drawFastHLine(p.x(6), p.y(20), p.len(6, 12), fg);
            break;
        case CompactMenuIcon::Clock:
            tft.drawCircle(p.x(12), p.y(12), p.r(10), fg);
            tft.drawLine(p.x(12), p.y(4), p.x(12), p.y(5), fg);
            tft.drawLine(p.x(19), p.y(12), p.x(20), p.y(12), fg);
            tft.drawLine(p.x(12), p.y(19), p.x(12), p.y(20), fg);
            tft.drawLine(p.x(4), p.y(12), p.x(5), p.y(12), fg);
            tft.drawLine(p.x(12), p.y(12), p.x(12), p.y(7), fg);
            tft.drawLine(p.x(12), p.y(12), p.x(17), p.y(15), fg);
            tft.fillCircle(p.x(12), p.y(12), p.r(1), fg);
            break;
        case CompactMenuIcon::Settings:
            tft.fillRect(p.x(10), p.y(1), p.len(10, 5), p.len(1, 5), fg);
            tft.fillRect(p.x(10), p.y(19), p.len(10, 5), p.len(19, 4), fg);
            tft.fillRect(p.x(1), p.y(10), p.len(1, 5), p.len(10, 5), fg);
            tft.fillRect(p.x(19), p.y(10), p.len(19, 4), p.len(10, 5), fg);
            tft.fillRect(p.x(4), p.y(4), p.len(4, 4), p.len(4, 4), fg);
            tft.fillRect(p.x(17), p.y(4), p.len(17, 4), p.len(4, 4), fg);
            tft.fillRect(p.x(4), p.y(17), p.len(4, 4), p.len(17, 4), fg);
            tft.fillRect(p.x(17), p.y(17), p.len(17, 4), p.len(17, 4), fg);
            tft.fillCircle(p.x(12), p.y(12), p.r(8), fg);
            tft.fillCircle(p.x(12), p.y(12), p.r(5), bg);
            tft.drawCircle(p.x(12), p.y(12), p.r(2), fg);
            break;
        case CompactMenuIcon::Fm:
            tft.drawRoundRect(p.x(1), p.y(7), p.len(1, 22), p.len(7, 15), p.r(2), fg);
            tft.drawLine(p.x(5), p.y(7), p.x(17), p.y(1), fg);
            tft.drawCircle(p.x(17), p.y(14), p.r(3), fg);
            tft.drawFastVLine(p.x(5), p.y(11), p.len(11, 7), fg);
            tft.drawFastVLine(p.x(8), p.y(11), p.len(11, 7), fg);
            tft.drawFastVLine(p.x(11), p.y(11), p.len(11, 7), fg);
            break;
        case CompactMenuIcon::Ethernet:
            tft.drawRect(p.x(3), p.y(2), p.len(3, 18), p.len(2, 14), fg);
            tft.drawRect(p.x(7), p.y(6), p.len(7, 10), p.len(6, 6), fg);
            tft.drawFastVLine(p.x(10), p.y(3), p.len(3, 4), fg);
            tft.drawFastVLine(p.x(13), p.y(3), p.len(3, 4), fg);
            tft.drawFastVLine(p.x(12), p.y(16), p.len(16, 6), fg);
            tft.drawFastHLine(p.x(3), p.y(21), p.len(3, 19), fg);
            break;
        case CompactMenuIcon::Gps:
            tft.fillCircle(p.x(12), p.y(8), p.r(7), fg);
            tft.fillTriangle(p.x(6), p.y(11), p.x(18), p.y(11), p.x(12), p.y(23), fg);
            tft.fillCircle(p.x(12), p.y(8), p.r(3), bg);
            break;
        case CompactMenuIcon::Connect:
            tft.drawLine(p.x(5), p.y(12), p.x(19), p.y(4), fg);
            tft.drawLine(p.x(5), p.y(12), p.x(19), p.y(20), fg);
            tft.drawLine(p.x(19), p.y(4), p.x(19), p.y(20), fg);
            tft.fillCircle(p.x(4), p.y(12), p.r(3), fg);
            tft.fillCircle(p.x(19), p.y(4), p.r(3), fg);
            tft.fillCircle(p.x(19), p.y(20), p.r(3), fg);
            break;
    }
}
#endif
