<div align="center">

<img src="assets/banner.svg" alt="Bruce DIY Flipper Zero" width="100%">

# 🦈 Bruce DIY Flipper Zero — ESP32-S3 N16R8

**A do-it-yourself Flipper Zero built around the ESP32-S3 N16R8.**
Sub-GHz · NFC/RFID · IR · WiFi/BLE attacks · NRF24 · Si5351 signal generator — all in one device.

<br>

[![ESP32-S3](https://img.shields.io/badge/ESP32--S3-N16R8-E7352C?logo=espressif&logoColor=white)](https://www.espressif.com/en/products/socs/esp32-s3)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Arduino-FF7F00?logo=platformio&logoColor=white)](https://platformio.org/)
[![License: AGPL v3](https://img.shields.io/badge/License-AGPL%20v3-3fb950)](./LICENSE)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-2ea043?logo=github)](./CONTRIBUTING.md)

[![Stars](https://img.shields.io/github/stars/modabucksmain-pixel/Bruce-DIY-Flipper?style=flat&color=2ea043&logo=github)](https://github.com/modabucksmain-pixel/Bruce-DIY-Flipper/stargazers)
[![Forks](https://img.shields.io/github/forks/modabucksmain-pixel/Bruce-DIY-Flipper?style=flat&color=1f6feb&logo=github)](https://github.com/modabucksmain-pixel/Bruce-DIY-Flipper/network/members)
[![Issues](https://img.shields.io/github/issues/modabucksmain-pixel/Bruce-DIY-Flipper?color=f78166)](https://github.com/modabucksmain-pixel/Bruce-DIY-Flipper/issues)
[![Last commit](https://img.shields.io/github/last-commit/modabucksmain-pixel/Bruce-DIY-Flipper?color=8b949e)](https://github.com/modabucksmain-pixel/Bruce-DIY-Flipper/commits)

### [🌐 Web Flasher](https://modabucksmain-pixel.github.io/Bruce-DIY-Flipper/) · [📥 Releases](https://github.com/modabucksmain-pixel/Bruce-DIY-Flipper/releases) · [🤝 Contribute](./CONTRIBUTING.md) · [🐛 Report a bug](https://github.com/modabucksmain-pixel/Bruce-DIY-Flipper/issues/new/choose)

</div>

---

A do-it-yourself Flipper Zero built around the ESP32-S3 N16R8. This is the [Bruce](https://github.com/pr3y/Bruce) firmware adapted to this hardware. Sub-GHz, NFC/RFID, IR, WiFi/BLE attacks, NRF24, and a Si5351 signal generator — all in one device.

> ⚠️ **Legal notice:** This firmware is for **authorized** security testing and education only. Distributed under the AGPL license. Unauthorized or malicious use is prohibited. You assume all responsibility.

---

## 📦 Download / Flash

### 🌐 One-click from your browser (easiest)
**[→ Web Flasher: modabucksmain-pixel.github.io/Bruce-DIY-Flipper](https://modabucksmain-pixel.github.io/Bruce-DIY-Flipper/)**
Open in Chrome/Edge (desktop), connect the device via the **left USB-C**, hit **Flash**. No install needed.

### 📥 Manual
Latest builds: **[Releases → v1.0.0](https://github.com/modabucksmain-pixel/Bruce-DIY-Flipper/releases/tag/v1.0.0)**

#### Single file (merged — bootloader + partitions + boot_app0 + firmware, offset `0x0`)
```sh
esptool.py --chip esp32s3 --port COMx write_flash 0x0 Bruce-esp32-s3-N16R8.bin
```

#### Separate components
```sh
esptool.py --chip esp32s3 --port COMx write_flash \
  0x0      bootloader.bin \
  0x8000   partitions.bin \
  0xe000   boot_app0.bin \
  0x10000  firmware.bin
```

> On Linux/Mac use `/dev/ttyACM0` or `/dev/ttyUSB0` instead of `COMx`. Use the **left USB-C** (UART) port for flashing.

| File | Offset | Purpose |
|---|---|---|
| `Bruce-esp32-s3-N16R8.bin` | `0x0` | Merged (all-in-one) |
| `bootloader.bin` | `0x0` | Bootloader |
| `partitions.bin` | `0x8000` | Partition table |
| `boot_app0.bin` | `0xe000` | OTA selector |
| `firmware.bin` | `0x10000` | Application |

---

## 🔧 Hardware

### Main board
- **ESP32-S3 N16R8** — 16 MB QSPI Flash, 8 MB OPI PSRAM
- Dual USB-C: **Left** = UART/programming, **Right** = Native USB (Bad USB)

### Modules
| Module | Function |
|---|---|
| CC1101 | Sub-GHz 300–928 MHz |
| PN532 | NFC 13.56 MHz + RFID |
| NRF24L01 ×2 | 2.4 GHz, MouseJack, jammer |
| Si5351 | Signal generator 8 kHz–160 MHz |
| MicroSD | File storage |
| OLED 1.3" SH1106 (SSD1106G) | Display |
| VS1838B | IR receiver 38 kHz |
| IR LED + 2N2222 | IR transmitter |
| Passive buzzer | Sound |
| 6× buttons | Navigation |

---

## 📌 Pinout

### I2C (shared) — OLED `0x3C`, PN532 `0x24`, Si5351 `0x60`
```
IO8  = SDA
IO18 = SCL
```

### SPI (shared bus) — CC1101 + MicroSD + NRF24 ×2
```
IO11 = MOSI
IO12 = SCK
IO13 = MISO
```

### CS / control pins (unique, no conflict)
```
CC1101    GDO0=IO9   CS=IO10
MicroSD   CS=IO14
NRF24 #1  CE=IO4     CSN=IO5
NRF24 #2  CE=IO6     CSN=IO7
IR        RX=IO1     TX=IO2
Buzzer    IO47
```

### Buttons (to GND, pull-up)
```
UP=IO15  DOWN=IO16  LEFT=IO38  RIGHT=IO39  OK=IO40  BACK=IO41
```

### ⛔ DO NOT USE — bricks the board
```
GPIO 26-32  → QSPI Flash
GPIO 33-37  → OPI PSRAM (forbidden on N16R8)
GPIO 19-20  → Native USB D+/D- (reserved for Bad USB)
GPIO 45-46  → Strapping
GPIO 43-44  → UART0 TX/RX (debug)
```

---

## 🧰 Build from source

Requires [PlatformIO](https://platformio.org/).

```sh
# Build
pio run -e esp32-s3-devkitc-1-psram

# Build + upload to device
pio run -e esp32-s3-devkitc-1-psram -t upload

# Serial monitor
pio device monitor
```

Output: `.pio/build/esp32-s3-devkitc-1-psram/firmware.bin`

The active board is set by `default_envs` in `platformio.ini` (`esp32-s3-devkitc-1-psram`).

> **One binary, mixed hardware:** modules are probed at runtime. If a chip is missing (e.g. no Si5351), the feature shows a "not found" message and returns to the menu instead of crashing. You can flash the same binary regardless of which optional modules you populated.

---

## ✨ Features

| Category | Contents |
|---|---|
| **WiFi** | AP/scan, Beacon spam, Deauth, Evil Portal, Wardriving, Sniffer, ARP spoof/poison, Responder |
| **BLE** | Scan, Bad BLE (Ducky), iOS/Windows/Samsung/Android spam |
| **RF (CC1101)** | Scan/Copy, Custom SubGhz, Spectrum, Replay, Jammer |
| **RFID/NFC (PN532)** | Read/Clone/Write, NDEF, Chameleon, Amiibolink |
| **IR** | TV-B-Gone, Receiver, Custom IR (NEC, SIRC, RC5/6, Samsung32…) |
| **NRF24** | Jammer, 2.4G Spectrum, Mousejack |
| **Si5351** | Frequency generation 8 kHz–160 MHz (CLK0/1/2), sweep, AM band |
| **Bad USB** | Ducky scripts (right USB-C / Native USB) |
| **Other** | JS interpreter, WebUI, SD/LittleFS manager, QR, iButton |

---

## 🤝 Contributing

Contributions welcome — bug fixes, new modules, board ports, docs. See **[CONTRIBUTING.md](./CONTRIBUTING.md)** for the full guide.

Quick version:
1. **Fork** this repo and create a branch: `git checkout -b feature/my-change`
2. Build it compiles: `pio run -e esp32-s3-devkitc-1-psram`
3. Format C/C++ with `clang-format` (config in `.clang-format`)
4. **Commit** with a clear message, **push**, open a **Pull Request**
5. Found a bug or have an idea? [Open an issue](https://github.com/modabucksmain-pixel/Bruce-DIY-Flipper/issues/new/choose)

Adding hardware? Pin maps live in `boards/ESP-General/pins_arduino.h`; a new module goes under `src/modules/<name>/` with its menu in `src/core/menu_items/`.

---

## 🙏 Credits

This project is based on the [**Bruce**](https://github.com/pr3y/Bruce) firmware. Thanks to the original developers [@pr3y](https://github.com/pr3y), [@bmorcelli](https://github.com/bmorcelli), and all Bruce contributors.

- Official project: https://bruce.computer
- Wiki: https://wiki.bruce.computer
- Discord: https://discord.gg/WJ9XF9czVT

## 📄 License

AGPL — see [LICENSE](./LICENSE).
