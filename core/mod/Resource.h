//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef RESOURCE_H
#define RESOURCE_H
#include <string>
#include <vector>

#include "ResourceRef.h"

namespace glimmer {
    struct TileRules;

    /**
     * Resource
     * 资源
     * All base classes that serialize objects from data packets.
     * 所有从数据包序列化对象的基类。
     */
    struct Resource {
        std::string packId;
        std::string key;
    };

    /**
     * StringResource
     * 字符串资源
     */
    struct StringResource : Resource {
        std::string value;
    };

    /**
     * TileResource
     * 瓦片资源
     */
    struct TileResource : Resource {
        ResourceRef name;
        ResourceRef description;
        std::string texture;
        float hardness = 1.0F;
        bool breakable = true;
        uint8_t physicsType = 0;
        uint8_t layerType = 0;
    };


    /**
     * TilePlacerRef
     * 瓦片放置器引用
     */
    struct TilePlacerRef {
        std::string id;
        std::vector<ResourceRef> tiles;
        std::string config;
    };


    /**
     * BiomeResource
     * 生物群系
     */
    struct BiomeResource : Resource {
        std::vector<TilePlacerRef> tilePlacerRefs;
        float humidity = 0.0F;
        float temperature = 0.0F;
        float weirdness = 0.0F;
        float erosion = 0.0F;
        float elevation = 0.0F;
    };
}


#endif //RESOURCE_H
