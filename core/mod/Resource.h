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
#include "SDL3/SDL_pixels.h"

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

    enum VariableDefinitionType {
        INT,
        FLOAT,
        BOOL,
        STRING,
        REF
    };


    struct VariableDefinition {
        std::string key;
        VariableDefinitionType type = STRING;
        std::string value;
        inline static const std::unordered_map<std::string, VariableDefinitionType> variableDefinitionTypeMap_{
            {"int", INT},
            {"float", FLOAT},
            {"bool", BOOL},
            {"string", STRING},
            {"ref", REF}
        };


        static VariableDefinitionType ResolveVariableType(const std::string &typeName);

        void AsResourceRef(ResourceRef &resourceRef) const;

        [[nodiscard]] int AsInt() const;

        [[nodiscard]] float AsFloat() const;

        [[nodiscard]] bool AsBool() const;

        [[nodiscard]] std::string AsString() const;
    };

    struct VariableConfig {
        std::vector<VariableDefinition> definition;

        [[nodiscard]] const VariableDefinition *FindVariable(const std::string &name) const;

        [[nodiscard]] VariableDefinition *FindVariableModifiable(const std::string &name);


        void UpdateArgs(const toml::spec &tomlVersion, const std::string &selfPackId);
    };

    struct StructurePlacementConditions {
        std::string processorId;
        VariableConfig config;
    };

    struct TileInfo {
        int x;
        int y;
        ResourceRef tile;
    };

    struct StructureResource : Resource {
        /**
        * Generator ID
         * 生成器Id
         */
        std::string generatorId;
        VariableConfig generatorConfig;
        std::vector<StructurePlacementConditions> condition;
        std::vector<TileInfo> tileInfo;
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
     * ColorResource
     * 颜色资源
     */
    struct ColorResource {
        uint8_t a = 255;
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;


        SDL_Color ToSDLColor() const;
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
        bool allowChainMining;
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


    struct InitialInventoryResource : Resource {
        std::vector<ResourceRef> addItems;
    };
}


#endif //RESOURCE_H
