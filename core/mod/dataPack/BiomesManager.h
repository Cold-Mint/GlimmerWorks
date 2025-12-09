//
// Created by Cold-Mint on 2025/12/8.
//

#ifndef GLIMMERWORKS_BIOMESMANAGER_H
#define GLIMMERWORKS_BIOMESMANAGER_H
#include <string>
#include <unordered_map>
#include "../Resource.h"
#include "../../ecs/component/TileLayerComponent.h"


namespace glimmer {
    struct BiomeResource;

    class BiomesManager {
    protected:
        std::unordered_map<std::string, std::unordered_map<std::string, BiomeResource> > biomeMap_{};
        std::vector<BiomeResource> biomeVector_{};

    public:
        void RegisterResource(BiomeResource &biomeResource);

        BiomeResource *Find(const std::string &packId, const std::string &key);


        /**
         * Find Best Biome
         * 查找最合的生物群系
         * @param humidity humidity 湿度
         * @param temperature temperature 温度
         * @param weirdness weirdness 奇异度
         * @param erosion erosion 侵蚀度
         * @param elevation elevation 海拔
         * @return
         */
        BiomeResource *FindBestBiome(float humidity, float temperature, float weirdness, float erosion,
                                     float elevation);

        /**
         * Obtain tile resources
         * 获取瓦片资源
         * @param tileVector2d coordinates 瓦片坐标
         * @param height Height percentage（占世界最大高度的百分百比0-1）
         * @param humidity humidity 湿度
         * @param temperature temperature 温度
         * @return
         */
        TileResource *GetTileResource(TileVector2D tileVector2d, float height, float humidity, float temperature);

        std::string ListTiles() const;
    };
}

#endif //GLIMMERWORKS_BIOMESMANAGER_H
