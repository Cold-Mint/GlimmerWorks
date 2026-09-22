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
#include <cstdint>

#include "core/math/Color.h"
#include "core/math/TileVector2D.h"


namespace glimmer {
    /**
     * LightAttenuation
     * 光照衰减模型
     */
    enum class LightAttenuation : uint8_t {
        /**
         * Linear falloff proportional to euclidean distance.
         * 与欧氏距离成正比的线性衰减。
         */
        Linear = 0,
        /**
         * Physically-inspired inverse-square falloff.
         * 物理启发的平方反比衰减。
         */
        InverseSquare = 1,
    };

    /**
     * LightDirection
     * 光照方向
     *
     * 统一了光源与光线遮照的方向语义：Radial 表示在 X/Y 平面内向四周泛洪的
     * 点光源，其遮挡来自瓦片的侧面遮罩；Downward 表示来自上方（+Y）向下的
     * 天光，其遮挡同样复用侧面遮罩；Backward 表示来自背景层（-Z）的背光，
     * 其遮挡来自瓦片的背面遮罩。环境光（Downward/Backward）没有中心与半径。
     */
    enum class LightDirection : uint8_t {
        Radial = 1,
        Downward = 2,
        Backward = 4,
    };

    class LightSource {
        TileVector2D center_ = {};
        int maxRadius_ = 0;
        Color emissionColor_ = {};
        LightAttenuation attenuation_ = LightAttenuation::Linear;
        LightDirection direction_ = LightDirection::Radial;

    public:
        explicit LightSource(const TileVector2D &center, int maxRadius, const Color &emissionColor);

        explicit LightSource(LightDirection direction, const Color &emissionColor);

        [[nodiscard]] LightDirection GetDirection() const;

        [[nodiscard]] int GetMaxRadius() const;

        [[nodiscard]] const TileVector2D &GetCenter() const;

        [[nodiscard]] const Color *GetEmissionColor() const;

        void SetEmissionColor(const Color &emissionColor);

        [[nodiscard]] LightAttenuation GetAttenuation() const;

        void SetAttenuation(LightAttenuation attenuation);

        /**
         * GetAttenuationFactor
         * 获取指定相对偏移处的衰减系数（0..1）。
         * @param dx dx 相对光源中心的 X 偏移
         * @param dy dy 相对光源中心的 Y 偏移
         * @return 0..1 的衰减系数
         */
        [[nodiscard]] float GetAttenuationFactor(int dx, int dy) const;
    };
}
