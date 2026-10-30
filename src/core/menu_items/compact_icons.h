#pragma once

#if defined(UI_COMPACT) && UI_COMPACT
#include <stdint.h>

// Every symbol is drawn with standard primitives (no bitmaps), so the WebUI Navigator shows it too.
// Icons are designed on a 24 x 24 grid; `size` scales them, e.g. 16 for the main menu icon grid.
enum class CompactMenuIcon : uint8_t {
    Wifi, Bluetooth, Radio, Nfc, Infrared, Tools, Nrf24, Signal,
    Files, Scripts, Diagnostics, Clock, Settings, LoRa, Fm, Ethernet, Gps, Connect
};

void drawCompactMenuIcon(
    CompactMenuIcon icon, int centerX, int centerY, uint16_t fg, uint16_t bg, int size = 24
);
#endif
