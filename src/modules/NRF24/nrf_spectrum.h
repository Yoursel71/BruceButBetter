#ifndef __NRF_SPECTRUM_H
#define __NRF_SPECTRUM_H
#include "modules/NRF24/nrf_common.h"
#include <RF24.h>

void nrf_spectrum();

// Sample the 2.4 GHz band for a while, then dump channel,freq_mhz,activity to a
// CSV on SD/LittleFS so the band can be analysed off-device.
void nrf_spectrum_csv();

// dual=true uses a second NRF24 (NRF24_2_*) to scan odd channels in parallel,
// roughly halving the sweep time. Falls back to single radio when unavailable.
String scanChannels(bool web = false, bool dual = false);

#endif
