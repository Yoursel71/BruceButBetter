#pragma once
#include <Arduino.h>
#ifdef HAS_SI5351

#include <globals.h>

// ── Public API ────────────────────────────────────────────────────────────────
// Interactive frequency setter with dial UI (CLK0/1/2)
void si5351_set_frequency_ui();

// Continuous output on a chosen CLK pin at a chosen frequency
void si5351_continuous_output();

// Toggle CLK0/1/2 output on/off
void si5351_output_toggle();

// Sweep start→stop in configurable steps & dwell
void si5351_sweep();

// AM broadcast band scanner (530kHz–1700kHz)
void si5351_am_band();

// Quick preset picker
void si5351_presets();

#endif // HAS_SI5351
