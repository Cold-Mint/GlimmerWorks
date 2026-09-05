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
#pragma once
#include <vector>

#include "core/math/Color.h"

namespace glimmer {
    struct LightKeyframe;
    struct SkyColorKeyframe;
    class WorldContext;

    /**
     * AmbientLight
     * 环境光
     */
    struct AmbientLight {
        //Ambient hue (RGB, alpha unused). 环境光色调（RGB，alpha 未使用）。
        Color color;
        //Ambient intensity (0..1). 环境光强度（0..1）。
        float intensity = 0.0F;
    };

    /**
     * SkyColors
     * 天空颜色（顶部与地平线）
     */
    struct SkyColors {
        Color top;
        Color horizon;
    };

    /**
     * ComputeAmbientLight
     * 根据一天中的时间和关键帧计算环境光的色调与强度（线性插值）。
     * @param timeOfDay timeOfDay 一天中的时间（0..1，0=清晨，0.5=午夜，1=次日清晨）
     * @param keyframes keyframes 环境光关键帧（按时间点升序）
     * @return 环境光
     */
    [[nodiscard]] AmbientLight ComputeAmbientLight(float timeOfDay, const std::vector<LightKeyframe> &keyframes);

    /**
     * ComputeAmbientLight
     * 读取当前维度的关键帧计算环境光；世界上下文为 null 时使用默认关键帧。
     * @param worldContext worldContext 世界上下文（可为 null）
     * @param timeOfDay timeOfDay 一天中的时间（0..1）
     * @return 环境光
     */
    [[nodiscard]] AmbientLight ComputeAmbientLight(const WorldContext *worldContext, float timeOfDay);

    /**
     * ComputeSkyColors
     * 读取当前维度的天空颜色关键帧并解析颜色资源；世界上下文为 null 或无关键帧时使用默认天空颜色。
     * @param worldContext worldContext 世界上下文（可为 null）
     * @param timeOfDay timeOfDay 一天中的时间（0..1）
     * @return 天空颜色
     */
    [[nodiscard]] SkyColors ComputeSkyColors(const WorldContext *worldContext, float timeOfDay);
}
