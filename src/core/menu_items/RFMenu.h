#ifndef __RF_MENU_H__
#define __RF_MENU_H__

#include <MenuItemInterface.h>

class RFMenu : public MenuItemInterface {
public:
    RFMenu() : MenuItemInterface("SUB-GHz") {}

    void optionsMenu(void);
    void drawIcon(float scale);
#if defined(UI_COMPACT) && UI_COMPACT
    CompactMenuIcon compactIcon() const override { return CompactMenuIcon::Radio; }
#endif
    bool hasTheme() { return bruceConfig.theme.rf; }
    String themePath() { return bruceConfig.theme.paths.rf; }

private:
    void configMenu(void);
    void jammerBandsMenu(void);
};

#endif
