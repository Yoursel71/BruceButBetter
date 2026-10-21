#include "core/display.h" // drawStatusBar(), requestCompactMenuRedraw()
#include "core/powerSave.h"
#include "core/utils.h"
#include "ok_button.h"
#include <Wire.h>
#include <driver/rtc_io.h>
#include <interface.h>

/***************************************************************************************
** Function name: _setup_gpio()
** Description:   Initial hardware setup called before tft.init()
***************************************************************************************/
void _setup_gpio() {
    // Start I2C: shared bus for OLED (0x3C) + PN532 (0x24) + Si5351 (0x60)
    Wire.begin(GROVE_SDA, GROVE_SCL);
    // Fast-mode I2C (400kHz): speeds up OLED rendering. All devices on the
    // bus (SH1106, PN532, Si5351) support 400kHz.
    Wire.setClock(400000);

    // Hold SPI chip-select pins high to avoid bus contention
    pinMode(CC1101_SS_PIN, OUTPUT);
    digitalWrite(CC1101_SS_PIN, HIGH);
    pinMode(NRF24_SS_PIN, OUTPUT);
    digitalWrite(NRF24_SS_PIN, HIGH);
#ifdef NRF24_2_SS_PIN
    // Radio 2 shares MISO: a floating/LOW CSN can corrupt radio 1's first probe.
    pinMode(NRF24_2_SS_PIN, OUTPUT);
    digitalWrite(NRF24_2_SS_PIN, HIGH);
#endif
    pinMode(NRF24_CE_PIN, OUTPUT);
    digitalWrite(NRF24_CE_PIN, LOW);
#ifdef NRF24_2_CE_PIN
    pinMode(NRF24_2_CE_PIN, OUTPUT);
    digitalWrite(NRF24_2_CE_PIN, LOW);
#endif
    pinMode(SDCARD_CS, OUTPUT);
    digitalWrite(SDCARD_CS, HIGH);

    // EXT0 wake-up leaves UP in RTC mode; return it to the digital GPIO driver.
    rtc_gpio_deinit(static_cast<gpio_num_t>(UP_BTN));
    // Buttons: active-LOW, internal pull-up
    pinMode(SEL_BTN, INPUT_PULLUP);
    pinMode(UP_BTN,  INPUT_PULLUP);
    pinMode(DW_BTN,  INPUT_PULLUP);
    pinMode(L_BTN,   INPUT_PULLUP);
    pinMode(R_BTN,   INPUT_PULLUP);
    pinMode(ESC_BTN, INPUT_PULLUP);

    // Mark CC1101 as the default RF module
    bruceConfigPins.rfModule = CC1101_SPI_MODULE;
    bruceConfigPins.irRx     = RXLED;
}

/***************************************************************************************
** Function name: getBattery()
** Description:   No battery present, always return 0
***************************************************************************************/
int getBattery() { return 0; }

/***************************************************************************************
** Function name: isCharging()
***************************************************************************************/
bool isCharging() { return false; }

/***************************************************************************************
** Function name: _setBrightness()
** Description:   OLED has no built-in brightness control
***************************************************************************************/
void _setBrightness(uint8_t brightval) { (void)brightval; }

/***************************************************************************************
** Function name: InputHandler()
** Description:   6 buttons: UP, DOWN, LEFT, RIGHT, OK(SEL), BACK(ESC)
**                Active-LOW - all buttons wired to GND, internal pull-up enabled
**                Tap OK to select; hold OK for 800 ms to go back once.
***************************************************************************************/
void InputHandler(void) {
    static unsigned long tm = 0;
    static OkButtonGesture ok;
    static bool okWasDown = false;
    static bool suppressOk = false;
    const uint32_t now = millis();

    const auto action = ok.update(digitalRead(SEL_BTN) == BTN_ACT, now);
    if (ok.pressed() && !okWasDown) suppressOk = wakeUpScreen();
    okWasDown = ok.pressed();
    if (action != OkButtonGesture::Action::None && !suppressOk && !wakeUpScreen()) {
        AnyKeyPress = true;
        if (action == OkButtonGesture::Action::Back) EscPress = true;
        else SelPress = true;
    }
    if (!ok.pressed()) suppressOk = false;

    bool _up  = digitalRead(UP_BTN)  == BTN_ACT;
    bool _dn  = digitalRead(DW_BTN)  == BTN_ACT;
    bool _l   = digitalRead(L_BTN)   == BTN_ACT;
    bool _r   = digitalRead(R_BTN)   == BTN_ACT;
    bool _esc = digitalRead(ESC_BTN) == BTN_ACT;

    if (!(_up || _dn || _l || _r || _esc)) return;
    if (now - tm < 150 && !LongPress) return;

    tm = now;
    if (wakeUpScreen()) return;

    AnyKeyPress = true;

    if (_up)  { UpPress = true;   PrevPagePress = true; }
    if (_dn)  { DownPress = true; NextPagePress = true; }
    if (_l)   { PrevPress = true; }
    if (_r)   { NextPress = true; }
    if (_esc) { EscPress  = true; }
}

/***************************************************************************************
** Function name: powerOff()
** Description:   Deep sleep, wake up with the UP button (GPIO15).
**                On the ESP32-S3 only RTC GPIOs 0-21 can wake the chip, so OK (GPIO40)
**                cannot be used as the wake-up button.
***************************************************************************************/
void powerOff() {
    const gpio_num_t wakePin = static_cast<gpio_num_t>(UP_BTN);
    // The OLED keeps its last frame while the ESP32 sleeps, so blank it first.
    tft.fillScreen(TFT_BLACK);
    // Going to sleep with UP held would wake the chip again immediately.
    while (digitalRead(UP_BTN) == BTN_ACT) delay(10);
    if (esp_sleep_enable_ext0_wakeup(wakePin, 0) != ESP_OK) {
        Serial.println("powerOff: UP (GPIO15) cannot be set as wake-up pin");
        requestCompactMenuRedraw();
        return;
    }
    // Keep UP pulled high in deep sleep; the button pulls it low to wake.
    rtc_gpio_pullup_en(wakePin);
    rtc_gpio_pulldown_dis(wakePin);
    esp_deep_sleep_start();
}

/***************************************************************************************
** Function name: checkReboot()
** Description:   Holding OK + BACK together for 3 seconds powers the device off
**                (deep sleep). Press UP to wake it again.
***************************************************************************************/
void checkReboot() {
    if (digitalRead(SEL_BTN) != BTN_ACT || digitalRead(ESC_BTN) != BTN_ACT) return;

    uint32_t holdStart = millis();
    int countDown = 0;

    while (digitalRead(SEL_BTN) == BTN_ACT && digitalRead(ESC_BTN) == BTN_ACT) {
        uint32_t elapsed = millis() - holdStart;
        if (elapsed > 500) {
            int step = elapsed / 1000 + 1;
            if (step != countDown) {
                countDown = step;
                tft.setTextSize(1);
                tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
                tft.drawCentreString("OFF IN " + String(4 - countDown) + "...", tftWidth / 2, 10, 1);
            }
        }
        if (elapsed >= 3000) {
            tft.fillScreen(bruceConfig.bgColor);
            while (digitalRead(SEL_BTN) == BTN_ACT || digitalRead(ESC_BTN) == BTN_ACT) delay(10);
            delay(100);
            powerOff();
            return;
        }
        delay(20);
    }
    // Clear the countdown text if the buttons were released
    if (countDown > 0) {
        tft.fillRect(0, 5, tftWidth, 18, bruceConfig.bgColor);
        drawStatusBar();
#if defined(UI_COMPACT) && UI_COMPACT
        // The cleared strip also covers the top of the menu icon; redraw the whole menu.
        requestCompactMenuRedraw();
#endif
    }
}
