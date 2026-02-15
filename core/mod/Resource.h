//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef RESOURCE_H
#define RESOURCE_H
#include <optional>
#include <string>
#include <vector>

#include "ResourceRef.h"
#include "core/lootTable/LootEntry.h"

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

        /**
         * Was not found
         * 是否未找到
         */
        bool missing = false;

        [[nodiscard]] static std::string GenerateId(const std::string &packId, const std::string &key);

        [[nodiscard]] static std::string GenerateId(const Resource &resource);

        [[nodiscard]] static std::optional<ResourceRef> ParseFromId(const std::string &id, int resourceType);
    };

    /**
     * StringResource
     * 字符串资源
     */
    struct StringResource : Resource {
        std::string value;
    };

    /**
     * ComposableItemResource
     * 可组合的物品资源
     */
    struct ComposableItemResource : Resource {
        ResourceRef name;
        ResourceRef description;
        ResourceRef texture;
        size_t slotSize;
        std::vector<ResourceRef> defaultAbilityList;
    };

    enum VariableDefinitionType:u_int8_t {
        INT = 0,
        FLOAT,
        BOOL,
        STRING
    };


    struct VariableDefinition {
        std::string key;
        VariableDefinitionType type = STRING;
        std::string value;


        [[nodiscard]] int AsInt() const;

        [[nodiscard]] float AsFloat() const;

        [[nodiscard]] bool AsBool() const;

        [[nodiscard]] std::string AsString() const;
    };

    struct VariableConfig {
        std::vector<VariableDefinition> definition;

        [[nodiscard]] const VariableDefinition *FindVariable(const std::string &name) const;
    };

    struct StructureResource : Resource {
        /**
        * Generator ID
         * 生成器Id
         */
        std::string generatorId;
        VariableConfig generatorConfig;
        /**
         * Only valid for static generators. Define the width of the structure.
         * 只对静态生成器有效，定义结构的宽度。
         */
        uint32_t width;
        std::vector<ResourceRef> data;
    };

    /**
     * AbilityItem
     * 能力物品
     */
    struct AbilityItemResource : Resource {
        ResourceRef name;
        ResourceRef description;
        ResourceRef texture;
        std::string ability;
        VariableConfig abilityConfig;
        bool canUseAlone = false;
    };

    /**
     * TileResource
     * 瓦片资源
     */
    struct TileResource : Resource {
        ResourceRef name;
        ResourceRef description;
        bool customLootTable;
        ResourceRef lootTable;
        ResourceRef texture;
        float hardness = 1.0F;
        bool breakable = true;
        uint8_t physicsType = 0;
        uint8_t layerType = 0;
    };


    /**
     * BiomeDecoratorResource
     * 生物群系装饰器
     */
    struct BiomeDecoratorResource {
        std::string id;
        std::vector<ResourceRef> data;
        VariableConfig config;
    };


    /**
     * BiomeResource
     * 生物群系
     */
    struct BiomeResource : Resource {
        std::vector<BiomeDecoratorResource> decorator;
        float humidity = 0.0F;
        float temperature = 0.0F;
        float weirdness = 0.0F;
        float erosion = 0.0F;
        float elevation = 0.0F;
    };


    struct LootResource : Resource {
        /**
         * mandatory
         * 必然掉落
         */
        std::vector<LootEntry> mandatory;

        /**
         * empty weight
         * 空白权重
         */
        uint32_t empty_weight;
        /**
         * Take out the pool several times
         * 抽取几次池子
         */
        uint32_t rolls = 1;
        /**
         * pool
         * 战利品池
         */
        std::vector<LootEntry> pool;

        static std::vector<ResourceRef> GetLootItems(const LootResource *lootResource);
    };
}


#endif //RESOURCE_H
