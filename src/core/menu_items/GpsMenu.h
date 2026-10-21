#ifndef __GPS_MENU_H__
#define __GPS_MENU_H__

#include <MenuItemInterface.h>

class GpsMenu : public MenuItemInterface {
public:
    GpsMenu() : MenuItemInterface("GPS") {}

    void optionsMenu(void);
    void wardrivingMenu(void);
    void drawIcon(float scale);
#if defined(UI_COMPACT) && UI_COMPACT
    CompactMenuIcon compactIcon() const override { return CompactMenuIcon::Gps; }
#endif
    bool hasTheme() { return bruceConfig.theme.gps; }
    String themePath() { return bruceConfig.theme.paths.gps; }

private:
    void configMenu(void);
};

#endif
