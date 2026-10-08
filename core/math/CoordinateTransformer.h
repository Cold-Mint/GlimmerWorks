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
#include <SDL3/SDL_rect.h>

#include "ChunkRelativeVector2D.h"
#include "ChunkVertexVector2D.h"
#include "ScreenVector2D.h"
#include "TerrainRelativeVector2D.h"
#include "TerrainVertexVector2D.h"
#include "TileVector2D.h"
#include "WorldVector2D.h"

namespace glimmer
{
    struct CoordinateTransformer final
    {
        /**
         * Get the camera viewport rectangle (in world coordinates)
         * 获取相机的视口矩形（世界坐标）
         * @param cameraPosition The camera position in world coordinates 相机位置（世界坐标）
         * @param cameraSize The camera size in pixels 相机尺寸（像素）
         * @param zoom The camera zoom factor 相机缩放比例
         * @return SDL_FRect Viewport rectangle in world coordinates 视口矩形（世界坐标）
         */
        [[nodiscard]] static SDL_FRect GetViewportRect(const WorldVector2D& cameraPosition,
                                                       const ScreenVector2D& cameraSize,
                                                       float zoom);

        /**
         * World coordinates to screen coordinates
         * 世界坐标转屏幕坐标
         * @param cameraPosition The camera position in world coordinates 相机位置（世界坐标）
         * @param worldPosition The world position to convert 要转换的世界坐标
         * @param cameraSize The camera size in pixels 相机尺寸（像素）
         * @param zoom The camera zoom factor 相机缩放比例
         * @return ScreenVector2D Screen position in pixels 屏幕坐标（像素）
         */
        [[nodiscard]] static ScreenVector2D WorldToScreen(const WorldVector2D& cameraPosition,
                                                          const WorldVector2D& worldPosition,
                                                          const ScreenVector2D& cameraSize,
                                                          float zoom);

        /**
         * Screen coordinates to world coordinates
         * 屏幕坐标转世界坐标
         * @param cameraPosition The camera position in world coordinates 相机位置（世界坐标）
         * @param screenPosition The screen position in pixels 屏幕位置（像素）
         * @param cameraSize The camera size in pixels 相机尺寸（像素）
         * @param zoom The camera zoom factor 相机缩放比例
         * @return WorldVector2D World position 世界坐标
         */
        [[nodiscard]] static WorldVector2D ScreenToWorld(const WorldVector2D& cameraPosition,
                                                         const ScreenVector2D& screenPosition,
                                                         const ScreenVector2D& cameraSize,
                                                         float zoom);

        /**
         * Tile coordinates to world coordinates
         * 瓦片坐标转世界坐标
         * @param tilePos TileVector2D Tile coordinates 瓦片坐标
         * @return WorldVector2D World coordinates (top-left corner of the tile) 世界坐标（瓦片左上角）
         */
        [[nodiscard]] static WorldVector2D TileToWorld(const TileVector2D& tilePos);

        /**
         * World coordinates to tile coordinates
         * 世界坐标转瓦片坐标
         * @param worldPos WorldVector2D World coordinates 世界坐标
         * @return TileVector2D Tile coordinates 瓦片坐标
         */
        [[nodiscard]] static TileVector2D WorldToTile(const WorldVector2D& worldPos);

        /**
         * TileCoordinatesToChunkCoordinates
         * 瓦片坐标转区块顶点坐标
         * @param tileVector2d
         * @return
         */
        [[nodiscard]] static ChunkVertexVector2D TileToChunkVertex(
            const TileVector2D& tileVector2d);

        /**
         * Convert block vertex to tile coordinate
         * 区块顶点转瓦片坐标
         * @param chunkVertexVector2d
         * @return
         */
        [[nodiscard]] static TileVector2D ChunkVertexToTile(const ChunkVertexVector2D& chunkVertexVector2d);

        /**
         * TileCoordinatesToChunkRelativeCoordinates
         * 瓦片坐标转区块相对坐标
         * @param tileVector2d
         * @return
         */
        [[nodiscard]] static ChunkRelativeVector2D TileToChunkRelative(const TileVector2D& tileVector2d);


        /**
         * ChunkRelativeToTile
         * 区块内坐标转瓦片坐标
         * @param chunkVertex
         * @param relative
         * @return
         */
        [[nodiscard]] static TileVector2D ChunkRelativeToTile(const ChunkVertexVector2D& chunkVertex,
                                                              const ChunkRelativeVector2D& relative);


        /**
         * ChunkVertexToTerrainVertex
         * 区块顶点到地形顶点
         * @param chunkVertexVector2D
         * @return
         */
        [[nodiscard]] static TerrainVertexVector2D ChunkVertexToTerrainVertex(
            const ChunkVertexVector2D& chunkVertexVector2D);

        /**
         * TileToTerrainVertex
         * 瓦片坐标转地形顶点坐标
         * @param tileVector2D
         * @return
         */
        [[nodiscard]] static TerrainVertexVector2D TileToTerrainVertex(const TileVector2D& tileVector2D);

        /**
         * TileToTerrainRelative
         * 瓦片坐标转地形相对坐标
         * @param tileVector2D
         * @return
         */
        [[nodiscard]] static TerrainRelativeVector2D TileToTerrainRelative(const TileVector2D& tileVector2D);


        static size_t GetArrayIndex(int x, int y, int width);

        /**
         * TerrainRelativeToTile
         * 地形相对坐标转瓦片坐标
         * @param terrainVertex
         * @param relative
         * @return
         */
        [[nodiscard]] static TileVector2D TerrainRelativeToTile(const TerrainVertexVector2D& terrainVertex,
                                                                const TerrainRelativeVector2D& relative);
    };
}
