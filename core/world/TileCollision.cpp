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
#include "TileCollision.h"

#include <cmath>
#include <limits>

#include "core/config/Constants.h"
#include "core/math/CoordinateTransformer.h"
#include "core/world/ChunkManager.h"
#include "core/world/Tile.h"
#include "core/world/WorldContext.h"
#include "core/world/generator/Chunk.h"
#include "core/world/generator/TileLayerType.h"
#include "core/world/generator/TilePhysicsType.h"

bool glimmer::TileCollision::IsSolidTile(const ChunkManager *chunkManager, const TileVector2D &tile) {
    if (chunkManager == nullptr) {
        return false;
    }
    const Chunk *chunk = chunkManager->GetChunk(CoordinateTransformer::TileToChunkVertex(tile));
    if (chunk == nullptr) {
        return false;
    }
    const ChunkRelativeVector2D relative = CoordinateTransformer::TileToChunkRelative(tile);
    const Tile *tilePtr = chunk->GetTile(TileLayerType::Ground,
                                         CoordinateTransformer::GetArrayIndex(relative.x, relative.y, CHUNK_SHIFT));
    return tilePtr != nullptr && tilePtr->GetTilePhysicsType() == TilePhysicsType::Static;
}

bool glimmer::TileCollision::LineHitsSolidTile(const WorldContext *worldContext,
                                               const WorldVector2D &a, const WorldVector2D &b) {
    if (worldContext == nullptr) {
        return false;
    }
    const ChunkManager *chunkManager = worldContext->GetChunkManager();
    // Shift into "tile-centered" space so that tile boundaries lie on integer coordinates; this makes the
    // DDA grid-line crossing math correct for the tile-center convention.
    // 平移到“以瓦片为中心”的空间，使瓦片边界落在整数坐标上；这使 DDA 的网格线穿越计算符合瓦片中心约定。
    const float x0 = a.x / TILE_SIZE + 0.5F;
    const float y0 = a.y / TILE_SIZE + 0.5F;
    const float x1 = b.x / TILE_SIZE + 0.5F;
    const float y1 = b.y / TILE_SIZE + 0.5F;

    int x = static_cast<int>(std::floor(x0));
    int y = static_cast<int>(std::floor(y0));
    const int endX = static_cast<int>(std::floor(x1));
    const int endY = static_cast<int>(std::floor(y1));

    if (IsSolidTile(chunkManager, TileVector2D{x, y})) {
        return true;
    }

    const float dx = x1 - x0;
    const float dy = y1 - y0;
    const int stepX = dx > 0.0F ? 1 : (dx < 0.0F ? -1 : 0);
    const int stepY = dy > 0.0F ? 1 : (dy < 0.0F ? -1 : 0);
    const float absDx = std::abs(dx);
    const float absDy = std::abs(dy);
    const float infinity = std::numeric_limits<float>::infinity();

    float tMaxX = stepX != 0 ? (stepX > 0 ? (x + 1.0F - x0) : (x0 - x)) / absDx : infinity;
    float tMaxY = stepY != 0 ? (stepY > 0 ? (y + 1.0F - y0) : (y0 - y)) / absDy : infinity;
    const float tDeltaX = stepX != 0 ? 1.0F / absDx : infinity;
    const float tDeltaY = stepY != 0 ? 1.0F / absDy : infinity;

    while (x != endX || y != endY) {
        if (tMaxX < tMaxY) {
            x += stepX;
            tMaxX += tDeltaX;
        } else {
            y += stepY;
            tMaxY += tDeltaY;
        }
        if (IsSolidTile(chunkManager, TileVector2D{x, y})) {
            return true;
        }
    }
    return false;
}
