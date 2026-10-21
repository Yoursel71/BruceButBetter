#pragma once
#include <stdint.h>

// Decide on release for a tap, so holding OK cannot select before going back.
class OkButtonGesture {
public:
    enum class Action { None, Select, Back };
    static constexpr uint32_t DebounceMs = 25;
    static constexpr uint32_t HoldMs = 800;

    constexpr Action update(bool pressed, uint32_t now) {
        if (pressed != rawPressed) {
            rawPressed = pressed;
            rawChangedAt = now;
        }
        if (stablePressed != rawPressed && uint32_t(now - rawChangedAt) >= DebounceMs) {
            stablePressed = rawPressed;
            if (stablePressed) {
                pressedAt = rawChangedAt;
                backSent = false;
            } else if (!backSent) {
                return uint32_t(rawChangedAt - pressedAt) >= HoldMs ? Action::Back : Action::Select;
            }
        }
        if (stablePressed && rawPressed && !backSent && uint32_t(now - pressedAt) >= HoldMs) {
            backSent = true;
            return Action::Back;
        }
        return Action::None;
    }

    constexpr bool pressed() const { return stablePressed; }

private:
    bool rawPressed = false;
    bool stablePressed = false;
    bool backSent = false;
    uint32_t rawChangedAt = 0;
    uint32_t pressedAt = 0;
};
