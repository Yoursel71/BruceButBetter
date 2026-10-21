#ifndef __RFID_MENU_H__
#define __RFID_MENU_H__

#include <MenuItemInterface.h>

class RFIDMenu : public MenuItemInterface {
public:
    RFIDMenu() : MenuItemInterface("RFID") {}

    void optionsMenu(void);
    void drawIcon(float scale);
#if defined(UI_COMPACT) && UI_COMPACT
    CompactMenuIcon compactIcon() const override { return CompactMenuIcon::Nfc; }
#endif
    bool hasTheme() { return bruceConfig.theme.rfid; }
    String themePath() { return bruceConfig.theme.paths.rfid; }

private:
    void configMenu(void);
};

#endif
