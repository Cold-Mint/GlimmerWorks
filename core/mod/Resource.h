//@genCode
//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef RESOURCE_H
#define RESOURCE_H
#include <optional>
#include <string>
#include <vector>

#include "ResourceRef.h"
#include "box2d/types.h"
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
    //@include(toml11/find.hpp)
    //@genNextLine(Resource|资源类)
    struct Resource {
        std::string packId;

        //@genNextLine(resourceId|资源Id)
        std::string resourceId;

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
    //@genNextLine(StringResource|字符串资源类)
    struct StringResource : Resource {
        //@genNextLine(value|字符串值)
        std::string value;
    };

    //@genNextLine(MobAppearanceResource|生物外观资源类)
    struct MobAppearanceResource {
        //@genNextLine(x|X坐标)
        float x = 0;
        //@genNextLine(y|Y坐标)
        float y = 0;
        //@genNextLine(texture|纹理资源引用)
        ResourceRef texture;
    };

    /**
     * Mob Resource
     * 生物资源
     */
    //@genNextLine(MobResource|生物资源类)
    struct MobResource : Resource {
        //@genNextLine(isPlayer|是否为玩家)
        bool isPlayer = false;
        //@genNextLine(moveSpeed|移动速度)
        int moveSpeed = 5;
        //@genNextLine(shape|碰撞形状)
        std::string shape;
        //@genNextLine(bodyType|物理体类型)
        uint8_t bodyType;
        //@genNextLine(allowBodySleep|是否允许物理体休眠)
        bool allowBodySleep;
        //@genNextLine(categoryBits|碰撞类别位)
        uint64_t categoryBits;
        //@genNextLine(maskBits|碰撞掩码位)
        uint64_t maskBits;
        //@genNextLine(fixedRotation|是否固定旋转)
        bool fixedRotation;
        //@genNextLine(friction|摩擦力)
        float friction = 0.0F;
        //@genNextLine(width|宽度)
        float width = 0;
        //@genNextLine(height|高度)
        float height = 0;
        //@genNextLine(appearance|生物外观列表)
        std::vector<MobAppearanceResource> appearance;
    };

    /**
     * ComposableItemResource
     * 可组合的物品资源
     */
    //@genNextLine(ComposableItemResource|可组合的物品资源类)
    struct ComposableItemResource : Resource {
        //@genNextLine(name|名称资源引用)
        ResourceRef name;
        //@genNextLine(description|描述资源引用)
        ResourceRef description = {};
        //@genNextLine(texture|纹理资源引用)
        ResourceRef texture;
        //@genNextLine(slotSize|槽位大小)
        size_t slotSize;
        //@genNextLine(defaultAbilityList|默认能力列表资源引用)
        std::vector<ResourceRef> defaultAbilityList = {};
    };

    enum VariableDefinitionType {
        INT,
        FLOAT,
        BOOL,
        STRING,
        REF
    };

    //他应该在下面而不是上面
    //@content(2)
    // template<>
    // struct from<glimmer::VariableDefinition> {
    //     static glimmer::VariableDefinition from_toml(const value &v) {
    //         glimmer::VariableDefinition r;
    //         r.key = toml::find<std::string>(v, "key");
    //         r.type = glimmer::VariableDefinition::ResolveVariableType(
    //             toml::find<std::string>(v, "type"));
    //         if (r.type == glimmer::VariableDefinitionType::INT) {
    //             r.value = std::to_string(toml::find<int>(v, "value"));
    //         } else if (r.type == glimmer::VariableDefinitionType::FLOAT) {
    //             r.value = std::to_string(toml::find<float>(v, "value"));
    //         } else if (r.type == glimmer::VariableDefinitionType::BOOL) {
    //             r.value = std::to_string(toml::find<bool>(v, "value"));
    //         } else if (r.type == glimmer::VariableDefinitionType::REF) {
    //             auto resourceRef = toml::find<glimmer::ResourceRef>(v, "value");
    //             ResourceRefMessage refMessage;
    //             resourceRef.ToMessage(refMessage);
    //             r.value = refMessage.SerializeAsString();
    //         } else {
    //             r.value = toml::find<std::string>(v, "value");
    //         }
    //         return r;
    //     }
    // };
    //@endContent

    struct VariableDefinition {
        std::string key;
        uint8_t type = 3;
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

    //@genNextLine(VariableConfig|变量配置)
    struct VariableConfig {
        //@genNextLine(definition|变量定义列表)
        std::vector<VariableDefinition> definition;

        [[nodiscard]] const VariableDefinition *FindVariable(const std::string &name) const;

        [[nodiscard]] VariableDefinition *FindVariableModifiable(const std::string &name);

        void UpdateArgs(const toml::spec &tomlVersion, const std::string &selfPackId);
    };

    //@genNextLine(StructurePlacementConditions|结构放置条件)
    struct StructurePlacementConditions {
        //@genNextLine(processorId|处理器ID)
        std::string processorId;
        //@genNextLine(config|变量配置)
        VariableConfig config = {};
    };

    //@genNextLine(TileInfo|瓦片信息)
    struct TileInfo {
        //@genNextLine(x|瓦片X坐标)
        int x;
        //@genNextLine(y|瓦片Y坐标)
        int y;
        //@genNextLine(tile|瓦片资源引用)
        ResourceRef tile;
    };

    //@genNextLine(StructureResource|结构资源)
    struct StructureResource : Resource {
        /**
        * Generator ID
         * 生成器Id
         */
        //@genNextLine(generatorId|生成器ID)
        std::string generatorId;
        //@genNextLine(generatorConfig|生成器变量配置)
        VariableConfig generatorConfig = {};
        //@genNextLine(condition|结构放置条件列表)
        std::vector<StructurePlacementConditions> condition = {};
        //@genNextLine(tileInfo|瓦片信息列表)
        std::vector<TileInfo> tileInfo = {};
        //@genNextLine(data|数据资源引用列表)
        std::vector<ResourceRef> data = {};
    };

    /**
     * AbilityItem
     * 能力物品
     */
    //@genNextLine(AbilityItemResource|能力物品资源)
    struct AbilityItemResource : Resource {
        //@genNextLine(name|名称资源引用)
        ResourceRef name;
        //@genNextLine(description|描述资源引用)
        ResourceRef description = {};
        //@genNextLine(texture|纹理资源引用)
        ResourceRef texture;
        //@genNextLine(ability|能力标识)
        std::string ability;
        //@genNextLine(abilityConfig|能力变量配置)
        VariableConfig abilityConfig = {};
        //@genNextLine(canUseAlone|是否可单独使用)
        bool canUseAlone = false;
    };

    /**
     * ColorResource
     * 颜色资源
     */
    //@genNextLine(ColorResource|颜色资源)
    struct ColorResource {
        //@genNextLine(a|透明度)
        uint8_t a = 255;
        //@genNextLine(r|红色通道值)
        uint8_t r = 0;
        //@genNextLine(g|绿色通道值)
        uint8_t g = 0;
        //@genNextLine(b|蓝色通道值)
        uint8_t b = 0;

        [[nodiscard]] SDL_Color ToSDLColor() const;
    };

    /**
     * TileResource
     * 瓦片资源
     */
    //@genNextLine(TileResource|瓦片资源)
    struct TileResource : Resource {
        //@genNextLine(name|名称资源引用)
        ResourceRef name;
        //@genNextLine(description|描述资源引用)
        ResourceRef description;
        //@genNextLine(customLootTable|是否使用自定义战利品表)
        bool customLootTable = false;
        //@genNextLine(lootTable|战利品表资源引用)
        ResourceRef lootTable = {};
        //@genNextLine(texture|纹理资源引用)
        ResourceRef texture;
        //@genNextLine(hardness|硬度)
        float hardness = 1.0F;
        //@genNextLine(breakable|是否可破坏)
        bool breakable = true;
        //@genNextLine(physicsType|物理类型)
        uint8_t physicsType = 0;
        //@genNextLine(layerType|层级类型)
        uint8_t layerType = 0;
        //@genNextLine(allowChainMining|是否允许连锁挖掘)
        bool allowChainMining = false;
    };

    /**
     * BiomeDecoratorResource
     * 生物群系装饰器
     */
    //@genNextLine(BiomeDecoratorResource|生物群系装饰器资源)
    struct BiomeDecoratorResource {
        //@genNextLine(id|装饰器ID)
        std::string id;
        //@genNextLine(data|数据资源引用列表)
        std::vector<ResourceRef> data;
        //@genNextLine(config|变量配置)
        VariableConfig config = {};
    };

    /**
     * BiomeResource
     * 生物群系
     */
    //@genNextLine(BiomeResource|生物群系)
    struct BiomeResource : Resource {
        //@genNextLine(decorator|生物群系装饰器列表)
        std::vector<BiomeDecoratorResource> decorator;
        //@genNextLine(humidity|湿度)
        float humidity = 0.0F;
        //@genNextLine(temperature|温度)
        float temperature = 0.0F;
        //@genNextLine(weirdness|怪异度)
        float weirdness = 0.0F;
        //@genNextLine(erosion|侵蚀度)
        float erosion = 0.0F;
        //@genNextLine(elevation|海拔高度)
        float elevation = 0.0F;
    };

    //@genNextLine(LootResource|战利品资源)
    struct LootResource : Resource {
        /**
         * mandatory
         * 必然掉落
         */
        //@genNextLine(mandatory|必然掉落列表)
        std::vector<LootEntry> mandatory;

        /**
         * empty weight
         * 空白权重
         */
        //@genNextLine(empty_weight|空白权重)
        uint32_t empty_weight = 0;
        /**
         * Take out the pool several times
         * 抽取几次池子
         */
        //@genNextLine(rolls|抽取次数)
        uint32_t rolls = 1;
        /**
         * pool
         * 战利品池
         */
        //@genNextLine(pool|战利品池列表)
        std::vector<LootEntry> pool = {};

        static std::vector<ResourceRef> GetLootItems(const LootResource *lootResource);
    };

    //@genNextLine(InitialInventoryResource|初始化库存资源)
    struct InitialInventoryResource : Resource {
        //@genNextLine(addItems|初始添加物品列表)
        std::vector<ResourceRef> addItems;
    };
}

#endif //RESOURCE_H
