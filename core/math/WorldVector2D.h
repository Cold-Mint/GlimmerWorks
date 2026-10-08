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
#include "Vector2D.h"

namespace glimmer
{
    /**
     * Two-dimensional vector of world coordinates
     * 世界坐标二维向量
     * The unit of the world coordinate is logical pixels. It is affected by the camera's zoom.
     * 世界坐标的单位是逻辑像素。会受到相机缩放的影响。
     * For example: X + 1, when scaled by a factor of 2, then it actually moves 2 screen pixels to the right.
     * 例如：X+1,缩放为2倍，那么实际向右偏移了2屏幕像素。
     */
    class WorldVector2D final : public Vector2DBase<WorldVector2D>
    {
        using Vector2DBase::Vector2DBase;
    };
}
