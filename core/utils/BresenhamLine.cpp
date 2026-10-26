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
#include "BresenhamLine.h"

#include <algorithm>
#include <cstdlib>

#include "core/math/Vector2DI.h"

glimmer::BresenhamLine::BresenhamLine(const Vector2DI &vector, const int stepLength) : stepLength_(stepLength) {
    dx_ = std::abs(vector.x);
    dy_ = std::abs(vector.y);
    sx_ = vector.x > 0 ? 1 : vector.x < 0 ? -1 : 0;
    sy_ = vector.y > 0 ? 1 : vector.y < 0 ? -1 : 0;
    err_ = dx_ - dy_;
    //Bresenham reaches the endpoint in max(dx, dy) steps.
    //布雷森汉姆算法最多需要 max(dx, dy) 步到达终点。
    remaining_ = std::max(dx_, dy_);
}

std::optional<glimmer::Vector2DI> glimmer::BresenhamLine::NextPoint() {
    std::optional<Vector2DI> result = std::nullopt;
    if (done_) {
        return result;
    }
    result = current_;
    if (remaining_ <= 0) {
        //Already standing at the endpoint: return it once, then signal completion.
        //已经站在终点：本次返回终点，下次返回 std::nullopt。
        done_ = true;
        return result;
    }

    const int steps = std::min(stepLength_, remaining_);
    for (int i = 0; i < steps; ++i) {
        const int e2 = 2 * err_;
        if (e2 > -dy_) {
            err_ -= dy_;
            current_.x += sx_;
        }
        if (e2 < dx_) {
            err_ += dx_;
            current_.y += sy_;
        }
    }
    remaining_ -= steps;
    return result;
}
