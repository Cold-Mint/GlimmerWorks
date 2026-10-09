/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "ScanCodeUtils.h"

std::unordered_map<std::string, SDL_Scancode> glimmer::ScanCodeUtils::stringToScanMap_ = {
    {"unknown", SDL_SCANCODE_UNKNOWN},
    {"a", SDL_SCANCODE_A},
    {"b", SDL_SCANCODE_B},
    {"c", SDL_SCANCODE_C},
    {"d", SDL_SCANCODE_D},
    {"e", SDL_SCANCODE_E},
    {"f", SDL_SCANCODE_F},
    {"g", SDL_SCANCODE_G},
    {"h", SDL_SCANCODE_H},
    {"i", SDL_SCANCODE_I},
    {"j", SDL_SCANCODE_J},
    {"k", SDL_SCANCODE_K},
    {"l", SDL_SCANCODE_L},
    {"m", SDL_SCANCODE_M},
    {"n", SDL_SCANCODE_N},
    {"o", SDL_SCANCODE_O},
    {"p", SDL_SCANCODE_P},
    {"q", SDL_SCANCODE_Q},
    {"r", SDL_SCANCODE_R},
    {"s", SDL_SCANCODE_S},
    {"t", SDL_SCANCODE_T},
    {"u", SDL_SCANCODE_U},
    {"v", SDL_SCANCODE_V},
    {"w", SDL_SCANCODE_W},
    {"x", SDL_SCANCODE_X},
    {"y", SDL_SCANCODE_Y},
    {"z", SDL_SCANCODE_Z},
    {"1", SDL_SCANCODE_1},
    {"2", SDL_SCANCODE_2},
    {"3", SDL_SCANCODE_3},
    {"4", SDL_SCANCODE_4},
    {"5", SDL_SCANCODE_5},
    {"6", SDL_SCANCODE_6},
    {"7", SDL_SCANCODE_7},
    {"8", SDL_SCANCODE_8},
    {"9", SDL_SCANCODE_9},
    {"0", SDL_SCANCODE_0},
    {"return", SDL_SCANCODE_RETURN},
    {"escape", SDL_SCANCODE_ESCAPE},
    {"backspace", SDL_SCANCODE_BACKSPACE},
    {"tab", SDL_SCANCODE_TAB},
    {"space", SDL_SCANCODE_SPACE},
    {"minus", SDL_SCANCODE_MINUS},
    {"equals", SDL_SCANCODE_EQUALS},
    {"leftbracket", SDL_SCANCODE_LEFTBRACKET},
    {"rightbracket", SDL_SCANCODE_RIGHTBRACKET},
    {"backslash", SDL_SCANCODE_BACKSLASH},
    {"nonushash", SDL_SCANCODE_NONUSHASH},
    {"semicolon", SDL_SCANCODE_SEMICOLON},
    {"apostrophe", SDL_SCANCODE_APOSTROPHE},
    {"grave", SDL_SCANCODE_GRAVE},
    {"comma", SDL_SCANCODE_COMMA},
    {"period", SDL_SCANCODE_PERIOD},
    {"slash", SDL_SCANCODE_SLASH},
    {"capslock", SDL_SCANCODE_CAPSLOCK},
    {"f1", SDL_SCANCODE_F1},
    {"f2", SDL_SCANCODE_F2},
    {"f3", SDL_SCANCODE_F3},
    {"f4", SDL_SCANCODE_F4},
    {"f5", SDL_SCANCODE_F5},
    {"f6", SDL_SCANCODE_F6},
    {"f7", SDL_SCANCODE_F7},
    {"f8", SDL_SCANCODE_F8},
    {"f9", SDL_SCANCODE_F9},
    {"f10", SDL_SCANCODE_F10},
    {"f11", SDL_SCANCODE_F11},
    {"f12", SDL_SCANCODE_F12},
    {"f13", SDL_SCANCODE_F13},
    {"f14", SDL_SCANCODE_F14},
    {"f15", SDL_SCANCODE_F15},
    {"f16", SDL_SCANCODE_F16},
    {"f17", SDL_SCANCODE_F17},
    {"f18", SDL_SCANCODE_F18},
    {"f19", SDL_SCANCODE_F19},
    {"f20", SDL_SCANCODE_F20},
    {"f21", SDL_SCANCODE_F21},
    {"f22", SDL_SCANCODE_F22},
    {"f23", SDL_SCANCODE_F23},
    {"f24", SDL_SCANCODE_F24},
    {"printscreen", SDL_SCANCODE_PRINTSCREEN},
    {"scrolllock", SDL_SCANCODE_SCROLLLOCK},
    {"pause", SDL_SCANCODE_PAUSE},
    {"insert", SDL_SCANCODE_INSERT},
    {"home", SDL_SCANCODE_HOME},
    {"pageup", SDL_SCANCODE_PAGEUP},
    {"delete", SDL_SCANCODE_DELETE},
    {"end", SDL_SCANCODE_END},
    {"pagedown", SDL_SCANCODE_PAGEDOWN},
    {"right", SDL_SCANCODE_RIGHT},
    {"left", SDL_SCANCODE_LEFT},
    {"down", SDL_SCANCODE_DOWN},
    {"up", SDL_SCANCODE_UP},
    {"numlockclear", SDL_SCANCODE_NUMLOCKCLEAR},
    {"kp_divide", SDL_SCANCODE_KP_DIVIDE},
    {"kp_multiply", SDL_SCANCODE_KP_MULTIPLY},
    {"kp_minus", SDL_SCANCODE_KP_MINUS},
    {"kp_plus", SDL_SCANCODE_KP_PLUS},
    {"kp_enter", SDL_SCANCODE_KP_ENTER},
    {"kp_1", SDL_SCANCODE_KP_1},
    {"kp_2", SDL_SCANCODE_KP_2},
    {"kp_3", SDL_SCANCODE_KP_3},
    {"kp_4", SDL_SCANCODE_KP_4},
    {"kp_5", SDL_SCANCODE_KP_5},
    {"kp_6", SDL_SCANCODE_KP_6},
    {"kp_7", SDL_SCANCODE_KP_7},
    {"kp_8", SDL_SCANCODE_KP_8},
    {"kp_9", SDL_SCANCODE_KP_9},
    {"kp_0", SDL_SCANCODE_KP_0},
    {"kp_period", SDL_SCANCODE_KP_PERIOD},
    {"nonusbackslash", SDL_SCANCODE_NONUSBACKSLASH},
    {"application", SDL_SCANCODE_APPLICATION},
    {"power", SDL_SCANCODE_POWER},
    {"kp_equals", SDL_SCANCODE_KP_EQUALS},
    {"kp_comma", SDL_SCANCODE_KP_COMMA},
    {"kp_equalsas400", SDL_SCANCODE_KP_EQUALSAS400},
    {"kp_00", SDL_SCANCODE_KP_00},
    {"kp_000", SDL_SCANCODE_KP_000},
    {"international1", SDL_SCANCODE_INTERNATIONAL1},
    {"international2", SDL_SCANCODE_INTERNATIONAL2},
    {"international3", SDL_SCANCODE_INTERNATIONAL3},
    {"lang1", SDL_SCANCODE_LANG1},
    {"lang2", SDL_SCANCODE_LANG2},
    {"lang3", SDL_SCANCODE_LANG3},
    {"lang4", SDL_SCANCODE_LANG4},
    {"lang5", SDL_SCANCODE_LANG5},
    {"lctrl", SDL_SCANCODE_LCTRL},
    {"lshift", SDL_SCANCODE_LSHIFT},
    {"lalt", SDL_SCANCODE_LALT},
    {"lgui", SDL_SCANCODE_LGUI},
    {"rctrl", SDL_SCANCODE_RCTRL},
    {"rshift", SDL_SCANCODE_RSHIFT},
    {"ralt", SDL_SCANCODE_RALT},
    {"rgui", SDL_SCANCODE_RGUI},
    {"mode", SDL_SCANCODE_MODE},
    {"sleep", SDL_SCANCODE_SLEEP},
    {"wake", SDL_SCANCODE_WAKE},
    {"mute", SDL_SCANCODE_MUTE},
    {"volumeup", SDL_SCANCODE_VOLUMEUP},
    {"volumedown", SDL_SCANCODE_VOLUMEDOWN},
    {"media_play", SDL_SCANCODE_MEDIA_PLAY},
    {"media_pause", SDL_SCANCODE_MEDIA_PAUSE},
    {"media_play_pause", SDL_SCANCODE_MEDIA_PLAY_PAUSE},
    {"media_next_track", SDL_SCANCODE_MEDIA_NEXT_TRACK},
    {"media_previous_track", SDL_SCANCODE_MEDIA_PREVIOUS_TRACK},
    {"reserved", SDL_SCANCODE_RESERVED}
};

std::vector<std::string> glimmer::ScanCodeUtils::cachedKeys_ = [] {
    std::vector<std::string> keys;
    keys.reserve(stringToScanMap_.size());
    for (const auto &pair: stringToScanMap_) {
        keys.emplace_back(pair.first);
    }
    return keys;
}();

SDL_Scancode glimmer::ScanCodeUtils::StringToScanCode(const std::string &key) {
    const auto iter = stringToScanMap_.find(key);
    return iter == stringToScanMap_.end() ? SDL_SCANCODE_UNKNOWN : iter->second;
}

bool glimmer::ScanCodeUtils::ContainsKey(const std::string &key) {
    return stringToScanMap_.contains(key);
}

const std::vector<std::string> &glimmer::ScanCodeUtils::GetAllScanCodeKeys() {
    return cachedKeys_;
}
