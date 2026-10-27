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
#include "core/math/TileVector2D.h"

namespace glimmer {
    struct SolveResult {
        // The actual distance that can be moved along this axis (signed, in pixels).
        // 0 means it cannot move at all in this direction.
        // 沿该轴实际能移动的距离（有符号，单位：像素）。0 表示一步都走不动。
        float distance = 0.0F;

        // Whether the movement was blocked by a solid tile.
        // 是否被实心瓦片挡住。
        bool blocked = false;

        // The tile coordinate of the tile that blocked the movement.
        // Only valid when blocked is true.
        // 挡住移动的那个瓦片的坐标。仅在 blocked 为 true 时有效。
        TileVector2D hitTile{0, 0};

        // The corrected velocity along this axis (zeroed when blocked).
        // 该轴修正后的速度（被挡则为 0）。
        float velocity = 0.0F;

        // Whether the body landed on a surface this tick (vertical solver only).
        // 本 tick 是否落地（仅竖直求解器设置）。
        bool grounded = false;
    };
}
