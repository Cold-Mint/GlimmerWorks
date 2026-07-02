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


namespace glimmer
{
    class ColorUtils
    {

    public:
        /**
       * Obtain linear interpolated color value between two colors.
       * 在两个颜色之间获取线性插值混合色。
       * @param from 起始基准颜色
       * @param to 目标终点颜色
       * @param percent 插值系数，取值范围0~1；0返回from原色，1返回to原色，中间数值为两者混合过渡色
       * @return 线性插值计算后的混合颜色结果
       */
        static Color LinearInterpolateColor(const Color& from, const Color& to, float percent);

        /**
         * AverageColors
         * 平均颜色
         * @param colors
         * @return
         */
        static Color AverageColors(const std::vector<Color>& colors);

        /**
         * AdditiveBlend
         * 加法混合
         * @param firstColor
         * @param secondColor
         * @return
         */
        static Color AdditiveBlend(const Color& firstColor, const Color& secondColor);
    };
}
