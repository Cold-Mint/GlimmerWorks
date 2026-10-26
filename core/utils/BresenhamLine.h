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
#include <optional>

#include "core/math/Vector2DI.h"

namespace glimmer {
    /**
     * BresenhamLine
     * 布雷森汉姆直线遍历器
     *
     * Walks the tile grid along a line segment using Bresenham's line algorithm.
     * 使用布雷森汉姆直线算法沿线段遍历瓦片网格。
     *
     * Construct with a vector (end - start) and a step length. NextPoint() returns each
     * point from the origin (0,0) to the vector's end in order, and returns std::nullopt
     * once the end is reached.
     * 使用向量（终点 - 起点）和步长构造。NextPoint() 依次返回从原点(0,0)到向量终点之间的
     * 每个点，走到尽头后返回 std::nullopt。
     */
    class BresenhamLine {
        Vector2DI current_{0, 0};
        int dx_ = 0;
        int dy_ = 0;
        int sx_ = 0;
        int sy_ = 0;
        int err_ = 0;
        int stepLength_ = 1;
        int remaining_ = 0;
        bool done_ = false;

    public:
        /**
         * BresenhamLine
         * @param vector vector 向量（终点 - 起点）
         * @param stepLength stepLength 每次调用 NextPoint 前进的格数，默认为 1
         */
        explicit BresenhamLine(const Vector2DI &vector, int stepLength = 1);

        /**
         * NextPoint
         * 返回直线上的下一个点。
         * @return 下一个点；std::nullopt 表示已经走到尽头
         */
        std::optional<Vector2DI> NextPoint();
    };
}
