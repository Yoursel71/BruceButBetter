# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What This Is

Bruce is an ESP32 firmware for offensive security / red team operations. It targets M5Stack, Lilygo, and custom hardware and is built with PlatformIO + Arduino framework.

## Build Commands

```bash
# Build for a specific board
platformio run -e m5stack-cardputer

# Build and flash to connected device
platformio run -e m5stack-cardputer -t upload

# Monitor serial output
platformio device monitor

# Flash a pre-built binary
esptool.py --port /dev/ttyACM0 write_flash 0x00000 Bruce-<device>.bin
```

To build a different board, change the `-e` argument to any environment name listed in `platformio.ini` (e.g., `m5stack-cplus2`, `lilygo-t-embed-cc1101`, `esp32-c5`).

The active build environment is controlled by `default_envs` in `platformio.ini`. All other board envs are commented out — uncomment one to make it the default.

## Architecture

### Source Layout

```
src/
  main.cpp            — entry point, sets up the menu loop
  core/               — framework: display, config, keyboard input, menus, WiFi/net utils
    menu_items/       — one *Menu.h per top-level menu category
    serial_commands/  — CLI commands accessible over serial
    wifi/             — WiFi helpers, WebUI, WireGuard
  modules/            — feature implementations
    ble/              — BLE spam, scan, exploits
    bjs_interpreter/  — JavaScript engine (mquickjs) bindings
    ethernet/         — ARP scanner/spoofer/poisoner, DHCP starvation
    fm/               — FM radio broadcast
    gps/              — GPS tracking, wardriving
    ir/               — IR send/receive, TV-B-Gone
    nrf24/            — NRF24 jammer, spectrum
    rf/               — Sub-GHz RF (CC1101, 433 MHz)
    rfid/             — RFID read/clone/write (PN532, MFRC522, ChameleonUltra)
    wifi/             — WiFi attacks, evil portal, sniffer, responder
    ...
boards/               — per-board hardware abstraction
  _boards_json/       — PlatformIO board JSON configs
  pinouts/            — shared variant header that #includes the active board's pins
  [board]/
    pins_arduino.h    — pin definitions and capability flags (#define HAS_RGB_LED, etc.)
    interface.cpp     — board-specific hardware setup (display init, input handling)
    [board].ini       — PlatformIO environment config
lib/                  — vendored/local libraries (HAL, TFT_eSPI, etc.)
sd_files/             — SD card content templates (themes, evil portal pages, scripts)
```

### Key Global Objects

- `bruceConfig` (`src/core/config.h`) — singleton holding all persisted settings; extends `BruceTheme`. Written to `/bruce.conf` on the device filesystem.
- `MainMenu` (`src/core/main_menu.h`) — owns all `*Menu` instances and drives the main UI loop.
- Each feature area has a `*Menu` class (e.g., `WifiMenu`, `RFMenu`) that implements `MenuItemInterface`.

### Build Variants

There are three base `[env]` configurations in `platformio.ini`:

| Base config | When used | Notable differences |
|---|---|---|
| `env` | Full-featured builds | All features, FastLED, SSH, WireGuard |
| `env_light` | Launcher-compatible (LITE_VERSION) | No TelNet, SSH, WireGuard, BLE scan, interpreter |
| `env_4mb` | 4 MB flash devices | Restricted IR protocol set, no FastLED/audio |

Board `.ini` files extend one of these via `extends = env` / `extends = env_light`.

### Adding a New Board

Follow the template in `boards/_New-Device-Model/`. Four files are required:

1. `boards/_boards_json/[board].json` — PlatformIO board descriptor (`"variant": "pinouts"`)
2. `boards/pinouts/pins_arduino.h` — add a `#ifdef` block that `#include`s your board's header
3. `boards/[board]/pins_arduino.h` — pin assignments and capability `#define`s
4. `boards/[board]/interface.cpp` — display init, input handler, board-specific setup
5. `boards/[board]/[board].ini` — PlatformIO env, inherits `${env.build_flags}` and `${env.lib_deps}`

See `boards/README.md` for the full walkthrough.

### CI

PRs against `main`/`dev` compile a representative matrix (Cardputer, StickC Plus2, CoreS3, CYD Launcher, ESP32-C5) via `.github/workflows/PR_check.yml`. Artifacts are uploaded as `Bruce-<env>.bin`.

# DIY Flipper Zero — Bruce Firmware Proje Dosyası

## Proje Özeti
ESP32-S3 N16R8 tabanlı DIY Flipper Zero. Bruce firmware kullanılıyor.
PlatformIO projesi. Hedef board: ESP-General.

# DIY Flipper Zero — Bruce Firmware Proje Dosyası

## Proje Özeti
ESP32-S3 N16R8 tabanlı DIY Flipper Zero. Bruce firmware kullanılıyor.
PlatformIO projesi. Hedef board: ESP-General.

---

## Donanım

### Ana Kart
- **ESP32-S3 N16R8** (16MB QSPI Flash, 8MB OPI PSRAM)
- Çift USB-C: Sol = UART/programlama, Sağ = Native USB (Bad USB)

### Modüller
| Modül | İşlev |
|---|---|
| CC1101 | Sub-GHz 300-928MHz |
| PN532 | NFC 13.56MHz + RFID |
| NRF24L01 x2 | 2.4GHz, MouseJack |
| MicroSD | Dosya depolama |
| OLED 1.3" SSD1106G | Ekran (SH1106 driver) |
| VS1838B | IR alıcı 38kHz |
| IR LED + 2N2222 | IR verici |
| Pasif Buzzer | Ses |
| 6x Buton | Navigasyon |
| Si5351 | Sinyal üreteci 8kHz-160MHz |

---

## Pin Haritası

### I2C Bus
```
IO8  = SDA  → OLED SSD1106G + PN532 (paylaşımlı)
IO18 = SCL  → OLED SSD1106G + PN532 (paylaşımlı)
```

### SPI Bus (paylaşımlı hat)
```
IO11 = MOSI → CC1101 + MicroSD + NRF24#1 + NRF24#2
IO12 = SCK  → CC1101 + MicroSD + NRF24#1 + NRF24#2
IO13 = MISO → CC1101 + MicroSD + NRF24#1 + NRF24#2
```

### CC1101 (Sub-GHz)
```
IO9  = GDO0
IO10 = CS
```

### MicroSD
```
IO14 = CS
```

### NRF24L01 #1
```
IO4 = CE
IO5 = CSN
```

### NRF24L01 #2
```
IO6 = CE
IO7 = CSN
```

### IR
```
IO1 = VS1838B OUT (IR RX)
IO2 = IR LED TX (2N2222 transistör base)
```

### Diğer
```
IO47 = Pasif Buzzer
```

### Si5351 Clock Generator
```
IO8  = SDA (mevcut I2C hattı, paylaşımlı)
IO18 = SCL (mevcut I2C hattı, paylaşımlı)
I2C Adres = 0x60
Çıkış: CLK0, CLK1, CLK2 (8kHz - 160MHz)
```

### Butonlar (GND'ye bağlı, pull-up)
```
IO15 = UP
IO16 = DOWN
IO38 = LEFT
IO39 = RIGHT
IO40 = OK / SEL
IO41 = BACK / ESC
```

---

## Kısıtlamalar — KESİNLİKLE KULLANMA

```
GPIO 26-32 → QSPI Flash (kullanırsan kart kitlenir)
GPIO 33-37 → OPI PSRAM  (N16R8'de kesinlikle yasak)
GPIO 19-20 → Native USB D+/D- (Bad USB için ayrılmış)
GPIO 45-46 → Strapping pinleri
GPIO 43-44 → UART0 TX/RX (debug)
```

---

## Firmware Yapısı

```
src/
├── core/
│   ├── main_menu.cpp      ← Ana menü
│   ├── startup_app.cpp    ← Açılış
│   ├── display.cpp        ← Ekran
│   ├── theme.cpp          ← Tema/renkler
│   └── menu_items/        ← Her menünün .cpp/.h dosyası
└── modules/
    ├── ble/               ← BLE spam, scan
    ├── rf/                ← CC1101 Sub-GHz
    ├── rfid/              ← PN532 NFC/RFID
    ├── ir/                ← IR kumanda
    ├── wifi/              ← WiFi saldırıları
    ├── NRF24/             ← MouseJack, jammer
    ├── badusb_ble/        ← Bad USB
    └── others/            ← Diğerleri

boards/ESP-General/
├── ESP-General.ini        ← Pin tanımları buraya
└── interface.cpp          ← Buton/ekran arayüzü
```

---

## Yapılacaklar (Öncelik Sırasıyla)

### 1. Pin Config (boards/ESP-General/ESP-General.ini)
Yukarıdaki pin haritasını bu dosyaya gir.
OLED için SH1106 driver seç (SSD1106G = SH1106).

### 2. Gereksiz Modülleri Gizle
Şunlar bu projede YOK, menüden kaldır:
- GPS
- LoRa
- Ethernet
- FM Radyo (isteğe bağlı)

### 3. Menü Özelleştirme
- Türkçe menü isimleri
- Kendi sıralama: WiFi, BLE, RF, NFC, IR, Bad USB, NRF24

### 4. BLE Spam Özelleştirme
Özel cihaz isimleri ekle.

### 5. Açılış Animasyonu
startup_app.cpp içinde özel logo/yazı.

### 7. Si5351 Modülü (Sıfırdan Yazılacak)
- platformio.ini'ye ekle: etherkit/Si5351Arduino
- src/modules/si5351/ klasörü oluştur
- Özellikler: frekans ayar, CLK seçimi, sweep, AM radyo modu
- src/core/menu_items/Si5351Menu.cpp oluştur
- Ana menüye "Sinyal" veya "RF Gen" olarak ekle
```bash
pio run -e esp32-s3-devkitc-1
```
.bin çıktısı → .pio/build/ klasöründe

---

## Notlar
- Bruce firmware SH1106 destekliyor (U8g2 + LovyanGFX)
- SPI bus paylaşımlı, CS pinleri unique olduğu için conflict yok
- I2C bus paylaşımlı, OLED=0x3C, PN532=0x24 farklı adresler
- Bad USB için sağ USB-C (Native USB) kullanılacak
- NRF24 Jammer ve MouseJack firmware'de hazır geliyor
- Si5351 Bruce'ta varsayılan olarak yok, modül sıfırdan yazılacak
- Si5351 kütüphane: etherkit/Si5351Arduino (platformio.ini'ye ekle)
- I2C conflict yok: OLED=0x3C, PN532=0x24, Si5351=0x60
- Wire.begin(8,18) zaten mevcut, Si5351 için tekrar başlatma
