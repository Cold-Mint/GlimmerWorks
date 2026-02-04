//
// Created by coldmint on 2026/2/4.
//

#ifndef GLIMMERWORKS_TERRAINRESULT_H
#define GLIMMERWORKS_TERRAINRESULT_H
#include <unordered_map>

#include "TerrainTileResult.h"
#include "core/ecs/component/TileLayerComponent.h"

namespace glimmer {
    /**
     * TerrainResult
     * 地形生成结果
     */
    class TerrainResult {
        std::unordered_map<TileVector2D, TerrainTileResult, Vector2DIHash> terrainTileResult_;

    public:
        /**
         * AddTile
         * 添加瓦片
         * @param vector2d
         * @param result
         */
        void AddTile(TileVector2D vector2d, TerrainTileResult result);

        /**
         * GetResult
         * 获取结果
         * @return
         */
        [[nodiscard]] std::unordered_map<TileVector2D, TerrainTileResult, Vector2DIHash> GetResult() const;
    };
}


#endif //GLIMMERWORKS_TERRAINRESULT_H
