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
#include "TerrainTileResult.h"

namespace glimmer {
    /**
     * TerrainResult
     * 地形生成结果
     */
    class TerrainResult {
        std::array<TerrainTileResult, CHUNK_AREA> terrainTileResult_;
        std::array<TerrainTileResult, CHUNK_SIZE> leftTerrainTileResult_;
        std::array<TerrainTileResult, CHUNK_SIZE> rightTerrainTileResult_;
        std::array<TerrainTileResult, CHUNK_SIZE> upTerrainTileResult_;
        std::array<TerrainTileResult, CHUNK_SIZE> downTerrainTileResult_;
        TileVector2D position_;

    public:
        void SetTerrainTileResult(int x, int y, const TerrainTileResult &result);


        void SetPosition(const TileVector2D &position);

        [[nodiscard]] TileVector2D GetPosition() const;

        [[nodiscard]] const TerrainTileResult &QueryTerrain(int x, int y) const;

        /**
         * Mark a certain coordinate as a structure.
         * 将某个坐标标记为结构。
         * @param tileIndex
         * @param structureResource
         */
        void SetTerrainTileStructure(
            int tileIndex,
            const ResourceRef *structureResource);

        /**
         * Set the terrain data of the last column on the left side of the block.
         * 设置左侧区块的最右侧的一列的地形数据
         * @param y
         * @param result
         */
        void SetLeftTerrainTileResult(int y, const TerrainTileResult &result);

        /**
         *Set the terrain data of the leftmost column of the right-side block
         *设置右侧区块的最左侧的一列的地形数据
         * @param y
         * @param result
         */
        void SetRightTerrainTileResult(int y, const TerrainTileResult &result);

        /**
         *Set the terrain data of the bottom row of the top block
         *设置顶部区块的最底部的一行的地形数据
         * @param x
         * @param result
         */
        void SetUpTerrainTileResult(int x, const TerrainTileResult &result);

        /***
         *Set the terrain data of the top row of the bottom block
         *设置底部区块的最顶部的一行的地形数据
         * @param x
         * @param result
         */
        void SetDownTerrainTileResult(int x, const TerrainTileResult &result);
    };
}
