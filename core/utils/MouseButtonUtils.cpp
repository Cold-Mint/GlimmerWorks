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
#include "MouseButtonUtils.h"

#include "SDL3/SDL_mouse.h"


std::unordered_map<std::string, uint8_t> glimmer::MouseButtonUtils::stringToMouseButtonMap_ = {
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

uint8_t glimmer::MouseButtonUtils::StringToMouseButton(const std::string &buttonStr) {
    const auto iter = stringToMouseButtonMap_.find(buttonStr);
    return iter == stringToMouseButtonMap_.end() ? 0 : iter->second;
}

bool glimmer::MouseButtonUtils::ContainsKey(const std::string &buttonStr) {
    return stringToMouseButtonMap_.contains(buttonStr);
}

const std::vector<std::string> &glimmer::MouseButtonUtils::GetAllMouseButtonKeys() {
    return cachedKeys_;
}
