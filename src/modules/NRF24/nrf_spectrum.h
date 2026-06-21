#ifndef __NRF_SPECTRUM_H
#define __NRF_SPECTRUM_H
#include "modules/NRF24/nrf_common.h"
#include <RF24.h>

void nrf_spectrum();

// dual=true uses a second NRF24 (NRF24_2_*) to scan odd channels in parallel,
// roughly halving the sweep time. Falls back to single radio when unavailable.
String scanChannels(bool web = false, bool dual = false);

#endif
