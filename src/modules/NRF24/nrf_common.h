#ifndef __NRF_COMMON_H
#define __NRF_COMMON_H
#include <RF24.h>
#include <globals.h>

// Define the Macros case it hasn't been declared
#ifndef NRF24_CE_PIN
#define NRF24_CE_PIN -1
#endif
#ifndef NRF24_SS_PIN
#define NRF24_SS_PIN -1
#endif

enum NRF24_MODE {
    NRF_MODE_DISABLED, // 0b00
    NRF_MODE_SPI,      // 0b01
    NRF_MODE_UART,     // 0b10
    NRF_MODE_BOTH      // 0b11
};
#define CHECK_NRF_SPI(mode) (mode & NRF_MODE_SPI)
#define CHECK_NRF_UART(mode) (mode & NRF_MODE_UART)
#define CHECK_NRF_BOTH(mode) (mode == NRF_MODE_BOTH)

extern RF24 NRFradio;
extern HardwareSerial NRFSerial; // Uses UART2 for External NRF's

// NRF24 #2 — inject-only radio (CE=NRF24_2_CE_PIN, CSN=NRF24_2_SS_PIN)
// Pays the same SPI bus as radio1, different CS/CE pins.
// When ready, mj_transmitReliable() uses this radio for TX while radio1 stays in RX.
#ifdef NRF24_2_CE_PIN
extern RF24 NRFradio2;
extern bool NRFradio2_ready;
bool nrf_start2();
#endif

NRF24_MODE nrf_setMode();

bool nrf_start(NRF24_MODE mode);

void nrf_info();
#endif
