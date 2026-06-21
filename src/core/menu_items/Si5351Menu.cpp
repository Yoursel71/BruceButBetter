#include "Si5351Menu.h"
#ifdef HAS_SI5351
#include "core/display.h"
#include "core/utils.h"
#include "modules/si5351/si5351_module.h"

void Si5351Menu::optionsMenu() {
    options = {
        {"Set Frequency",  si5351_set_frequency_ui  },
        {"Live Output",    si5351_continuous_output  },
        {"Toggle CLK",     si5351_output_toggle      },
        {"Freq Sweep",     si5351_sweep              },
        {"AM Band",        si5351_am_band            },
        {"Presets",        si5351_presets            },
        {"Calibrate",      si5351_calibration        },
        {"Favorites",      si5351_favorites          },
    };
    addOptionToMainMenu();
    loopOptions(options, MENU_TYPE_SUBMENU, "RF Gen");
}

void Si5351Menu::drawIcon(float scale) {
    clearIconArea();

    int w  = scale * 70;
    int h  = scale * 50;
    int cx = iconCenterX;
    int cy = iconCenterY;

    if (w % 2) w++;
    if (h % 2) h++;

    // Oscillator body
    tft.drawRoundRect(cx - w / 2, cy - h / 2, w, h, w / 10, bruceConfig.priColor);

    // Sine wave inside
    int waveY = cy;
    int waveW = w - 10;
    int wX    = cx - waveW / 2;
    for (int i = 0; i < waveW - 1; i++) {
        float a1 = (float)i       / waveW * 4.0f * M_PI;
        float a2 = (float)(i + 1) / waveW * 4.0f * M_PI;
        int   y1 = waveY - (int)(sin(a1) * h / 4);
        int   y2 = waveY - (int)(sin(a2) * h / 4);
        tft.drawLine(wX + i, y1, wX + i + 1, y2, bruceConfig.priColor);
    }

    // Output pins (3 lines extending right)
    int pinX = cx + w / 2;
    int pinH = h / 5;
    for (int i = 0; i < 3; i++) {
        int pinY = cy - h / 4 + i * pinH;
        tft.drawFastHLine(pinX, pinY, (int)(scale * 12), bruceConfig.priColor);
    }
}

#endif // HAS_SI5351
