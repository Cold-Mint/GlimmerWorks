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
#include "CoordinateTransformer.h"

#include "TerrainRelativeVector2D.h"
#include "core/config/Constants.h"

SDL_FRect glimmer::CoordinateTransformer::GetViewportRect(const WorldVector2D &cameraPosition,
                                                          const ScreenVector2D &cameraSize, const float zoom) {
    const float scaledWidth = cameraSize.x / zoom;
    const float scaledHeight = cameraSize.y / zoom;
    return SDL_FRect{
        .x = cameraPosition.x - scaledWidth * 0.5F,
        .y = cameraPosition.y - scaledHeight * 0.5F,
        .w = scaledWidth,
        .h = scaledHeight
    };
}

glimmer::ScreenVector2D glimmer::CoordinateTransformer::WorldToScreen(const WorldVector2D &cameraPosition,
                                                                      const WorldVector2D &worldPosition,
                                                                      const ScreenVector2D &cameraSize,
                                                                      const float zoom) {
    const float offsetX = (worldPosition.x - cameraPosition.x) * zoom;
    const float offsetY = (worldPosition.y - cameraPosition.y) * zoom;
    return ScreenVector2D{
        cameraSize.x * 0.5F + offsetX,
        cameraSize.y * 0.5F - offsetY
    };
}

glimmer::WorldVector2D glimmer::CoordinateTransformer::ScreenToWorld(const WorldVector2D &cameraPosition,
                                                                     const ScreenVector2D &screenPosition,
                                                                     const ScreenVector2D &cameraSize,
                                                                     const float zoom) {
    return WorldVector2D{
        cameraPosition.x + (screenPosition.x - cameraSize.x * 0.5F) / zoom,
        cameraPosition.y + (cameraSize.y * 0.5F - screenPosition.y) / zoom
    };
}

glimmer::WorldVector2D glimmer::CoordinateTransformer::TileToWorld(const TileVector2D &tilePos) {
    return WorldVector2D{
        static_cast<float>(tilePos.x) * TILE_SIZE,
        static_cast<float>(tilePos.y) * TILE_SIZE
    };
}

glimmer::TileVector2D glimmer::CoordinateTransformer::WorldToTile(const WorldVector2D &worldPos) {
    return TileVector2D{
        static_cast<int>(std::floor(worldPos.x / TILE_SIZE + 0.5F)),
        static_cast<int>(std::floor(worldPos.y / TILE_SIZE + 0.5F))
    };
}

glimmer::ChunkVertexVector2D glimmer::CoordinateTransformer::TileToChunkVertex(const TileVector2D &tileVector2d) {
    return ChunkVertexVector2D{
        tileVector2d.x & CHUNK_ALIGN,
        tileVector2d.y & CHUNK_ALIGN
    };
}

glimmer::TileVector2D glimmer::CoordinateTransformer::
ChunkVertexToTile(const ChunkVertexVector2D &chunkVertexVector2d) {
    return TileVector2D{
        chunkVertexVector2d.x,
        chunkVertexVector2d.y
    };
}

glimmer::ChunkRelativeVector2D glimmer::CoordinateTransformer::TileToChunkRelative(const TileVector2D &tileVector2d) {
    return ChunkRelativeVector2D{
        static_cast<uint32_t>(tileVector2d.x & CHUNK_MASK),
        static_cast<uint32_t>(tileVector2d.y & CHUNK_MASK)
    };
}

glimmer::TileVector2D glimmer::CoordinateTransformer::ChunkRelativeToTile(const ChunkVertexVector2D &chunkVertex,
                                                                          const ChunkRelativeVector2D &relative) {
    return TileVector2D{static_cast<int>(chunkVertex.x + relative.x), static_cast<int>(chunkVertex.y + relative.y)};
}

glimmer::TerrainVertexVector2D glimmer::CoordinateTransformer::ChunkVertexToTerrainVertex(
    const ChunkVertexVector2D &chunkVertexVector2D) {
    return TerrainVertexVector2D{chunkVertexVector2D.x & TERRAIN_ALIGN, chunkVertexVector2D.y & TERRAIN_ALIGN};
}

glimmer::TerrainVertexVector2D glimmer::CoordinateTransformer::TileToTerrainVertex(const TileVector2D &tileVector2D) {
    return TerrainVertexVector2D{tileVector2D.x & TERRAIN_ALIGN, tileVector2D.y & TERRAIN_ALIGN};
}

glimmer::TerrainRelativeVector2D glimmer::CoordinateTransformer::
TileToTerrainRelative(const TileVector2D &tileVector2D) {
    return TerrainRelativeVector2D{
        static_cast<uint32_t>(tileVector2D.x & TERRAIN_MASK),
        static_cast<uint32_t>(tileVector2D.y & TERRAIN_MASK)
    };
}

size_t glimmer::CoordinateTransformer::GetArrayIndex(int x, int y, int width) {
    return y * width + x;
}

glimmer::TileVector2D glimmer::CoordinateTransformer::TerrainRelativeToTile(const TerrainVertexVector2D &terrainVertex,
                                                                            const TerrainRelativeVector2D &relative) {
    return TileVector2D{static_cast<int>(terrainVertex.x + relative.x), static_cast<int>(terrainVertex.y + relative.y)};
}
