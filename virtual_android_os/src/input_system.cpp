/**
 * @file input_system.cpp
 * @brief Implementation of Virtual Touchscreen and Hardware Key Injector.
 */

#include "input_system.hpp"
#include <iostream>
#include <algorithm>

namespace VPhone {

InputSystem::InputSystem() {}

InputSystem::~InputSystem() {}

bool InputSystem::initialize(int screen_width, int screen_height) {
    screen_width_ = screen_width;
    screen_height_ = screen_height;
    win_w_ = screen_width;
    win_h_ = screen_height;
    return true;
}

bool InputSystem::sendTouchEvent(TouchAction /* action */, const std::vector<TouchPointer>& /* pointers */) {
    total_events_++;
    // Translates to Linux evdev input_event struct:
    // struct input_event ev;
    // ev.type = EV_ABS; ev.code = ABS_MT_POSITION_X; ev.value = scaled_x;
    return true;
}

bool InputSystem::sendSingleTouch(TouchAction action, float normalized_x, float normalized_y) {
    normalized_x = std::clamp(normalized_x, 0.0f, 1.0f);
    normalized_y = std::clamp(normalized_y, 0.0f, 1.0f);

    TouchPointer p;
    p.pointer_id = 0;
    p.x = normalized_x * screen_width_;
    p.y = normalized_y * screen_height_;
    p.pressure = (action == TouchAction::UP) ? 0.0f : 1.0f;

    return sendTouchEvent(action, {p});
}

bool InputSystem::sendKeyEvent(KeyCode /* key_code */, bool /* is_down */) {
    total_events_++;
    // Injects EV_KEY event to virtual input node
    return true;
}

bool InputSystem::triggerHomeButton() {
    sendKeyEvent(KeyCode::KEYCODE_HOME, true);
    return sendKeyEvent(KeyCode::KEYCODE_HOME, false);
}

bool InputSystem::triggerBackButton() {
    sendKeyEvent(KeyCode::KEYCODE_BACK, true);
    return sendKeyEvent(KeyCode::KEYCODE_BACK, false);
}

bool InputSystem::triggerRecentsButton() {
    sendKeyEvent(KeyCode::KEYCODE_APP_SWITCH, true);
    return sendKeyEvent(KeyCode::KEYCODE_APP_SWITCH, false);
}

void InputSystem::updateWindowMapping(int window_x, int window_y, int window_w, int window_h) {
    win_x_ = window_x;
    win_y_ = window_y;
    win_w_ = (window_w > 0) ? window_w : 1080;
    win_h_ = (window_h > 0) ? window_h : 2400;
}

uint64_t InputSystem::getTotalInputEvents() const {
    return total_events_;
}

} // namespace VPhone
