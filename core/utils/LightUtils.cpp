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
#include "LightUtils.h"

#include <algorithm>
#include <cmath>

#include "core/log/LogCat.h"
#include "core/math/Color.h"


std::unique_ptr<glimmer::Color> glimmer::LightUtils::ApplyLightingMask(const Color *light, const Color *mask,
                                                                       const float tintFactor) {
    auto result = std::make_unique<Color>();
    const float intensity = static_cast<float>(light->a) / 255.0F;
    const float maskStrength = static_cast<float>(mask->a) / 255.0F;
    const float pass = 1.0F - maskStrength;
    const float finalScale = intensity * pass;
    result->r = static_cast<uint8_t>(std::lerp(static_cast<float>(light->r), static_cast<float>(mask->r), tintFactor));
    result->g = static_cast<uint8_t>(std::lerp(static_cast<float>(light->g), static_cast<float>(mask->g), tintFactor));
    result->b = static_cast<uint8_t>(std::lerp(static_cast<float>(light->b), static_cast<float>(mask->b), tintFactor));
    result->a = static_cast<uint8_t>(finalScale * 255.0F);
    return result;
}

std::unique_ptr<glimmer::Color> glimmer::LightUtils::MixLights(const Color *colorA, const Color *colorB) {
    auto result = std::make_unique<Color>();
    result->r = static_cast<uint8_t>(std::min(255, static_cast<int>(colorA->r) + colorB->r));
    result->g = static_cast<uint8_t>(std::min(255, static_cast<int>(colorA->g) + colorB->g));
    result->b = static_cast<uint8_t>(std::min(255, static_cast<int>(colorA->b) + colorB->b));
    result->a = std::max(colorA->a, colorB->a);
    return result;
}
