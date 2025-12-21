//
// Created by Cold-Mint on 2025/12/9.
//

#ifndef GLIMMERWORKS_TILEPLACER_H
#define GLIMMERWORKS_TILEPLACER_H
#include <optional>
#include <string>
#include <vector>

#include "Chunk.h"
#include "nlohmann/json_fwd.hpp"
#include "../ecs/component/TileLayerComponent.h"
#include "../mod/ResourceRef.h"

namespace glimmer {
    class AppContext;
    /**
     * TilePlacer
     * 瓦片放置器
     */
    class TilePlacer {
    public:
        virtual ~TilePlacer();

        /**
         * Place
         * 放置瓦片
         * It is necessary to set the corresponding id for the tile within this method without loading the tile texture.
         * 需要在这个方法内为瓦片设置对应的id，而无须加载瓦片纹理。
         * @param appContext appContext 应用上下文
         * @param tilesRef chunkTilesRef 区块瓦片引用
         * @param tileSet tileSet 可放置的瓦片
         * @param coordinateArray coordinateArray 坐标数组
         * @param configData json 配置数据
         */
        [[nodiscard]] virtual bool PlaceTileId(AppContext *appContext,
                                               std::array<std::array<ResourceRef, CHUNK_SIZE>, CHUNK_SIZE> &tilesRef,
                                               std::vector<ResourceRef> &tileSet,
                                               std::vector<TileVector2D> &coordinateArray,
                                               std::optional<nlohmann::json> configData) = 0;

        /**
         * Get the id of the tile placement device
         * 获取瓦片放置器的Id
         * @return
         */
        virtual std::string GetId() = 0;
    };
}

#endif //GLIMMERWORKS_TILEPLACER_H
