#ifndef __IR_MENU_H__
#define __IR_MENU_H__

#include <MenuItemInterface.h>

class IRMenu : public MenuItemInterface {
public:
    IRMenu() : MenuItemInterface("IR") {}

    void optionsMenu(void);
    void drawIcon(float scale);
#if defined(UI_COMPACT) && UI_COMPACT
    CompactMenuIcon compactIcon() const override { return CompactMenuIcon::Infrared; }
#endif
    bool hasTheme() { return bruceConfig.theme.ir; }
    String themePath() { return bruceConfig.theme.paths.ir; }

private:
    void configMenu(void);
    void powerOffMenu(void);
};

#endif
