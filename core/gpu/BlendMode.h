/*
 * Copyright (C) 2025-2026  Cold-Mint <cold_mint@qq.com>
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
 * 版权(C) 2025-2026  Cold-Mint <cold_mint@qq.com>
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

#include <cstdint>

#include <SDL3/SDL_gpu.h>

namespace glimmer {
    /**
     * BlendMode
     * 颜色混合模式
     *
     * Strongly-typed wrapper over the `blendMode` uint8 field of
     * GPUPipelineResource. The numeric values are the data format that is
     * authored in the resource pack `pipelines/*.toml` files.
     * GPUPipelineResource 的 `blendMode` uint8 字段的强类型封装。数值即
     * 资源包 `pipelines/*.toml` 中约定的数据格式。
     */
    enum class BlendMode : uint8_t {
        /** No blending, destination is overwritten. 不混合，直接覆盖。 */
        Opaque = 0,
        /** Standard alpha blending. 标准 alpha 混合。 */
        Alpha = 1,
        /** Additive blending (lighting). 加法混合（光照）。 */
        Additive = 2,
        /** Multiply blending. 相乘混合。 */
        Multiply = 3,
        /** Pre-multiplied alpha blending. 预乘 alpha 混合。 */
        Premultiplied = 4,
    };

    /**
     * Convert a raw uint8 value to the BlendMode enum.
     * 将原始 uint8 值转换为 BlendMode 枚举。
     * @param value value 原始值
     */
    [[nodiscard]] BlendMode BlendModeFromUint8(uint8_t value);

    /**
     * Convert a BlendMode into an SDL3 GPU color target blend state.
     * 将 BlendMode 转换为 SDL3 GPU 颜色目标混合状态。
     * @param mode mode 混合模式
     */
    [[nodiscard]] SDL_GPUColorTargetBlendState ToColorTargetBlendState(BlendMode mode);
}
