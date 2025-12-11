//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef RESOURCE_H
#define RESOURCE_H
#include <string>
#include <vector>

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
        std::string texture;
    };

    /**
     * Scope
     * 范围
     */
    struct Scope {
        float min;
        float max;

        /**
         * Whether the given value is within the range
         * 给定的数值是否在范围内
         * @param value value 数值
         * @return In Scope 是否在范围内
         */
        bool InScope(const float value) const {
            return value >= min && value <= max;
        }
    };

    /**
     * TilePlacerRef
     * 瓦片放置器引用
     */
    struct TilePlacerRef {
        std::string id;
        std::vector<std::string> tiles;
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

    /**
     * Condition
     * 条件
     */
    struct Condition {
        std::string condition;
        std::vector<std::string> resourceKeys;
    };


}


#endif //RESOURCE_H
