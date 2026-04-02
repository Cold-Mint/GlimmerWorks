//@genCode
//
// Created by Cold-Mint on 2025/10/9.
//
#ifndef RESOURCE_H
#define RESOURCE_H
#include <string>
#include <vector>

#include "ResourceRef.h"
#include "core/Box2dFilter.h"
#include "core/lootTable/LootEntry.h"
#include "SDL3/SDL_pixels.h"
#include "src/saves/item.pb.h"

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
    };


    /**
     * Vector2DResource
     * 向量资源
     */
    //@genNextLine(Vector2DIResource|向量整数资源)
    struct Vector2DIResource {
        //@genNextLine(x|x)
        int x = 0;
        //@genNextLine(y|y)
        int y = 0;
    };

    //@genNextLine(Vector2DResource|向量资源)
    struct Vector2DResource {
        //@genNextLine(x|x)
        float x = 0.0F;
        //@genNextLine(y|y)
        float y = 0.0F;
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

    /**
     *ShapeResource
     * 形状资源
     */
    //@genNextLine(ShapeResource|形状资源)
    struct ShapeResource : Resource {
        //@genNextLine(shapeType|形状类型)
        uint8_t shapeType = 0;

        virtual ~ShapeResource() = default;
    };

    /**
     * Rectangular shape resource
     * 矩形形状资源
     */
    //@genNextLine(RectangularShapeResource|矩形形状资源)
    struct RectangleShapeResource : ShapeResource {
        //@genNextLine(width|宽度)
        float width = 1.0F;
        //@genNextLine(height|高度)
        float height = 1.0F;
    };


    /**
     * Circular resources
     * 圆形资源
     */
    //@genNextLine(CircularShapeResource|圆形资源)
    struct CircularShapeResource : ShapeResource {
        //@genNextLine(center|中心)
        Vector2DResource center;
        //@genNextLine(radius|半径)
        float radius = 1.0F;
    };

    /**
 * Rounded Rectangle Shape resources
 * 圆角矩形资源
 */
    //@genNextLine(RoundedRectangleShapeResource|圆角矩形资源)
    struct RoundedRectangleShapeResource : ShapeResource {
        //@genNextLine(width|宽度)
        float width = 1.0F;
        //@genNextLine(height|高度)
        float height = 1.0F;
        //@genNextLine(radius|半径)
        float radius = 0.0F;
    };


    /**
     * RayCastResource
     * 射线资源
     */
    //@genNextLine(RayCastResource|射线资源类)
    struct RayCastResource {
        //@genNextLine(origin|原点)
        Vector2DResource origin;
        //@genNextLine(translation|位移)
        Vector2DResource translation;
        //@genNextLine(filter|物理过滤)
        Box2dFilter filter;
    };


    /**
     * Prohibited area for placing tiles
     * 瓦片放置禁止区域
     */
    //@genNextLine(TilePlacementForbiddenZone|瓦片放置禁止区域)
    struct TilePlacementForbiddenZone {
        //@genNextLine(width|宽度)
        int width = 1;
        //@genNextLine(height|高度)
        int height = 1;
        //@genNextLine(offsetX|偏移X)
        float offsetX = 1;
        //@genNextLine(offsetY|偏移Y)
        float offsetY = 1;
    };

    /**
     * Mob Resource
     * 生物资源
     */
    //@genNextLine(MobResource|生物资源)
    struct MobResource : Resource {
        //@genNextLine(isPlayer|是否为玩家)
        bool isPlayer = false;
        //@genNextLine(movementAcceleration|移动加速度)
        float movementAcceleration = 6.0F;
        //@genNextLine(jumpForce|跳跃强度)
        float jumpForce = 7.5F;
        //@genNextLine(maxSpeed|最大速度)
        float maxSpeed = 18.0F;
        //@genNextLine(airControlFactor|空中移动衰减 如果设置为0,那么禁止空中左右移动，如果设置为1,那么不限制空中移动，如果设置为0.8那么会将加速度乘以0.8。)
        float airControlFactor = 1.0F;
        //@genNextLine(shape|碰撞形状)
        ResourceRef shape;
        //@genNextLine(bodyType|物理体类型)
        uint8_t bodyType;
        //@genNextLine(allowBodySleep|是否允许物理体休眠)
        bool allowBodySleep;
        //@genNextLine(box2dFilter|物理层过滤)
        Box2dFilter box2dFilter;
        //@genNextLine(fixedRotation|是否固定旋转)
        bool fixedRotation;
        //@genNextLine(friction|摩擦力)
        float friction = 0.0F;
        //@genNextLine(textureOffset|纹理偏移)
        Vector2DResource textureOffset;
        //@genNextLine(density|密度)
        float density = 0.001F;
        //@genNextLine(texture|纹理资源引用)
        ResourceRef texture = {};
        //@genNextLine(TilePlacementForbiddenZone|瓦片放置禁止区域)
        TilePlacementForbiddenZone tilePlacementForbiddenZone = {};
        //@genNextLine(groundCheckRayCast|地面检测射线)
        std::vector<RayCastResource> groundCheckRayCast = {};
    };


    enum VariableDefinitionType {
        INT,
        FLOAT,
        BOOL,
        STRING,
        REF
    };

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
    //             resourceRef.WriteResourceRefMessage(refMessage);
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

        void UpdateArgs(const std::string &selfPackId);
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
        //@genNextLine(position|位置)
        Vector2DIResource position;
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

    //@genNextLine(AbilityConfig|能力配置)
    struct AbilityConfig {
        //@genNextLine(canMineBlock|是否能挖掘方块)
        bool allowMineBlock = false;
        //@genNextLine(miningRange|挖掘范围)
        float miningRange = 5;
        //@genNextLine(Using the item is more likely to cause accidental dropping.|使用物品时多大概率触发手滑)
        float fumbleProbability = 0;
        //@genNextLine(chainMiningRadius|连锁采集半径)
        int chainMiningRadius = 0;
        //@genNextLine(precisionMining|是否精准采集)
        bool enablePrecisionMining = false;
        //@genNextLine(miningEfficiency|工具效率)
        float miningEfficiency = 0;

        void Reset() {
            allowMineBlock = false;
            miningRange = 5;
            fumbleProbability = 0;
            chainMiningRadius = 0;
            enablePrecisionMining = false;
            miningEfficiency = 0;
        }

        AbilityConfig &operator+=(const AbilityConfig &other) {
            this->allowMineBlock = this->allowMineBlock || other.allowMineBlock;
            this->enablePrecisionMining = this->enablePrecisionMining || other.enablePrecisionMining;
            this->miningRange += other.miningRange;
            this->fumbleProbability += other.fumbleProbability;
            this->chainMiningRadius += other.chainMiningRadius;
            this->miningEfficiency += other.miningEfficiency;
            return *this;
        }
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
        AbilityConfig abilityConfig = {};
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
        ResourceRef description = {};
        //@genNextLine(customLootTable|是否使用自定义战利品表)
        bool customLootTable = false;
        //@genNextLine(lootTable|战利品表资源引用)
        ResourceRef lootTable = {};
        //@genNextLine(texture|纹理资源引用)
        ResourceRef texture;
        //@genNextLine(breakSFX|瓦片被破坏时的音效)
        ResourceRef breakSfx;
        //@genNextLine(placeSFX|放置瓦片时的音效)
        ResourceRef placeSfx;
        //@genNextLine(hardness If the hardness is less than 0, then it is indestructible.|硬度 如果硬度小于0那么不可破坏。)
        float hardness = 1.0F;
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
        //@genNextLine(BGM|BGM)
        ResourceRef bgm;
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

        static std::vector<ItemMessage> GetLootItems(const LootResource *lootResource);
    };

    //@genNextLine(ItemMessageResource|物品消息资源)
    struct ItemMessageResource {
        //@genNextLine(item|物品)
        ResourceRef item;
        //@genNextLine(amount|数量)
        uint64_t amount = 0;
        //If it is a combinable item, then a list of capabilities needs to be set up.
        //如果是可组合物品，那么需要设置能力列表。
        //@genNextLine(abilityItemRef|能力物品)
        std::vector<ItemMessageResource> abilityItemRef = {};
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
        std::vector<ItemMessageResource> defaultAbilityList = {};
    };

    //@genNextLine(InitialInventoryResource|初始化库存资源)
    struct InitialInventoryResource : Resource {
        //@genNextLine(addItems|初始添加物品列表)
        std::vector<ItemMessageResource> addItems;
    };
}

#endif //RESOURCE_H
