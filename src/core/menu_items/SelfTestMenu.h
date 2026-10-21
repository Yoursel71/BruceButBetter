#pragma once
#include <Arduino.h>
#include <MenuItemInterface.h>

// Hardware Self-Test — probes the I2C bus and reports which modules answer,
// labelling the ones this build expects (OLED / PN532 / Si5351 ...). Aimed at
// DIY builders verifying their wiring. Not present in upstream Bruce.
class SelfTestMenu : public MenuItemInterface {
public:
    SelfTestMenu() : MenuItemInterface("Self-Test") {}

    void optionsMenu(void);
    void drawIcon(float scale);
#if defined(UI_COMPACT) && UI_COMPACT
    CompactMenuIcon compactIcon() const override { return CompactMenuIcon::Diagnostics; }
#endif
    bool hasTheme() { return false; }
    String themePath() { return ""; }
};
