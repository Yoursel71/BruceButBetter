#pragma once
#include "nrf_common.h"

// Enhanced ShockBurst passive sniffer.
// Sets NRF24 to promiscuous mode and captures ESB packets from
// wireless keyboards, mice, and USB dongles (Logitech, Microsoft, etc.)
void nrf_esb_sniffer();
