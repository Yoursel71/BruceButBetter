#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "soc/soc_caps.h"
#include <stdint.h>

// ── USB ─────────────────────────────────────────────────────────────────────
#define USB_VID 0x303A
#define USB_PID 0x1001
#define USB_as_HID 1

// ── UART0 (debug) ────────────────────────────────────────────────────────────
static const uint8_t TX = 43;
static const uint8_t RX = 44;
#define SERIAL_TX 43
#define SERIAL_RX 44

// ── I2C  (SDA=8, SCL=18)  -> OLED SSD1106G + PN532 + Si5351 ─────────────────
#define GROVE_SDA 8
#define GROVE_SCL 18

// Si5351 clock generator (I2C addr 0x60, same bus)
#define HAS_SI5351 1
#define SI5351_I2C_ADDR 0x60
static const uint8_t SDA = GROVE_SDA;
static const uint8_t SCL = GROVE_SCL;

// ── SPI bus (shared) ──────────────────────────────────────────────────────────
#define SPI_MOSI_PIN 11
#define SPI_SCK_PIN  12
#define SPI_MISO_PIN 13
#define SPI_SS_PIN   10   // CC1101 CS (default SS)
static const uint8_t MOSI = SPI_MOSI_PIN;
static const uint8_t MISO = SPI_MISO_PIN;
static const uint8_t SCK  = SPI_SCK_PIN;
static const uint8_t SS   = SPI_SS_PIN;

// ── CC1101 ───────────────────────────────────────────────────────────────────
#define USE_CC1101_VIA_SPI
#define CC1101_GDO0_PIN  9
#define CC1101_SS_PIN    10
#define CC1101_MOSI_PIN  SPI_MOSI_PIN
#define CC1101_SCK_PIN   SPI_SCK_PIN
#define CC1101_MISO_PIN  SPI_MISO_PIN

// ── MicroSD ──────────────────────────────────────────────────────────────────
#define SDCARD_CS    14
#define SDCARD_MOSI  SPI_MOSI_PIN
#define SDCARD_SCK   SPI_SCK_PIN
#define SDCARD_MISO  SPI_MISO_PIN

// ── NRF24L01 #1 ──────────────────────────────────────────────────────────────
#define USE_NRF24_VIA_SPI
#define NRF24_CE_PIN   4
#define NRF24_SS_PIN   5
#define NRF24_MOSI_PIN SPI_MOSI_PIN
#define NRF24_SCK_PIN  SPI_SCK_PIN
#define NRF24_MISO_PIN SPI_MISO_PIN

// ── NRF24L01 #2 (MouseJack inject - not yet used) ───────────────────────────
#define NRF24_2_CE_PIN   6
#define NRF24_2_SS_PIN   7

// ── IR ───────────────────────────────────────────────────────────────────────
#define RXLED   1   // VS1838B OUT
#define TXLED   2   // IR LED TX
#define LED_ON  HIGH
#define LED_OFF LOW

// ── Buzzer ───────────────────────────────────────────────────────────────────
#define BUZZ_PIN 47

// ── Buttons (active-LOW, wired to GND, internal pull-up) ────────────────────
#define HAS_5_BUTTONS
#define BTN_ALIAS "\"OK\""
#define SEL_BTN  40   // OK / SEL
#define UP_BTN   15   // UP
#define DW_BTN   16   // DOWN
#define L_BTN    38   // LEFT  (PrevPress)
#define R_BTN    39   // RIGHT (NextPress)
#define ESC_BTN  41   // BACK / ESC
#define BTN_ACT  LOW

// ── OLED SSD1106G / SH1106 (1.3", 128×64, I2C) ───────────────────────────────
#define HAS_SCREEN  1
#define ROTATION    0
#define MINBRIGHT   0

// Display backend selection is handled in ESP-General.ini (LovyanGFX)
// #define USE_ARDUINO_GFX     1
// #define TFT_DATABUS_N       5       
// #define TFT_DISPLAY_DRIVER_N 48     
// #define TFT_I2C_ADDR        0x3C
// #define TFT_RST             -1
// #define TFT_WIDTH           128
// #define TFT_HEIGHT          64
#define TFT_BL              -1
#define BACKLIGHT           -1
#define SMOOTH_FONT         0       // not needed on a monochrome OLED

// ── Font sizes (tuned for the small OLED) ──────────────────────────────────────
#define FP 1
#define FM 2
#define FG 3

// ── Device name ─────────────────────────────────────────────────────────────────
#define DEVICE_NAME "\"BruceButBetter\""

// ── Reserved pins (DO NOT USE!) ───────────────────────────────────────────────
// GPIO 19-20 : Native USB D+/D-
// GPIO 26-32 : QSPI Flash
// GPIO 33-37 : OPI PSRAM (N16R8)
// GPIO 43-44 : UART0 TX/RX
// GPIO 45-46 : Strapping

#endif /* Pins_Arduino_h */
