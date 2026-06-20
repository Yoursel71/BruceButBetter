# Changelog

All notable changes to BruceButBetter. Format loosely follows
[Keep a Changelog](https://keepachangelog.com/); this project is a downstream fork of
[pr3y/Bruce](https://github.com/pr3y/Bruce).

## [1.0.0] — 2026-06-20

First public release of the DIY ESP32-S3 N16R8 build.

### Added
- **Si5351 signal-generator module** (8 kHz–160 MHz) wired into the menus — not in upstream Bruce.
- **Custom `ESP-General` hardware target** — shared SPI + I²C backbone, dual USB-C, runtime-probed
  modules (CC1101, PN532, 2× NRF24, IR, OLED SH1106, microSD, buzzer, 6 buttons).
- **One-click browser flasher** (esp-web-tools) for the ESP32-S3 build:
  https://yoursel71.github.io/BruceButBetter/
- **Prebuilt merged `Bruce-<board>.bin` for 45 boards** attached to the release — flash at `0x0`,
  no toolchain required.
- **DIY build guide** ([`docs/BUILD.md`](docs/BUILD.md)) — bill of materials with purchase links,
  wiring diagram, assembly steps, and troubleshooting.
- Full upstream Bruce board matrix ported in (auto-registered via the `extra_configs` glob).

### Fixed
- **Classic-ESP32 link failures.** Restored the missing `libnet80211.a` and re-applied Bruce's raw
  802.11 injection patch locally (`objcopy --weaken-symbol=ieee80211_raw_frame_sanity_check`) so
  deauth / beacon TX works on `esp32dev` boards (Marauder, most CYD).
- Added the missing `custom_4Mb.csv` / `custom_4Mb_full.csv` partition tables that broke all 4 MB
  board builds.

### Known limitations
- `esp32-c5` variants need the RISC-V toolchain; `arduino-nesso-n1` rejects the MSYS build env —
  both build from source rather than shipping a prebuilt bin.

[1.0.0]: https://github.com/Yoursel71/BruceButBetter/releases/tag/v1.0.0
