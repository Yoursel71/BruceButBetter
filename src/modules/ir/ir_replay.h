#pragma once
#include "custom_ir.h"
#include <globals.h>

// Global last-captured IR signal — populated by IrRead::read_signal()
// when a signal is decoded successfully.
struct IrLastCapture {
    bool   valid    = false;
    IRCode code;
};
extern IrLastCapture g_irLastCapture;

// Store a decoded signal into the global buffer (called from ir_read.cpp)
void irReplay_store(const IRCode &code);

// Replay the last captured signal (shows error if nothing captured yet)
void ir_replay_last();

// Cycle through NEC codes 0x0000..0xFFFF for a given address — brute force
void ir_brute_force();
