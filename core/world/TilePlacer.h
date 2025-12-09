//
// Created by Cold-Mint on 2025/12/9.
//

#ifndef GLIMMERWORKS_TILEPLACER_H
#define GLIMMERWORKS_TILEPLACER_H
#include <string>
#include <vector>

#include "nlohmann/json_fwd.hpp"

namespace glimmer {
    /**
     * TilePlacer
     * 瓦片放置器
     */
    class TilePlacer {
    public:
        virtual ~TilePlacer() = default;

        /**
         *
         * @param tileSet 可放置的瓦片
         * @param json 配置数据
         */
        void Place(std::vector<std::string> tileSet, nlohmann::json json);

        /**
         * Get the id of the tile placement device
         * 获取瓦片放置器的Id
         * @return
         */
        virtual std::string GetId() = 0;
    };
}

#endif //GLIMMERWORKS_TILEPLACER_H
