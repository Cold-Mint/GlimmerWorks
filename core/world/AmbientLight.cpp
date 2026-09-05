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

#include <cmath>

#include "core/mod/Resource.h"
#include "core/world/WorldContext.h"

glimmer::AmbientLight glimmer::ComputeAmbientLight(const float timeOfDay,
                                                   const std::vector<LightKeyframe> &keyframes) {
    if (keyframes.empty()) {
        return ComputeAmbientLight(timeOfDay, DimensionResource::GetDefaultAmbientLightKeyframes());
    }
    float t = std::fmod(timeOfDay, 1.0F);
    if (t < 0.0F) {
        t += 1.0F;
    }
    for (size_t i = 0; i + 1 < keyframes.size(); ++i) {
        const LightKeyframe &start = keyframes[i];
        const LightKeyframe &end = keyframes[i + 1];
        if (t >= start.t && t <= end.t) {
            const float span = end.t - start.t;
            const float u = span > 0.0F ? (t - start.t) / span : 0.0F;
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
    result.color = Color(keyframes.back().r, keyframes.back().g, keyframes.back().b, 255);
    result.intensity = keyframes.back().intensity;
    return result;
}

glimmer::AmbientLight glimmer::ComputeAmbientLight(const WorldContext *worldContext, const float timeOfDay) {
    const std::vector<LightKeyframe> &keyframes = worldContext != nullptr
                                                      ? worldContext->GetAmbientLightKeyframes()
                                                      : DimensionResource::GetDefaultAmbientLightKeyframes();
    return ComputeAmbientLight(timeOfDay, keyframes);
}
