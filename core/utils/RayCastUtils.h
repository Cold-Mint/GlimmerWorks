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

namespace glimmer {
    class ChunkManager;
    class Vector2D;
    class TileVector2D;
    class WorldContext;

    struct RayCastUtils {
        /**
         * TileRayCast
         * 瓦片射线检测
         *
        * Detects ground-layer tiles along a line segment from start to end. Returns the position of the first solid tile hit, or nullopt if none are intersected.
         * 用于检测地面层的瓦片，从起点到终点看作一条直线，如果直线穿过了固体瓦片，那么返回第一个固体瓦片的位置，如果没有穿过任何瓦片那么返回null。
         * @param chunkManager
         * @param startPosition
         * @param endPosition
         * @return
         */
        static std::optional<TileVector2D> TileRayCast(const ChunkManager *chunkManager, const TileVector2D& startPosition,
                                                       const TileVector2D& endPosition);
    };
}
