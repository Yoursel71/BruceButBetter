#ifndef __WIFI_MENU_H__
#define __WIFI_MENU_H__

#include <MenuItemInterface.h>

class WifiMenu : public MenuItemInterface {
public:
    WifiMenu() : MenuItemInterface("WiFi") {}

    void optionsMenu(void);
    void drawIcon(float scale);
#if defined(UI_COMPACT) && UI_COMPACT
    CompactMenuIcon compactIcon() const override { return CompactMenuIcon::Wifi; }
#endif
    bool hasTheme() { return bruceConfig.theme.wifi; }
    String themePath() { return bruceConfig.theme.paths.wifi; }

private:
    void configMenu(void);
    void attacksMenu(void);
    void toolsMenu(void);
    void connectionMenu(void);
};

#endif
