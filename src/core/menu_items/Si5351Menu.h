#pragma once
#include <Arduino.h>
#ifdef HAS_SI5351
#include <MenuItemInterface.h>

class Si5351Menu : public MenuItemInterface {
public:
    Si5351Menu() : MenuItemInterface("RF Gen") {}
    void optionsMenu(void);
    void drawIcon(float scale);
#if defined(UI_COMPACT) && UI_COMPACT
    CompactMenuIcon compactIcon() const override { return CompactMenuIcon::Signal; }
#endif
    bool hasTheme() { return false; }
    String themePath() { return ""; }
};
#endif // HAS_SI5351
