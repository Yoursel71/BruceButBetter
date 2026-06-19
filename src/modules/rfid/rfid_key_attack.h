#pragma once
#ifndef LITE_VERSION

// Dictionary attack against MIFARE Classic tags.
// Tries 30+ known default keys on every sector (Key A and Key B).
// Found keys are displayed and optionally saved to /rfid_keys.txt
void rfid_key_attack();

#endif
