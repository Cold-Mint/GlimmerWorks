//
// Created by coldmint on 2026/5/3.
//

#include "EventTypeUtils.h"

std::unordered_map<std::string, SDL_EventType> glimmer::EventTypeUtils::stringToEventMap_ = {
    {"event_first", SDL_EVENT_FIRST},
    {"quit", SDL_EVENT_QUIT},
    {"terminating", SDL_EVENT_TERMINATING},
    {"low_memory", SDL_EVENT_LOW_MEMORY},
    {"will_enter_background", SDL_EVENT_WILL_ENTER_BACKGROUND},
    {"did_enter_background", SDL_EVENT_DID_ENTER_BACKGROUND},
    {"will_enter_foreground", SDL_EVENT_WILL_ENTER_FOREGROUND},
    {"did_enter_foreground", SDL_EVENT_DID_ENTER_FOREGROUND},
    {"locale_changed", SDL_EVENT_LOCALE_CHANGED},
    {"system_theme_changed", SDL_EVENT_SYSTEM_THEME_CHANGED},
    {"display_orientation", SDL_EVENT_DISPLAY_ORIENTATION},
    {"display_added", SDL_EVENT_DISPLAY_ADDED},
    {"display_removed", SDL_EVENT_DISPLAY_REMOVED},
    {"display_moved", SDL_EVENT_DISPLAY_MOVED},
    {"display_desktop_mode_changed", SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED},
    {"display_current_mode_changed", SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED},
    {"display_content_scale_changed", SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED},
    {"display_usable_bounds_changed", SDL_EVENT_DISPLAY_USABLE_BOUNDS_CHANGED},
    {"display_first", SDL_EVENT_DISPLAY_FIRST},
    {"display_last", SDL_EVENT_DISPLAY_LAST},
    {"window_shown", SDL_EVENT_WINDOW_SHOWN},
    {"window_hidden", SDL_EVENT_WINDOW_HIDDEN},
    {"window_exposed", SDL_EVENT_WINDOW_EXPOSED},
    {"window_moved", SDL_EVENT_WINDOW_MOVED},
    {"window_resized", SDL_EVENT_WINDOW_RESIZED},
    {"window_pixel_size_changed", SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED},
    {"window_metal_view_resized", SDL_EVENT_WINDOW_METAL_VIEW_RESIZED},
    {"window_minimized", SDL_EVENT_WINDOW_MINIMIZED},
    {"window_maximized", SDL_EVENT_WINDOW_MAXIMIZED},
    {"window_restored", SDL_EVENT_WINDOW_RESTORED},
    {"window_mouse_enter", SDL_EVENT_WINDOW_MOUSE_ENTER},
    {"window_mouse_leave", SDL_EVENT_WINDOW_MOUSE_LEAVE},
    {"window_focus_gained", SDL_EVENT_WINDOW_FOCUS_GAINED},
    {"window_focus_lost", SDL_EVENT_WINDOW_FOCUS_LOST},
    {"window_close_requested", SDL_EVENT_WINDOW_CLOSE_REQUESTED},
    {"window_hit_test", SDL_EVENT_WINDOW_HIT_TEST},
    {"window_iccprof_changed", SDL_EVENT_WINDOW_ICCPROF_CHANGED},
    {"window_display_changed", SDL_EVENT_WINDOW_DISPLAY_CHANGED},
    {"window_display_scale_changed", SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED},
    {"window_safe_area_changed", SDL_EVENT_WINDOW_SAFE_AREA_CHANGED},
    {"window_occluded", SDL_EVENT_WINDOW_OCCLUDED},
    {"window_enter_fullscreen", SDL_EVENT_WINDOW_ENTER_FULLSCREEN},
    {"window_leave_fullscreen", SDL_EVENT_WINDOW_LEAVE_FULLSCREEN},
    {"window_destroyed", SDL_EVENT_WINDOW_DESTROYED},
    {"window_hdr_state_changed", SDL_EVENT_WINDOW_HDR_STATE_CHANGED},
    {"window_first", SDL_EVENT_WINDOW_FIRST},
    {"window_last", SDL_EVENT_WINDOW_LAST},
    {"key_down", SDL_EVENT_KEY_DOWN},
    {"key_up", SDL_EVENT_KEY_UP},
    {"text_editing", SDL_EVENT_TEXT_EDITING},
    {"text_input", SDL_EVENT_TEXT_INPUT},
    {"keymap_changed", SDL_EVENT_KEYMAP_CHANGED},
    {"keyboard_added", SDL_EVENT_KEYBOARD_ADDED},
    {"keyboard_removed", SDL_EVENT_KEYBOARD_REMOVED},
    {"text_editing_candidates", SDL_EVENT_TEXT_EDITING_CANDIDATES},
    {"screen_keyboard_shown", SDL_EVENT_SCREEN_KEYBOARD_SHOWN},
    {"screen_keyboard_hidden", SDL_EVENT_SCREEN_KEYBOARD_HIDDEN},
    {"mouse_motion", SDL_EVENT_MOUSE_MOTION},
    {"mouse_button_down", SDL_EVENT_MOUSE_BUTTON_DOWN},
    {"mouse_button_up", SDL_EVENT_MOUSE_BUTTON_UP},
    {"mouse_wheel", SDL_EVENT_MOUSE_WHEEL},
    {"mouse_added", SDL_EVENT_MOUSE_ADDED},
    {"mouse_removed", SDL_EVENT_MOUSE_REMOVED},
    {"joystick_axis_motion", SDL_EVENT_JOYSTICK_AXIS_MOTION},
    {"joystick_ball_motion", SDL_EVENT_JOYSTICK_BALL_MOTION},
    {"joystick_hat_motion", SDL_EVENT_JOYSTICK_HAT_MOTION},
    {"joystick_button_down", SDL_EVENT_JOYSTICK_BUTTON_DOWN},
    {"joystick_button_up", SDL_EVENT_JOYSTICK_BUTTON_UP},
    {"joystick_added", SDL_EVENT_JOYSTICK_ADDED},
    {"joystick_removed", SDL_EVENT_JOYSTICK_REMOVED},
    {"joystick_battery_updated", SDL_EVENT_JOYSTICK_BATTERY_UPDATED},
    {"joystick_update_complete", SDL_EVENT_JOYSTICK_UPDATE_COMPLETE},
    {"gamepad_axis_motion", SDL_EVENT_GAMEPAD_AXIS_MOTION},
    {"gamepad_button_down", SDL_EVENT_GAMEPAD_BUTTON_DOWN},
    {"gamepad_button_up", SDL_EVENT_GAMEPAD_BUTTON_UP},
    {"gamepad_added", SDL_EVENT_GAMEPAD_ADDED},
    {"gamepad_removed", SDL_EVENT_GAMEPAD_REMOVED},
    {"gamepad_remapped", SDL_EVENT_GAMEPAD_REMAPPED},
    {"gamepad_touchpad_down", SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN},
    {"gamepad_touchpad_motion", SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION},
    {"gamepad_touchpad_up", SDL_EVENT_GAMEPAD_TOUCHPAD_UP},
    {"gamepad_sensor_update", SDL_EVENT_GAMEPAD_SENSOR_UPDATE},
    {"gamepad_update_complete", SDL_EVENT_GAMEPAD_UPDATE_COMPLETE},
    {"gamepad_steam_handle_updated", SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED},
    {"finger_down", SDL_EVENT_FINGER_DOWN},
    {"finger_up", SDL_EVENT_FINGER_UP},
    {"finger_motion", SDL_EVENT_FINGER_MOTION},
    {"finger_canceled", SDL_EVENT_FINGER_CANCELED},
    {"pinch_begin", SDL_EVENT_PINCH_BEGIN},
    {"pinch_update", SDL_EVENT_PINCH_UPDATE},
    {"pinch_end", SDL_EVENT_PINCH_END},
    {"clipboard_update", SDL_EVENT_CLIPBOARD_UPDATE},
    {"drop_file", SDL_EVENT_DROP_FILE},
    {"drop_text", SDL_EVENT_DROP_TEXT},
    {"drop_begin", SDL_EVENT_DROP_BEGIN},
    {"drop_complete", SDL_EVENT_DROP_COMPLETE},
    {"drop_position", SDL_EVENT_DROP_POSITION},
    {"audio_device_added", SDL_EVENT_AUDIO_DEVICE_ADDED},
    {"audio_device_removed", SDL_EVENT_AUDIO_DEVICE_REMOVED},
    {"audio_device_format_changed", SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED},
    {"sensor_update", SDL_EVENT_SENSOR_UPDATE},
    {"pen_proximity_in", SDL_EVENT_PEN_PROXIMITY_IN},
    {"pen_proximity_out", SDL_EVENT_PEN_PROXIMITY_OUT},
    {"pen_down", SDL_EVENT_PEN_DOWN},
    {"pen_up", SDL_EVENT_PEN_UP},
    {"pen_button_down", SDL_EVENT_PEN_BUTTON_DOWN},
    {"pen_button_up", SDL_EVENT_PEN_BUTTON_UP},
    {"pen_motion", SDL_EVENT_PEN_MOTION},
    {"pen_axis", SDL_EVENT_PEN_AXIS},
    {"camera_device_added", SDL_EVENT_CAMERA_DEVICE_ADDED},
    {"camera_device_removed", SDL_EVENT_CAMERA_DEVICE_REMOVED},
    {"camera_device_approved", SDL_EVENT_CAMERA_DEVICE_APPROVED},
    {"camera_device_denied", SDL_EVENT_CAMERA_DEVICE_DENIED},
    {"render_targets_reset", SDL_EVENT_RENDER_TARGETS_RESET},
    {"render_device_reset", SDL_EVENT_RENDER_DEVICE_RESET},
    {"render_device_lost", SDL_EVENT_RENDER_DEVICE_LOST},
    {"private0", SDL_EVENT_PRIVATE0},
    {"private1", SDL_EVENT_PRIVATE1},
    {"private2", SDL_EVENT_PRIVATE2},
    {"private3", SDL_EVENT_PRIVATE3},
    {"poll_sentinel", SDL_EVENT_POLL_SENTINEL},
    {"user", SDL_EVENT_USER},
    {"last", SDL_EVENT_LAST},
    {"enum_padding", SDL_EVENT_ENUM_PADDING}
};

std::vector<std::string> glimmer::EventTypeUtils::cachedKeys_ = [] {
    std::vector<std::string> keys;
    keys.reserve(stringToEventMap_.size());
    for (const auto &pair: stringToEventMap_) {
        keys.emplace_back(pair.first);
    }
    return keys;
}();

SDL_EventType glimmer::EventTypeUtils::StringToEventType(const std::string &eventStr) {
    const auto iter = stringToEventMap_.find(eventStr);
    return iter != stringToEventMap_.end() ? iter->second : SDL_EVENT_FIRST;
}

bool glimmer::EventTypeUtils::ContainsKey(const std::string &eventStr) {
    return stringToEventMap_.contains(eventStr);
}

const std::vector<std::string> &glimmer::EventTypeUtils::GetAllEventTypeKeys() {
    return cachedKeys_;
}
