# Changelog

All notable changes to BruceButBetter. Format loosely follows
[Keep a Changelog](https://keepachangelog.com/); this project is a downstream fork of
[pr3y/Bruce](https://github.com/pr3y/Bruce).

## [Unreleased]

## [1.4.1] — 2026-10-31

### Fixed
- **No silent switch to the RF433 module.** In *Config → RF Module*, choosing CC1101 when no CC1101
  answers now shows the error and keeps the current module; it used to switch to "M5 RF433T/R"
  without saying so. Leaving the menu with BACK no longer selects RF433 either.
- **RF433 pins kept off the I²C bus on ESP-General.** The single-pin RF433 module defaulted to the
  Grove pins, which on ESP-General are the OLED/PN532/Si5351 I²C lines (GPIO 8/18). Its defaults are
  now GPIO 2/1, the only pins the board's RF TX/RX menu offers (shared with IR), and saved configs
  that still hold 8/18 are corrected and rewritten at boot. Other boards keep the Grove pins.
- **`cat` no longer freezes the serial CLI.** Files over 1 KB, including both config files, made
  it show "File is too big" on the screen and wait for a button press, so the CLI stopped answering.
  `cat` now streams the file.
- **PR build check runs again.** Its trigger named a single branch "main, dev", so pull requests to
  `master` were never compiled.
- **Wardriving is no longer listed for ESP-General** in the README and on the site: it needs GPS,
  which this build does not include.

### Added
- `tools/publish_web_flasher.sh <tag>` publishes a release's ESP32 / ESP32-S3 images to the browser
  flasher (the BruceButBetter-bins repo), which had stopped at v1.2.0.
- README and CONTRIBUTING link the [feature poll](https://github.com/Yoursel71/BruceButBetter/discussions/12)
  and the good first issue / help wanted lists.

## [1.4.0] — 2026-10-30

### Changed
- **Main menu is a 4×2 icon grid.** On the 128×64 OLED (ESP-General compact UI) the single-icon
  carousel is replaced by eight 16 px icons per page, a framed focus cell, page dots, an `n/total`
  counter and the focused app's full name in the footer (scrolls when it does not fit). Hidden apps
  drop out of the grid and the counter.
- **Grid navigation.** LEFT/RIGHT move one cell in reading order and continue across rows and pages;
  UP/DOWN change row and flip to the previous/next page at the edge, keeping the column.
- **Fixed-slot status bar.** Indicators (S W @ B [G] V) keep their positions when one turns on or off,
  battery % or `USB` stays at the right edge, and indicators also show while a title is displayed.
  A category's name stays as the title on every screen below it.
- **Icons redrawn for 16 px.** Icons scale from a 24-unit design grid; Files (folder), NRF24 (double
  antenna), Infrared (two beams) and Sub-GHz (tighter rings) were redrawn to stay legible.

### Added
- **List footers** with a divider and key hints (`OK:Sec BACK:Geri`, `OK:Degistir`, and
  `OK:Ac BACK:Ust` in the file browser).
- **BACK in dialogs** closes single-button dialogs and selects `Cancel` when one is present.
- `displayLoading()` for indeterminate waits and `setCompactScreenTitle()` for modules.

### Improved
- Lists, the icon grid, dialogs and progress screens are drawn in one batch
  (`startWrite`/`endWrite`) on LovyanGFX panels to reduce flicker.
- The Web UI Navigator log grows from 64 to 128 entries on the compact UI so a full grid page is
  captured.

## [1.3.0] — 2026-10-22

### Fixed
- **ESP-General SH1106 display configuration** — correct 128×64 geometry, panel offsets,
  rotation and monochrome colors for the ESP32-S3 N16R8 build.
- **Shared-SPI NRF24 initialization** — deselect both radios and hold CE low at startup,
  avoiding intermittent detection failures caused by the second chip-select line.
- **Compact UI and Navigator stability** — wrap long status messages, preserve neighboring
  text rows, replay the drawing log in chronological order, reuse logging buffers and guard
  image-slot bounds and allocation failures.
- **Menu visibility and deep sleep** — hidden apps can be shown again, BACK exits the visibility
  menu, Config cannot be hidden, and UP (GPIO15) wakes ESP-General from deep sleep.
- **Image and Web UI error handling** — guard empty-button dialogs, failed image loads,
  oversized BMP row buffers, encrypted upload lengths and bounded file-open retries.
- **Host Info port scan is now cancellable.** ESC is polled during each port's wait, not only
  between ports, and the WiFi connect is bounded by the scan timeout, so a filtered port can no
  longer block the loop and ESC quits the scan promptly.

### Added
- **Native 128×64 OLED interface** — 18 compact icons, a four-row scrolling list, readable
  dialogs, progress bars and status indicators; Web UI runs alongside the physical menu.
- **Short OK selects; long OK goes back** — 25 ms debounce and an 800 ms hold threshold on
  ESP-General, without sending a selection before the back action.
- **Capture the 2.4 GHz band to CSV.** New NRF24 menu entry *Spectrum → CSV* samples the band and
  writes `channel,freq_mhz,activity` for all 80 channels to `/BruceRF/nrf_spectrum_<ms>.csv`, reporting
  the busiest channel. The 2.4 GHz counterpart of the sub-GHz capture.
- **Capture a sub-GHz sweep to CSV.** New RF menu entry *Spectrum → CSV* sweeps the selected sub-GHz
  range and writes `freq_mhz,rssi_dbm` for every step to `/BruceRF/spectrum_<ms>.csv` on SD/LittleFS,
  so a band profile can be analysed off-device. Reports the peak and the saved path; ESC aborts with a
  partial save.
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

[Unreleased]: https://github.com/Yoursel71/BruceButBetter/compare/v1.4.1...master
[1.4.1]: https://github.com/Yoursel71/BruceButBetter/compare/v1.4.0...v1.4.1
[1.4.0]: https://github.com/Yoursel71/BruceButBetter/compare/v1.3.0...v1.4.0
[1.3.0]: https://github.com/Yoursel71/BruceButBetter/compare/v1.2.0...v1.3.0
[1.2.0]: https://github.com/Yoursel71/BruceButBetter/compare/v1.1.0...v1.2.0
[1.1.0]: https://github.com/Yoursel71/BruceButBetter/compare/v1.0.0...v1.1.0
[1.0.0]: https://github.com/Yoursel71/BruceButBetter/releases/tag/v1.0.0
