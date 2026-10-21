#ifndef __MENU_ITEM_INTERFACE_H__
#define __MENU_ITEM_INTERFACE_H__

#include "core/display.h"
#include <globals.h>
#if defined(UI_COMPACT) && UI_COMPACT
#include "core/menu_items/compact_icons.h"
#endif

class MenuItemInterface {
public:
    virtual ~MenuItemInterface() = default;
    virtual void optionsMenu(void) = 0;
    virtual void drawIcon(float scale = 1) = 0;
    virtual void drawIconImg() {
        drawImg(
            *bruceConfig.themeFS(),
            bruceConfig.getThemeItemImg(themePath()),
            0,
            imgCenterY,
            true,
            bruceConfig.theme.gifDuration,
            false
        );
    }
    virtual bool hasTheme() = 0;
    virtual String themePath() = 0;

    bool checkTheme() { return hasTheme() && themePath() != ""; }
    String getName() const { return _name; }

#if defined(UI_COMPACT) && UI_COMPACT
    void setCompactPosition(int index, int count) {
        compactItemIndex = index;
        compactItemCount = count;
    }
#endif

    void draw(float scale = 1) {
#if defined(UI_COMPACT) && UI_COMPACT
        resetCoordinates();
        tft.fillRect(0, compactContentY, tftWidth, tftHeight - compactContentY, bruceConfig.bgColor);
        drawCompactMenuIcon(
            compactIcon(), iconCenterX, iconCenterY, bruceConfig.priColor, bruceConfig.bgColor
        );
        drawArrows();
        drawTitle();
        drawCompactPosition();
        drawStatusBar();
#else
        if (rotation != bruceConfigPins.rotation) resetCoordinates();
        if (!checkTheme()) {
            tft.fillRect(0, 27, tftWidth, tftHeight - 27, bruceConfig.bgColor);
            drawIcon(scale);
            drawArrows(scale);
            drawTitle(scale);
        } else {
            if (bruceConfig.theme.label)
                drawTitle(scale); // If using .GIF, labels are draw after complete, which takes some time
            drawIconImg();
            if (bruceConfig.theme.label) drawTitle(scale); // Makes sure to draw over the image
        }
        drawStatusBar();
#endif
    }

    void drawArrows(float scale = 1) {
        tft.fillRect(arrowAreaX, iconAreaY, arrowAreaW, iconAreaH, bruceConfig.bgColor);
        tft.fillRect(
            tftWidth - arrowAreaX - arrowAreaW, iconAreaY, arrowAreaW, iconAreaH, bruceConfig.bgColor
        );

#if defined(UI_COMPACT) && UI_COMPACT
        // Single-pixel chevrons stay legible on the monochrome display.
        const int arrowSize = 3;
        const int leftX = BORDER_PAD_X;
        const int rightX = tftWidth - 1 - leftX;
        const int arrowY = iconCenterY;
        tft.drawLine(leftX, arrowY, leftX + arrowSize, arrowY - arrowSize, bruceConfig.priColor);
        tft.drawLine(leftX, arrowY, leftX + arrowSize, arrowY + arrowSize, bruceConfig.priColor);
        tft.drawLine(rightX, arrowY, rightX - arrowSize, arrowY - arrowSize, bruceConfig.priColor);
        tft.drawLine(rightX, arrowY, rightX - arrowSize, arrowY + arrowSize, bruceConfig.priColor);
#else
        int arrowSize = scale * 10;
        int lineWidth = scale * 3;

        int arrowX = BORDER_PAD_X + 1.5 * arrowSize;
        int arrowY = iconCenterY + 1.5 * arrowSize;

        // Left Arrow
        tft.drawWideLine(
            arrowX,
            arrowY,
            arrowX + arrowSize,
            arrowY + arrowSize,
            lineWidth,
            bruceConfig.priColor,
            bruceConfig.bgColor
        );
        tft.drawWideLine(
            arrowX,
            arrowY,
            arrowX + arrowSize,
            arrowY - arrowSize,
            lineWidth,
            bruceConfig.priColor,
            bruceConfig.bgColor
        );

        // Right Arrow
        tft.drawWideLine(
            tftWidth - arrowX,
            arrowY,
            tftWidth - arrowX - arrowSize,
            arrowY + arrowSize,
            lineWidth,
            bruceConfig.priColor,
            bruceConfig.bgColor
        );
        tft.drawWideLine(
            tftWidth - arrowX,
            arrowY,
            tftWidth - arrowX - arrowSize,
            arrowY - arrowSize,
            lineWidth,
            bruceConfig.priColor,
            bruceConfig.bgColor
        );
#endif
    }

    void drawTitle(float scale = 1) {
#if defined(UI_COMPACT) && UI_COMPACT
        // One consistent caption row; the footer below it shows the carousel position.
        tft.setTextSize(1);
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.fillRect(0, compactTitleY, tftWidth, 8, bruceConfig.bgColor);
        const int nchars = (tftWidth - 2 * BORDER_PAD_X) / 6;
        tft.drawCentreString(getName().substring(0, nchars), iconCenterX, compactTitleY, 1);
#else
        int titleY = iconCenterY + iconAreaH / 2 + FG;

        tft.setTextSize(FM);
        tft.drawPixel(0, 0, 0);
        tft.fillRect(arrowAreaX, titleY, tftWidth - 2 * arrowAreaX, LH * FM, bruceConfig.bgColor);
        int nchars = (tftWidth - 16) / (LW * FM);
        tft.drawCentreString(getName().substring(0, nchars), iconCenterX, titleY, 1);
#endif
    }

protected:
    String _name = "";
    uint8_t rotation = ROTATION;

    int iconAreaH =
        ((tftHeight - 2 * BORDER_PAD_Y) % 2 == 0 ? tftHeight - 2 * BORDER_PAD_Y
                                                 : tftHeight - 2 * BORDER_PAD_Y + 1);
    int iconAreaW = iconAreaH;

    int iconCenterX = tftWidth / 2;
    int iconCenterY = tftHeight / 2;
    int imgCenterY = 13;

    int iconAreaX = iconCenterX - iconAreaW / 2;
    int iconAreaY = iconCenterY - iconAreaH / 2;

    int arrowAreaX = BORDER_PAD_X;
    int arrowAreaW = iconAreaX - arrowAreaX;

#if defined(UI_COMPACT) && UI_COMPACT
    int compactContentY = STATUS_BAR_HEIGHT;
    int compactTitleY = 0;
    int compactItemIndex = 0;
    int compactItemCount = 0;
    virtual CompactMenuIcon compactIcon() const { return CompactMenuIcon::Tools; }

    void drawCompactPosition() {
        tft.fillRect(0, tftHeight - 10, tftWidth, 10, bruceConfig.bgColor);
        if (compactItemCount < 2) return;
        int maxDots = (tftWidth - 48) / 5;
        if (maxDots < 1) maxDots = 1;
        if (maxDots > 14) maxDots = 14;
        const int count = compactItemCount < maxDots ? compactItemCount : maxDots;
        int first = compactItemIndex - count / 2;
        if (first < 0) first = 0;
        if (first + count > compactItemCount) first = compactItemCount - count;
        for (int i = 0; i < count; ++i) {
            const bool active = first + i == compactItemIndex;
            tft.fillRect(
                8 + i * 5, tftHeight - (active ? 7 : 4), active ? 3 : 2, active ? 4 : 1, bruceConfig.priColor
            );
        }
        tft.setTextSize(1);
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.drawRightString(
            String(compactItemIndex + 1) + "/" + String(compactItemCount), tftWidth - 3, tftHeight - 8, 1
        );
    }
#endif

    MenuItemInterface(const String &name) : _name(name) {}

    void clearIconArea(void) {
        tft.fillRect(iconAreaX, iconAreaY, iconAreaW, iconAreaH, bruceConfig.bgColor);
    }
#if defined(UI_COMPACT) && UI_COMPACT
    void clearImgArea(void) {
        tft.fillRect(0, compactContentY, tftWidth, tftHeight - compactContentY, bruceConfig.bgColor);
    }
#else
    void clearImgArea(void) { tft.fillRect(7, 27, tftWidth - 14, tftHeight - 34, bruceConfig.bgColor); }
#endif
    void resetCoordinates(void) {
#if defined(UI_COMPACT) && UI_COMPACT
        compactContentY = STATUS_BAR_HEIGHT;
        compactTitleY = tftHeight - 18;
        const int contentTop = compactContentY + 1;
        const int contentHeight = compactTitleY - 2 - contentTop;
        // Keep the full content width so the arrow cleanup does not erase wider glyphs.
        const int contentWidth = tftWidth - 2 * (BORDER_PAD_X + 8);
        iconAreaH = contentHeight > 0 ? contentHeight : 1;
        iconAreaW = contentWidth > 0 ? contentWidth : 1;
        iconCenterX = tftWidth / 2;
        iconCenterY = contentTop + contentHeight / 2;
        iconAreaX = iconCenterX - iconAreaW / 2;
        iconAreaY = iconCenterY - iconAreaH / 2;
        arrowAreaX = BORDER_PAD_X;
        arrowAreaW = iconAreaX - arrowAreaX;
        if (arrowAreaW < 0) arrowAreaW = 0;
        rotation = bruceConfigPins.rotation;
#else
        // Recalculate Center and ared due to portrait/landscape changings
        if (tftWidth > tftHeight) {
            iconAreaH =
                ((tftHeight - 2 * BORDER_PAD_Y) % 2 == 0 ? tftHeight - 2 * BORDER_PAD_Y
                                                         : tftHeight - 2 * BORDER_PAD_Y + 1);
        } else {
            iconAreaH =
                ((tftWidth - 2 * BORDER_PAD_Y) % 2 == 0 ? tftWidth - 2 * BORDER_PAD_Y
                                                        : tftWidth - 2 * BORDER_PAD_Y + 1);
        }

        iconAreaW = iconAreaH;

        iconCenterX = tftWidth / 2;
        iconCenterY = tftHeight / 2;

        iconAreaX = iconCenterX - iconAreaW / 2;
        iconAreaY = iconCenterY - iconAreaH / 2;

        arrowAreaX = BORDER_PAD_X;
        arrowAreaW = iconAreaX - arrowAreaX;

        rotation = bruceConfigPins.rotation;
#endif
    }

private:
};

#endif // __MENU_ITEM_INTERFACE_H__
