#include "main_menu.h"
#include "display.h"
#include "utils.h"
#include <algorithm>
#include <globals.h>

MainMenu::MainMenu() {
    _menuItems = {
        &wifiMenu,
        &bleMenu,
        &rfMenu,
        &rfidMenu,
        &irMenu,
        &othersMenu, // Bad USB & HID tools live here
        &nrf24Menu,
#if !defined(LITE_VERSION) && !defined(DISABLE_LORA)
        &loraMenu,
#endif
#if defined(FM_SI4713) && !defined(LITE_VERSION) && !defined(DISABLE_FM)
        &fmMenu,
#endif
#ifdef HAS_SI5351
        &si5351Menu,
#endif
#if !defined(LITE_VERSION) && !defined(DISABLE_ETHERNET)
        &ethernetMenu,
#endif
#ifndef DISABLE_GPS
        &gpsMenu,
#endif
        &fileMenu,
#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
        &scriptsMenu,
#endif
        &selfTestMenu,
        &clockMenu,
        &configMenu,
    };

    _totalItems = _menuItems.size();
}

MainMenu::~MainMenu() {}

void MainMenu::begin(void) {
    returnToMenu = false;
    options = {};

    std::vector<String> l = bruceConfig.disabledMenus;
    for (int i = 0; i < _totalItems; i++) {
        String itemName = _menuItems[i]->getName();
        if (find(l.begin(), l.end(), itemName) == l.end()) { // If menu item is not disabled
            options.push_back(
                {// selected lambda
                 _menuItems[i]->getName(),
                 [this, i]() { _menuItems[i]->optionsMenu(); },
                 false,                                  // selected = false
                 [](void *menuItem, bool shouldRender) { // render lambda
                     if (!shouldRender) return false;
                     drawMainBorder(false);

                     MenuItemInterface *obj = static_cast<MenuItemInterface *>(menuItem);
#if defined(UI_COMPACT) && UI_COMPACT
                     obj->draw();
#else
                     float scale = float((float)tftWidth / (float)240);
                     if (bruceConfigPins.rotation & 0b01) scale = float((float)tftHeight / (float)135);
                     obj->draw(scale);
#endif
#if defined(HAS_TOUCH)
                     TouchFooter();
#endif
                     return true;
                 },
                 _menuItems[i]
                }
            );
        }
    }
#if defined(UI_COMPACT) && UI_COMPACT
    int compactIndex = 0;
    for (auto *item : _menuItems) {
        if (find(l.begin(), l.end(), item->getName()) == l.end())
            item->setCompactPosition(compactIndex++, static_cast<int>(options.size()));
    }
#endif
    _currentIndex = loopOptions(options, MENU_TYPE_MAIN, "Main Menu", _currentIndex);
};

/*********************************************************************
**  Function: hideAppsMenu
**  Menu to Hide or show menus
**********************************************************************/

void MainMenu::hideAppsMenu() {
    auto items = this->getItems();
    int chosen = -1;
RESTART: // using gotos to avoid stackoverflow after many choices
    options.clear();
    for (auto item : items) {
        // Config holds this menu; hiding it would leave no way to show the menus again.
        if (item == &configMenu) continue;
        String label = item->getName();
        std::vector<String> l = bruceConfig.disabledMenus;
        bool enabled = find(l.begin(), l.end(), label) == l.end();
        // Selecting an item toggles it: a visible menu is hidden, a hidden one is shown again.
        options.push_back(
            {label,
             [label, enabled]() {
                 if (enabled) {
                     bruceConfig.addDisabledMenu(label);
                 } else {
                     auto &d = bruceConfig.disabledMenus;
                     d.erase(std::remove(d.begin(), d.end(), label), d.end());
                 }
             },
             enabled}
        );
    }
    options.push_back({"Show All", [=]() { bruceConfig.disabledMenus.clear(); }, true});
    addOptionToMainMenu();
    chosen = loopOptions(options);
    bruceConfig.saveFile();
    // BACK returns -1: leave the menu instead of drawing it again.
    if (chosen >= 0 && !returnToMenu) goto RESTART;
}
