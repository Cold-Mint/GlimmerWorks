//
// Created by coldmint on 2026/2/4.
//

#ifndef GLIMMERWORKS_TERRAINRESULT_H
#define GLIMMERWORKS_TERRAINRESULT_H

#include "TerrainTileResult.h"
#include "core/ecs/component/TileLayerComponent.h"

namespace glimmer {
    /**
     * TerrainResult
     * 地形生成结果
     */
    class TerrainResult {
        std::array<std::array<TerrainTileResult, CHUNK_SIZE>, CHUNK_SIZE> terrainTileResult_;
        std::array<TerrainTileResult, CHUNK_SIZE> leftTerrainTileResult_;
        std::array<TerrainTileResult, CHUNK_SIZE> rightTerrainTileResult_;
        std::array<TerrainTileResult, CHUNK_SIZE> upTerrainTileResult_;
        std::array<TerrainTileResult, CHUNK_SIZE> downTerrainTileResult_;

    public:
        void SetTerrainTileResult(int x, int y, TerrainTileResult result);

        [[nodiscard]] const TerrainTileResult &QueryTerrain(int x, int y) const;

        /**
         * Set the terrain data of the last column on the left side of the block.
         * 设置左侧区块的最右侧的一列的地形数据
         * @param y
         * @param result
         */
        void SetLeftTerrainTileResult(int y, TerrainTileResult result);

        /**
         *Set the terrain data of the leftmost column of the right-side block
         *设置右侧区块的最左侧的一列的地形数据
         * @param y
         * @param result
         */
        void SetRightTerrainTileResult(int y, TerrainTileResult result);

        /**
         *Set the terrain data of the bottom row of the top block
         *设置顶部区块的最底部的一行的地形数据
         * @param x
         * @param result
         */
        void SetUpTerrainTileResult(int x, TerrainTileResult result);

        /***
         *Set the terrain data of the top row of the bottom block
         *设置底部区块的最顶部的一行的地形数据
         * @param x
         * @param result
         */
        void SetDownTerrainTileResult(int x, TerrainTileResult result);
    };
}


#endif //GLIMMERWORKS_TERRAINRESULT_H
