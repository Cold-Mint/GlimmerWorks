//
// Created by coldmint on 2026/5/3.
//

#include "MouseButtonUtils.h"

#include "SDL3/SDL_mouse.h"


std::unordered_map<std::string, u_int8_t> glimmer::MouseButtonUtils::stringToMouseButtonMap_ = {
    {"left", SDL_BUTTON_LEFT},
    {"middle", SDL_BUTTON_MIDDLE},
    {"right", SDL_BUTTON_RIGHT},
    {"x1", SDL_BUTTON_X1},
    {"x2", SDL_BUTTON_X2},
};

std::vector<std::string> glimmer::MouseButtonUtils::cachedKeys_ = [] {
    std::vector<std::string> keys;
    keys.reserve(stringToMouseButtonMap_.size());
    for (const auto &pair: stringToMouseButtonMap_) {
        keys.emplace_back(pair.first);
    }
    return keys;
}();

u_int8_t glimmer::MouseButtonUtils::StringToMouseButton(const std::string &buttonStr) {
    const auto iter = stringToMouseButtonMap_.find(buttonStr);
    return iter == stringToMouseButtonMap_.end() ? 0 : iter->second;
}

bool glimmer::MouseButtonUtils::ContainsKey(const std::string &buttonStr) {
    return stringToMouseButtonMap_.contains(buttonStr);
}

const std::vector<std::string> &glimmer::MouseButtonUtils::GetAllMouseButtonKeys() {
    return cachedKeys_;
}
