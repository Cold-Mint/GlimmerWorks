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
#include "ColorUtils.h"

#include <algorithm>
#include <cmath>


glimmer::Color glimmer::ColorUtils::LinearInterpolateColor(const Color& from, const Color& to, const float percent)
{
    const float blend_ratio = std::clamp(percent, 0.0F, 1.0F);
    const auto r = static_cast<uint8_t>(static_cast<float>(from.r) + static_cast<float>(to.r - from.r) * blend_ratio);
    const auto g = static_cast<uint8_t>(static_cast<float>(from.g) + static_cast<float>(to.g - from.g) * blend_ratio);
    const auto b = static_cast<uint8_t>(static_cast<float>(from.b) + static_cast<float>(to.b - from.b) * blend_ratio);
    const auto a = static_cast<uint8_t>(static_cast<float>(from.a) + static_cast<float>(to.a - from.a) * blend_ratio);
    return {r, g, b, a};
}

glimmer::Color glimmer::ColorUtils::AverageColors(const std::vector<Color>& colors)
{
    if (colors.empty())
    {
        return {0, 0, 0, 0};
    }
    int totalR = 0;
    int totalG = 0;
    int totalB = 0;
    int totalA = 0;
    for (const auto& color : colors)
    {
        totalR += color.r;
        totalG += color.g;
        totalB += color.b;
        totalA += color.a;
    }

    const auto count = static_cast<int>(colors.size());
    const auto r = static_cast<uint8_t>(std::clamp(totalR / count, 0, 255));
    const auto g = static_cast<uint8_t>(std::clamp(totalG / count, 0, 255));
    const auto b = static_cast<uint8_t>(std::clamp(totalB / count, 0, 255));
    const auto a = static_cast<uint8_t>(std::clamp(totalA / count, 0, 255));
    return {r, g, b, a};
}

ImVec4 glimmer::ColorUtils::ColorToImVec4(const Color& color)
{
    return {
       static_cast<float>(color.r) / 255,
       static_cast<float>(color.g) / 255,
       static_cast<float>(color.b) / 255,
       static_cast<float>(color.a) / 255
   };
}

glimmer::Color glimmer::ColorUtils::AdditiveBlend(const Color& firstColor, const Color& secondColor)
{
    Color result{};
    result.r = static_cast<uint8_t>(std::min(255, static_cast<int>(firstColor.r) + secondColor.r));
    result.g = static_cast<uint8_t>(std::min(255, static_cast<int>(firstColor.g) + secondColor.g));
    result.b = static_cast<uint8_t>(std::min(255, static_cast<int>(firstColor.b) + secondColor.b));
    result.a = std::max(firstColor.a, secondColor.a);
    return result;
}
