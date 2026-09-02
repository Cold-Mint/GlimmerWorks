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
#include "AmbientLight.h"

#include <array>
#include <cmath>

namespace {
    struct Keyframe {
        float hour;
        uint8_t r;
        uint8_t g;
        uint8_t b;
        float intensity;
    };

    constexpr std::array<Keyframe, 7> KEYFRAMES = {{
        {0.0F, 80, 100, 160, 0.10F},
        {4.0F, 230, 150, 90, 0.35F},
        {6.0F, 250, 240, 210, 0.70F},
        {12.0F, 255, 255, 250, 0.85F},
        {18.0F, 240, 150, 80, 0.50F},
        {20.0F, 90, 110, 170, 0.20F},
        {24.0F, 80, 100, 160, 0.10F},
    }};
}

glimmer::AmbientLight glimmer::ComputeAmbientLight(const float timeOfDay) {
    float t = std::fmod(timeOfDay, 24.0F);
    if (t < 0.0F) {
        t += 24.0F;
    }
    for (size_t i = 0; i + 1 < KEYFRAMES.size(); ++i) {
        const Keyframe &start = KEYFRAMES[i];
        const Keyframe &end = KEYFRAMES[i + 1];
        if (t >= start.hour && t <= end.hour) {
            const float span = end.hour - start.hour;
            const float u = span > 0.0F ? (t - start.hour) / span : 0.0F;
            AmbientLight result;
            result.color.r = static_cast<uint8_t>(std::lerp(static_cast<float>(start.r), static_cast<float>(end.r), u));
            result.color.g = static_cast<uint8_t>(std::lerp(static_cast<float>(start.g), static_cast<float>(end.g), u));
            result.color.b = static_cast<uint8_t>(std::lerp(static_cast<float>(start.b), static_cast<float>(end.b), u));
            result.color.a = 255;
            result.intensity = std::lerp(start.intensity, end.intensity, u);
            return result;
        }
    }
    AmbientLight result;
    result.color = Color(KEYFRAMES.back().r, KEYFRAMES.back().g, KEYFRAMES.back().b, 255);
    result.intensity = KEYFRAMES.back().intensity;
    return result;
}
