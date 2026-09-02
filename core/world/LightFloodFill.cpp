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
#include "LightFloodFill.h"

#include <array>
#include <queue>
#include <unordered_set>

#include "core/math/Vector2DIHash.h"

namespace {
    constexpr std::array<std::pair<int, int>, 8> NEIGHBORS = {
        {
            {-1, -1}, {0, -1}, {1, -1},
            {-1, 0}, {1, 0},
            {-1, 1}, {0, 1}, {1, 1}
        }
    };
}

void glimmer::LightFloodFill::Propagate(const TileVector2D &center, const int maxRadius,
                                        const OpaquePredicate &isOpaque,
                                        const VisitCallback &visit,
                                        const bool diagonalBlock) {
    if (!visit) {
        return;
    }
    // The center tile always emits light, even if the source tile itself is
    // solid (e.g. a glowing block).
    // 中心瓦片始终发光，即使光源瓦片本身是实体（如发光方块）。
    visit(center);
    if (maxRadius <= 0) {
        return;
    }

    const int maxRadiusSq = maxRadius * maxRadius;
    std::unordered_set<TileVector2D, Vector2DIHash> visited;
    visited.insert(center);
    std::queue<TileVector2D> queue;
    queue.push(center);

    while (!queue.empty()) {
        const TileVector2D current = queue.front();
        queue.pop();
        for (const auto &[dx, dy]: NEIGHBORS) {
            const TileVector2D next(current.x + dx, current.y + dy);
            if (visited.contains(next)) {
                continue;
            }
            const int relX = next.x - center.x;
            const int relY = next.y - center.y;
            if (relX * relX + relY * relY > maxRadiusSq) {
                continue;
            }
            // Diagonal anti-leak rule: a diagonal step is only allowed when at
            // least one of the two orthogonal neighbors is transparent.
            // 对角防漏光规则：仅当两个正交邻居中至少一个透光时才允许对角移动。
            if (diagonalBlock && dx != 0 && dy != 0) {
                const TileVector2D orthA(current.x + dx, current.y);
                const TileVector2D orthB(current.x, current.y + dy);
                if (isOpaque && isOpaque(orthA) && isOpaque(orthB)) {
                    continue;
                }
            }
            if (isOpaque && isOpaque(next)) {
                continue;
            }
            visited.insert(next);
            visit(next);
            queue.push(next);
        }
    }
}
