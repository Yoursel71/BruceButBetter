# Contributing to Bruce But Better

Thanks for your interest! Bug fixes, new modules, board ports, and documentation are all welcome.

This project is a hardware-specific fork of [Bruce](https://github.com/pr3y/Bruce) (ESP32-S3 N16R8 DIY Flipper Zero).

## Ways to contribute

- **Report a bug** — [open an issue](https://github.com/modabucksmain-pixel/BruceButBetter/issues/new/choose) with steps to reproduce, board, and what you expected.
- **Suggest a feature** — open an issue describing the use case.
- **Send a fix or feature** — open a Pull Request (see below).
- **Improve docs** — README, pinout, this guide.

## Development setup

You need [PlatformIO](https://platformio.org/) (CLI or the VS Code extension).

```sh
git clone https://github.com/modabucksmain-pixel/BruceButBetter.git
cd BruceButBetter

# Build
pio run -e esp32-s3-devkitc-1-psram

# Build + flash to a connected device
pio run -e esp32-s3-devkitc-1-psram -t upload

# Serial monitor
pio device monitor
```

The active board is set by `default_envs` in `platformio.ini`.

## Pull Request workflow

1. **Fork** the repo and create a topic branch:
   `git checkout -b feature/short-description`
2. Make your change. Keep it focused — one logical change per PR.
3. **Make sure it compiles:** `pio run -e esp32-s3-devkitc-1-psram`
4. **Format** your C/C++ with `clang-format` (config in `.clang-format`):
   `clang-format -i path/to/file.cpp`
5. **Commit** with a clear message (imperative mood, e.g. `Add Si5351 sweep wrap-around`).
6. **Push** and open a Pull Request against `master`. Describe what changed and why; link any related issue.

A maintainer will review. CI builds the firmware on PRs — keep the build green.

## Code layout

```
src/
  core/                 framework: display, config, keyboard, menus
    menu_items/         one *Menu.cpp/.h per top-level menu category
  modules/              feature implementations (ble, rf, rfid, ir, wifi, nrf24, si5351, ...)
boards/ESP-General/     this board's pin map + interface
  pins_arduino.h        pin assignments and HAS_* capability flags
  interface.cpp         display init, input handling
```

### Adding a new module

1. Create `src/modules/<name>/` with your `.cpp/.h`.
2. Guard hardware-specific code behind a capability flag, e.g. `#ifdef HAS_<NAME>`, and define that flag in `boards/ESP-General/pins_arduino.h`.
3. **Degrade gracefully when hardware is absent.** Probe the chip at init; if it's missing, show a message and return — never block or crash. Example pattern (`src/modules/si5351/si5351_module.cpp`):
   ```cpp
   if (!si.init(...)) { displayError("Si5351 not found", true); return; }
   ```
4. Add a menu under `src/core/menu_items/` and wire it into the main menu.

### Adding a new board

See `boards/README.md` for the full walkthrough (board JSON, pinout header, interface, env).

## Coding style

- C++ with the Arduino framework. Match the surrounding code.
- Run `clang-format` before committing (`.clang-format` is the source of truth).
- Prefer small, readable functions. Avoid blocking loops without an exit (`EscPress`) check.

## Reporting security issues

For vulnerabilities, please open an issue describing the impact. Do not include working exploits against third-party systems.

## License

By contributing you agree your work is licensed under the project's **AGPL** license (see [LICENSE](./LICENSE)).
