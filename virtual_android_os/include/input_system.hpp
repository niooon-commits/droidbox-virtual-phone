/**
 * @file input_system.hpp
 * @brief Virtual Touchscreen Digitizer & Hardware Key Injection.
 *
 * Converts host touch events (MotionEvents) to guest Android /dev/input/event*
 * evdev protocol, handles touch scaling between floating window and guest 1080p canvas,
 * and virtualizes navigation keys (BACK, HOME, RECENTS).
 */

#ifndef INPUT_SYSTEM_HPP
#define INPUT_SYSTEM_HPP

#include <cstdint>
#include <vector>
#include <functional>

namespace VPhone {

enum class TouchAction {
    DOWN = 0,
    UP = 1,
    MOVE = 2,
    CANCEL = 3
};

enum class KeyCode {
    KEYCODE_HOME = 3,
    KEYCODE_BACK = 4,
    KEYCODE_VOLUME_UP = 24,
    KEYCODE_VOLUME_DOWN = 25,
    KEYCODE_POWER = 26,
    KEYCODE_APP_SWITCH = 187 // Recents
};

struct TouchPointer {
    int pointer_id;
    float x;
    float y;
    float pressure;
};

class InputSystem {
public:
    InputSystem();
    ~InputSystem();

    bool initialize(int screen_width, int screen_height);

    // Touch event routing
    bool sendTouchEvent(TouchAction action, const std::vector<TouchPointer>& pointers);
    bool sendSingleTouch(TouchAction action, float normalized_x, float normalized_y);

    // Key event injection
    bool sendKeyEvent(KeyCode key_code, bool is_down);
    bool triggerHomeButton();
    bool triggerBackButton();
    bool triggerRecentsButton();

    // Coordinate mapping (converts host window rectangle to virtual screen coordinates)
    void updateWindowMapping(int window_x, int window_y, int window_w, int window_h);

    uint64_t getTotalInputEvents() const;

private:
    int screen_width_ = 1080;
    int screen_height_ = 2400;
    int win_x_ = 0;
    int win_y_ = 0;
    int win_w_ = 1080;
    int win_h_ = 2400;

    uint64_t total_events_ = 0;
};

} // namespace VPhone

#endif // INPUT_SYSTEM_HPP
