#pragma once
#include <Arduino.h>
#ifdef HAS_SI5351
#include <MenuItemInterface.h>

class Si5351Menu : public MenuItemInterface {
public:
    Si5351Menu() : MenuItemInterface("RF Gen") {}
    void optionsMenu(void);
    void drawIcon(float scale);
    bool hasTheme() { return false; }
    String themePath() { return ""; }
};
#endif // HAS_SI5351
