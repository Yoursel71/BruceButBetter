#include "WifiMenu.h"
#include "core/display.h"
#include "core/settings.h"
#include "core/utils.h"
#include "core/wifi/webInterface.h"
#include "core/wifi/wg.h"
#include "core/wifi/wifi_common.h"
#include "core/wifi/wifi_mac.h"
#include "modules/ethernet/ARPScanner.h"
#include "modules/wifi/ap_info.h"
#include "modules/wifi/clients.h"
#include "modules/wifi/evil_portal.h"
#include "modules/wifi/karma_attack.h"
#include "modules/wifi/netcut.h"
#include "modules/wifi/responder.h"
#include "modules/wifi/scan_hosts.h"
#include "modules/wifi/sniffer.h"
#include "modules/wifi/wifi_atks.h"
#include "modules/NRF24/nrf_jammer.h"

#ifndef LITE_VERSION
#include "modules/pwnagotchi/pwnagotchi.h"
#include "modules/wifi/wifi_recover.h"
#endif

// #include "modules/reverseShell/reverseShell.h"
//  Developed by Fourier (github.com/9dl)
//  Use BruceC2 to interact with the reverse shell server
//  BruceC2: https://github.com/9dl/Bruce-C2
//  To use BruceC2:
//  1. Start Reverse Shell Mode in Bruce
//  2. Start BruceC2 and wait.
//  3. Visit 192.168.4.1 in your browser to access the web interface for shell executing.

// 32bit: https://github.com/9dl/Bruce-C2/releases/download/v1.0/BruceC2_windows_386.exe
// 64bit: https://github.com/9dl/Bruce-C2/releases/download/v1.0/BruceC2_windows_amd64.exe
#include "modules/wifi/socks4_proxy.h"
#include "modules/wifi/tcp_utils.h"

// global toggle - controls whether scanNetworks includes hidden SSIDs
bool showHiddenNetworks = false;

// "Saldırılar" - en güçlü / saldırgan özellikler
void WifiMenu::attacksMenu() {
    std::vector<Option> atkOptions;

    atkOptions.push_back({"Wifi Atks", wifi_atk_menu});
    atkOptions.push_back({"Evil Portal", [=]() {
                              // WebUI cleanup now handled automatically inside EvilPortal constructor
                              EvilPortal();
                          }});
    atkOptions.push_back({"NetCut", [=]() { netcutMenu(); }});
    atkOptions.push_back({"Jammer (Hizli)", nrf_jam_wifi_fast});
    atkOptions.push_back({"Jammer (Agir)", nrf_jam_wifi_heavy});
    // atkOptions.push_back({"ReverseShell", [=]()       { ReverseShell(); }});
#ifndef LITE_VERSION
    atkOptions.push_back({"Responder", responder});
#endif

    atkOptions.push_back({"Back", [this]() { optionsMenu(); }});
    loopOptions(atkOptions, MENU_TYPE_SUBMENU, "Wifi Attacks");
}

// "Araçlar" - zararsız ağ araçları
void WifiMenu::toolsMenu() {
    std::vector<Option> toolOptions;

#ifndef LITE_VERSION
    toolOptions.push_back({"Sniffer", sniffer_setup});
    toolOptions.push_back({"Brucegotchi", brucegotchi_start});
    toolOptions.push_back({"WiFi Pass Recovery", wifi_recover_menu});
    toolOptions.push_back({"Wireguard", wg_setup});
    toolOptions.push_back({"TelNET", telnet_setup});
    toolOptions.push_back({"SSH", lambdaHelper(ssh_setup, String(""))});
    toolOptions.push_back({"SOCKS4 Proxy", []() { socks4Proxy(1080); }});
    toolOptions.push_back({"Listen TCP", listenTcpPort});
    toolOptions.push_back({"Client TCP", clientTCP});
    toolOptions.push_back({"Scan Hosts", [=]() {
                               bool doScan = true;
                               if (!wifiConnected) doScan = wifiConnectMenu();

                               if (doScan) {
                                   esp_netif_t *esp_netinterface =
                                       esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
                                   if (esp_netinterface == nullptr) {
                                       Serial.println("Failed to get netif handle");
                                       return;
                                   }
                                   ARPScanner{esp_netinterface};
                               }
                           }});
#endif

    toolOptions.push_back({"Back", [this]() { optionsMenu(); }});
    loopOptions(toolOptions, MENU_TYPE_SUBMENU, "Wifi Tools");
}

// "Bağlantı" - temel/zararsız bağlantı işlemleri
void WifiMenu::connectionMenu() {
    std::vector<Option> connOptions;

    if (WiFi.getMode() == WIFI_MODE_STA || WiFi.getMode() == WIFI_MODE_APSTA) {
        connOptions.push_back({"AP info", displayAPInfo});
    }
    if (WiFi.status() != WL_CONNECTED) {
        connOptions.push_back({"Connect to Wifi", lambdaHelper(wifiConnectMenu, WIFI_STA)});
        connOptions.push_back({"Start WiFi AP", [=]() {
                                    wifiConnectMenu(WIFI_AP);
                                    displayInfo("pwd: " + bruceConfig.wifiAp.pwd, true);
                                }});
    }
    if (WiFi.getMode() != WIFI_MODE_NULL) { connOptions.push_back({"Turn Off WiFi", wifiDisconnect}); }

    connOptions.push_back({"Back", [this]() { optionsMenu(); }});
    loopOptions(connOptions, MENU_TYPE_SUBMENU, "Wifi Connection");
}

void WifiMenu::optionsMenu() {
    returnToMenu = false;
    options.clear();
    // Note: WiFi features will cleanly stop WebUI automatically when they start
    // User can navigate menu normally even with WebUI active
    // Sıralama: en güçlü/saldırgan özellikler en üstte, zararsız/temel özellikler en altta
    options.push_back({"Attacks", [this]() { attacksMenu(); }});
    options.push_back({"Tools", [this]() { toolsMenu(); }});
    options.push_back({"Connection", [this]() { connectionMenu(); }});
    options.push_back({"Config", [this]() { configMenu(); }});

    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "WiFi");

    options.clear();
}

void WifiMenu::configMenu() {
    std::vector<Option> wifiOptions;

    wifiOptions.push_back({"Change MAC", wifiMACMenu});
    wifiOptions.push_back({"Add Evil Wifi", addEvilWifiMenu});
    wifiOptions.push_back({"Remove Evil Wifi", removeEvilWifiMenu});
    wifiOptions.push_back({bruceConfig.TerminalLog ? "SSH/Telnet Log OFF" : "SSH/Telnet Log ON", [this]() {
                               bruceConfig.setTerminalLog(!bruceConfig.TerminalLog);
                               configMenu();
                           }});

    // Evil Wifi Settings submenu (unchanged)
    wifiOptions.push_back({"Evil Wifi Settings", [this]() {
                               std::vector<Option> evilOptions;

                               evilOptions.push_back({"Set Gateway IP", setEvilGatewayIp});
                               evilOptions.push_back({"Password Mode", setEvilPasswordMode});
                               evilOptions.push_back({"Rename /creds", setEvilEndpointCreds});
                               evilOptions.push_back({"Allow /creds access", setEvilAllowGetCreds});
                               evilOptions.push_back({"Rename /ssid", setEvilEndpointSsid});
                               evilOptions.push_back({"Allow /ssid access", setEvilAllowSetSsid});
                               evilOptions.push_back({"Display endpoints", setEvilAllowEndpointDisplay});
                               evilOptions.push_back({"Back", [this]() { configMenu(); }});
                               loopOptions(evilOptions, MENU_TYPE_SUBMENU, "Evil Wifi Settings");
                           }});

    {

        String hidden__wifi_option = String("Hidden Networks:") + (showHiddenNetworks ? "ON" : "OFF");

        // construct Option explicitly using char* label
        Option opt(hidden__wifi_option.c_str(), [this]() {
            showHiddenNetworks = !showHiddenNetworks;
            displayInfo(String("Hidden Networks:") + (showHiddenNetworks ? "ON" : "OFF"), true);
            configMenu();
        });

        wifiOptions.push_back(opt);
    }
    wifiOptions.push_back({"Back", [this]() { optionsMenu(); }});
    loopOptions(wifiOptions, MENU_TYPE_SUBMENU, "WiFi Config");
}

void WifiMenu::drawIcon(float scale) {
    clearIconArea();
    int deltaY = scale * 20;
    int radius = scale * 6;

    tft.fillCircle(iconCenterX, iconCenterY + deltaY, radius, bruceConfig.priColor);
    tft.drawArc(
        iconCenterX,
        iconCenterY + deltaY,
        deltaY + radius,
        deltaY,
        130,
        230,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    tft.drawArc(
        iconCenterX,
        iconCenterY + deltaY,
        2 * deltaY + radius,
        2 * deltaY,
        130,
        230,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
}
