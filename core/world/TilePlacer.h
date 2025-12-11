//
// Created by Cold-Mint on 2025/12/9.
//

#ifndef GLIMMERWORKS_TILEPLACER_H
#define GLIMMERWORKS_TILEPLACER_H
#include <string>
#include <vector>

#include "nlohmann/json_fwd.hpp"
#include "../ecs/component/TileLayerComponent.h"

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
         * @param appContext appContext 应用上下文
         * @param tileLayerComponent tileLayerComponent 瓦片图层组建
         * @param tileSet tileSet 可放置的瓦片
         * @param coordinateArray coordinateArray 坐标数组
         * @param json json 配置数据
         */
        [[nodiscard]] virtual bool Place(AppContext *appContext, TileLayerComponent *tileLayerComponent,
                                         std::vector<std::string> tileSet,
                                         std::vector<TileVector2D> coordinateArray,
                                         nlohmann::json json) = 0;

        /**
         * Get the id of the tile placement device
         * 获取瓦片放置器的Id
         * @return
         */
        virtual std::string GetId() = 0;
    };
}

#endif //GLIMMERWORKS_TILEPLACER_H
