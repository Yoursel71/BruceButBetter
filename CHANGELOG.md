# Changelog

All notable changes to BruceButBetter. Format loosely follows
[Keep a Changelog](https://keepachangelog.com/); this project is a downstream fork of
[pr3y/Bruce](https://github.com/pr3y/Bruce).

## [Unreleased]

### Added
- **CC1101 RSSI spectrum: peak-hold + dBm.** The sub-GHz range-scan analyzer (RF → RSSI Spectrum)
  now keeps a peak-hold cap above each bar so brief signals stay on screen, and the Max readout shows
  the strongest signal's dBm next to its frequency. Mono-OLED friendly.

## [1.2.0] — 2026-06-21

### Added
- **Self-Test now probes the SPI bus too** — alongside the I²C scan it reports microSD and both
  NRF24 radios (`[OK]/[--]`), probing all modules first and drawing once so a shared-SPI read can't
  glitch the screen mid-render. CC1101 is left to the RF menu (its own probe pops a dialog). The
  second NRF24 line is gated on `NRF24_2_CE_PIN`.
- **Si5351 frequency favorites** — "Favorites" in RF Gen saves/recalls CLK0 frequencies, persisted
  to `/si5351_fav.txt` on LittleFS (survives reboot; no `bruceConfig` schema change).
- **Tag-triggered release automation** — pushing a `v*` tag now builds the whole board matrix in CI
  and auto-attaches each merged `Bruce-<env>.bin` to the GitHub Release (`.github/workflows/release.yml`).
  The legacy build pipeline was restricted to manual dispatch to avoid double-builds.

## [1.1.0] — 2026-06-21

### Added
- **Hardware Self-Test** — new top-level menu that scans the I²C bus and reports every device that
  answers, labelling the modules this build expects (OLED, PN532, Si5351, RTC, sensors). For DIY
  builders verifying their wiring.
- **Si5351 live calibration** — "Calibrate" entry in RF Gen: 10 MHz reference on CLK0 with live ppb
  correction trimming (variable step), applied for the session.
- **Dual-NRF24 spectrum** — the 2.4 GHz analyzer uses a second NRF24 (when populated) to scan odd
  channels in parallel with radio 1's even channels (~2× faster sweep); single-radio fallback.
- **Multi-board web flasher** — the browser flasher now flashes any of 45 boards (chip family
  auto-matched), bins served same-origin from GitHub Pages.

### Notes
- The dual-NRF24 parallel-RF path is compile-verified; validate on real dual-radio hardware.

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
