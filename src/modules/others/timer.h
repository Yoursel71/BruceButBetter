/**
 * @file timer.cpp
 * @author Aleksei Gor (https://github.com/AlekseiGor) - Reviewed and optimized by Senape3000
 * @brief Timer - Optimized implementation
 * @version 0.2
 * @date 2026-01-25
 */

#ifndef __TIMER_H__
#define __TIMER_H__

#include <globals.h>

class Timer {
private:
    int duration = 0;
#if defined(UI_COMPACT) && UI_COMPACT
    // 128x64: size-4 digits (192 px) overflow the width and the old y positions ran off the bottom.
    int fontSize = 2;
    int timerX = tftWidth / 2;
    int timerY = 12;
    int underlineY = timerY + (fontSize + 1) * LH;
#else
    int fontSize = 4;
    int timerX = tftWidth / 2;
    int timerY = tftHeight / 2;
    int underlineY = timerY + (fontSize + 1) * LH;
#endif
    bool playSoundOnFinish = true; // Sound option

    void clearUnderline();
    void underlineHours();
    void underlineMinutes();
    void underlineSeconds();
    void drawSoundOption(bool highlight);
    void playAlarmPattern();
    bool responsiveDelay(unsigned long ms);

public:
    Timer();
    ~Timer();

    void setup();
    void loop();
};

#endif
