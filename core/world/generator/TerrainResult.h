//
// Created by coldmint on 2026/2/4.
//

#ifndef GLIMMERWORKS_TERRAINRESULT_H
#define GLIMMERWORKS_TERRAINRESULT_H

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
         * @param index
         */
        void MarkStructure(int index);

        /**
         * Mark a certain position as the source of the structure
         * 将某个位置标记为结构源
         * @param index
         */
        void MarkStructureSource(int index);


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


#endif //GLIMMERWORKS_TERRAINRESULT_H
