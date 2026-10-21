#pragma once

#if defined(UI_COMPACT) && UI_COMPACT
#include <stdint.h>

// Every symbol is drawn directly in a 24 x 24 pixel cell, including in the WebUI Navigator.
enum class CompactMenuIcon : uint8_t {
    Wifi, Bluetooth, Radio, Nfc, Infrared, Tools, Nrf24, Signal,
    Files, Scripts, Diagnostics, Clock, Settings, LoRa, Fm, Ethernet, Gps, Connect
};

void drawCompactMenuIcon(CompactMenuIcon icon, int centerX, int centerY, uint16_t fg, uint16_t bg);
#endif
