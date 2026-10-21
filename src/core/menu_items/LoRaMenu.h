#ifndef __LORA_MENU_H__
#define __LORA_MENU_H__
#if !defined(LITE_VERSION)
#include <MenuItemInterface.h>

void lorachat();
void changeusername();
void chfreq();

class LoRaMenu : public MenuItemInterface {
public:
    LoRaMenu() : MenuItemInterface("LoRa") {}

    void optionsMenu(void);
    void drawIcon(float scale);
#if defined(UI_COMPACT) && UI_COMPACT
    CompactMenuIcon compactIcon() const override { return CompactMenuIcon::LoRa; }
#endif
    bool hasTheme() { return bruceConfig.theme.lora; }
    String themePath() { return bruceConfig.theme.paths.lora; }

private:
    void configMenu(void);
};

#endif
#endif
