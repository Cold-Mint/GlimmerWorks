/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#pragma once
//@genCode
#include <FastNoiseLite.h>
#include <memory>
#include <string>
#include <vector>

#include "ResourceRef.h"
#include "core/Box2dFilter.h"
#include "core/lootTable/LootEntry.h"
#include "core/math/Color.h"
#include "src/saves/item.pb.h"

namespace glimmer
{
    struct TileRules;

    /**
     * Resource
     * 资源
     * All base classes that serialize objects from data packets.
     * 所有从数据包序列化对象的基类。
     */
    //@include(toml11/find.hpp)
    //@genNextLine(Resource|资源类)
    struct Resource
    {
        std::string packId;

        //@genNextLine(resourceId|资源Id)
        std::string resourceId;

        /**
         * Was not found
         * 是否未找到
         */
        bool missing = false;

        [[nodiscard]] static std::string GenerateId(const std::string& packId, const std::string& key);

        [[nodiscard]] static std::string GenerateId(const Resource& resource);
    };


    //@genNextLine(LightSourceResource|光源资源)
    struct LightSourceResource : Resource
    {
        //@genNextLine(lightRadius The maximum value is 16, which is the length of the block.|光照半径 最大值为区块长度16)
        uint8_t lightRadius = 0;
        //@genNextLine(lightBrightestAtCenter If true, light is brightest at center; if false, light is brightest at edge.|亮度峰值在圆心 true=中心亮四周暗，false=中心暗四周亮)
        bool lightBrightestAtCenter = true;
        //@genNextLine(lightColor A: Indicates the intensity of light emission.|发光颜色 A表示发光强度。)
        ResourceRef lightColor = {};
    };

    //@genNextLine(LightMaskResource|光源遮照资源)
    struct LightMaskResource : Resource
    {
        //@genNextLine(lightMaskColor A 0 represents the complete blocking of light by the RGB control, resulting in no light transmission.|光线透射色彩 A 0代表完全不透光 RGB 控制光线的混合)
        ResourceRef lightMaskColor;
        //@genNextLine(tintFactor|染色系数 0为光照颜色，1为光源遮照颜色)
        float tintFactor = 0.0F;
    };

    //@genNextLine(FixedColorResource|固定颜色资源)
    struct FixedColorResource : Resource
    {
        //@genNextLine(a|透明度)
        uint8_t a = 255;
        //@genNextLine(r|红色通道值)
        uint8_t r = 0;
        //@genNextLine(g|绿色通道值)
        uint8_t g = 0;
        //@genNextLine(b|蓝色通道值)
        uint8_t b = 0;

        [[nodiscard]] Color ToColor() const;
    };

    /**
     * Vector2DResource
     * 向量资源
     */
    //@genNextLine(Vector2DIResource|向量整数资源)
    struct Vector2DIResource
    {
        //@genNextLine(x|x)
        int x = 0;
        //@genNextLine(y|y)
        int y = 0;
    };

    //@genNextLine(Vector2DResource|向量资源)
    struct Vector2DResource
    {
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
    struct StringResource : Resource
    {
        //@genNextLine(value|字符串值)
        std::string value;
    };

    /**
     *ShapeResource
     * 形状资源
     */
    //@genNextLine(ShapeResource|形状资源)
    struct IShapeResource : Resource
    {
        //@genNextLine(shapeType|形状类型)
        uint8_t shapeType = 0;

        virtual ~IShapeResource() = default;
    };

    /**
     * Rectangular shape resource
     * 矩形形状资源
     */
    //@genNextLine(RectangularShapeResource|矩形形状资源)
    struct RectangleShapeResource : IShapeResource
    {
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
    struct CircularShapeResource : IShapeResource
    {
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
    struct RoundedRectangleShapeResource : IShapeResource
    {
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
    struct RayCastResource
    {
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
    struct TilePlacementForbiddenZone
    {
        //@genNextLine(width|宽度)
        int width = 1;
        //@genNextLine(height|高度)
        int height = 1;
        //@genNextLine(offsetX|偏移X)
        int offsetX = 0;
        //@genNextLine(offsetY|偏移Y)
        int offsetY = 0;
    };

    /**
     * Mob Resource
     * 生物资源
     */
    //@genNextLine(MobResource|生物资源)
    struct MobResource : Resource
    {
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


    enum VariableDefinitionType
    {
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

    struct VariableDefinition
    {
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


        static VariableDefinitionType ResolveVariableType(const std::string& typeName);

        void AsResourceRef(ResourceRef& resourceRef) const;

        [[nodiscard]] int AsInt() const;

        [[nodiscard]] float AsFloat() const;

        [[nodiscard]] bool AsBool() const;

        [[nodiscard]] std::string AsString() const;
    };

    //@genNextLine(VariableConfig|变量配置)
    struct VariableConfig
    {
        //@genNextLine(definition|变量定义列表)
        std::vector<VariableDefinition> definition;

        [[nodiscard]] const VariableDefinition* FindVariable(const std::string& name) const;

        [[nodiscard]] VariableDefinition* FindVariableModifiable(const std::string& name);

        void UpdateArgs(const std::string& selfPackId);
    };

    //@genNextLine(StructurePlacementConditions|结构放置条件)
    struct StructurePlacementConditions
    {
        //@genNextLine(processorId|处理器ID)
        std::string processorId;
        //@genNextLine(config|变量配置)
        VariableConfig config = {};
    };

    //@genNextLine(TileInfo|瓦片信息)
    struct TileInfo
    {
        //@genNextLine(position|位置)
        Vector2DIResource position;
        //@genNextLine(tile|瓦片资源引用)
        ResourceRef tile;
        //@genNextLine(layerType|图层类型)
        uint8_t layerType = 0;
    };

    //@genNextLine(StructureResource|结构资源)
    struct IStructureResource : Resource
    {
        virtual ~IStructureResource() = default;

        //@genNextLine(generatorId|生成器ID)
        uint8_t generatorId = 0;
        //@genNextLine(condition|结构放置条件列表)
        std::vector<StructurePlacementConditions> condition = {};
        //@genNextLine(data|数据资源引用列表)
        std::vector<ResourceRef> data = {};
    };

    //@genNextLine(StaticStructureResource|静态结构资源)
    struct StaticStructureResource : IStructureResource
    {
        //@genNextLine(tileInfo|瓦片信息列表)
        std::vector<TileInfo> tileInfo = {};
    };

    //@genNextLine(TreeStructureResource|树结构资源)
    struct TreeStructureResource : IStructureResource
    {
        //@genNextLine(hasLeaves|是否拥有树叶)
        bool hasLeaves = false;
        //@genNextLine(leafDataIndex|树叶数据索引)
        uint8_t leafDataIndex = 0;
        //@genNextLine(trunkDataIndex|树干数据索引)
        uint8_t trunkDataIndex = 0;
        //@genNextLine(trunkHeightMax|树干最大高度)
        uint8_t trunkHeightMax = 9;
        //@genNextLine(trunkHeightMin|树干最小高度)
        uint8_t trunkHeightMin = 5;
        //@genNextLine(leafRadius|树叶半径)
        uint8_t leafRadius = 2;
        //@genNextLine(leafClusterCount|树叶簇数量)
        uint8_t leafClusterCount = 1;
        //@genNextLine(leafVerticalSpacing|树叶垂直间距)
        uint8_t leafVerticalSpacing = 0;
        //@genNextLine(trunkWidth|树干宽度)
        uint8_t trunkWidth = 1;
        //@genNextLine(trunkTileLayer|树干图层类型)
        uint8_t trunkTileLayer = 0;
        //@genNextLine(leafTileLayer|树叶图层类型)
        uint8_t leafTileLayer = 0;
    };

    //@genNextLine(AbilityConfig|能力配置)
    struct AbilityConfig
    {
        //@genNextLine(miningRange|挖掘范围)
        float miningRange = 5;
        //@genNextLine(Using the item is more likely to cause accidental dropping.|使用物品时多大概率触发手滑)
        float fumbleProbability = 0;
        //@genNextLine(chainMiningRadius|连锁采集半径)
        uint8_t chainMiningRadius = 0;
        //@genNextLine(precisionMining|是否精准采集)
        bool enablePrecisionMining = false;
        //@genNextLine(miningEfficiency|工具效率)
        float miningEfficiency = 0;
        //@genNextLine(mineAbleLayer|可挖掘的图层)
        uint8_t mineAbleLayer = 0;

        void Reset()
        {
            miningRange = 5;
            fumbleProbability = 0;
            chainMiningRadius = 0;
            enablePrecisionMining = false;
            miningEfficiency = 0;
            mineAbleLayer = 0;
        }

        AbilityConfig& operator+=(const AbilityConfig& other)
        {
            this->enablePrecisionMining = this->enablePrecisionMining || other.enablePrecisionMining;
            this->miningRange += other.miningRange;
            this->fumbleProbability += other.fumbleProbability;
            this->chainMiningRadius += other.chainMiningRadius;
            this->miningEfficiency += other.miningEfficiency;
            this->mineAbleLayer = this->mineAbleLayer | other.mineAbleLayer;
            return *this;
        }
    };

    //@genNextLine(ItemTagResource|物品标签)
    struct ItemTagResource
    {
    private:
        uint64_t cachedTagId_ = 0;

    public:
        //@genNextLine(name|标签名)
        std::string name;
        //@genNextLine(value|值)
        uint8_t value = 1;

        void MakeCachedTag();

        [[nodiscard]] uint64_t GetCachedTagId() const;
    };

    /**
     * AbilityItem
     * 能力物品
     */
    //@genNextLine(AbilityItemResource|能力物品资源)
    struct AbilityItemResource : Resource
    {
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
        //@genNextLine(maxDurability|最大耐久度)
        uint32_t maxDurability = 16;
        //@genNextLine(isUnbreakable|是否坚不可摧)
        bool isUnbreakable = false;
        //@genNextLine(tags|标签)
        std::vector<ItemTagResource> tags = {};
    };

    //@genNextLine(MaterialItemResource|材料物品资源)
    struct MaterialItemResource : Resource
    {
        //@genNextLine(name|名称资源引用)
        ResourceRef name;
        //@genNextLine(description|描述资源引用)
        ResourceRef description = {};
        //@genNextLine(texture|纹理资源引用)
        ResourceRef texture;
        //@genNextLine(tags|标签)
        std::vector<ItemTagResource> tags = {};
    };

    /**
     * ColorResource
     * 颜色资源
     */
    //@genNextLine(ColorResource|颜色资源)
    struct ColorResource
    {
        //@genNextLine(a|透明度)
        uint8_t a = 255;
        //@genNextLine(r|红色通道值)
        uint8_t r = 0;
        //@genNextLine(g|绿色通道值)
        uint8_t g = 0;
        //@genNextLine(b|蓝色通道值)
        uint8_t b = 0;

        [[nodiscard]] Color ToColor() const;
    };


    /**
     * TileResource
     * 瓦片资源
     */
    //@genNextLine(TileResource|瓦片资源)
    struct TileResource : Resource
    {
        //@genNextLine(name|名称资源引用)
        ResourceRef name;
        //@genNextLine(description|描述资源引用)
        ResourceRef description = {};
        //@genNextLine(tileWidth|瓦片宽度)
        uint8_t tileWidth = 1;
        //@genNextLine(tileHeight|瓦片高度)
        uint8_t tileHeight = 1;
        //@genNextLine(The technological level provided by the tiles|瓦片提供的科技等级)
        uint8_t technologyLevel = 0;
        //genNextLine(recipeGroup|配方组)
        uint8_t recipeGroup = 0;
        //@genNextLine(customLootTable|是否使用自定义战利品表)
        bool customLootTable = false;
        //@genNextLine(lootTable|战利品表资源引用)
        ResourceRef lootTable = {};
        //@genNextLine(texture|纹理资源引用)
        ResourceRef texture;
        //@genNextLine(enableBlueprint|启用蓝图-手持瓦片时显示蓝图)
        bool enableBlueprint = true;
        //@genNextLine(enableBlueprintMask|启用蓝图遮照-用颜色标记禁止放置的区域)
        bool enableBlueprintMask = true;
        //@genNextLine(drawValidBlueprintColor|绘制合法放置的蓝图颜色)
        bool drawValidBlueprintColor = false;
        //@genNextLine(blueprintTexture|蓝图纹理资源引用)
        ResourceRef blueprintTexture = {};
        //@genNextLine(breakSFX|瓦片被破坏时的音效)
        ResourceRef breakSfx;
        //@genNextLine(placeSFX|放置瓦片时的音效)
        ResourceRef placeSfx;
        //@genNextLine(Single-cell hardness: Sets the hardness provided by a single cell. If the hardness is less than 0, it is non-destructible.|单格硬度 设置单个格子提供的硬度。 如果硬度小于0那么不可破坏。)
        float unitHardness = 1.0F;
        //@genNextLine(Is the total hardness calculated based on the area?|是否按面积计算总硬度)
        bool autoHardnessScale = true;
        //@genNextLine(Does the loot quantity scale with tile area? (Only for giant tiles)|战利品数量是否按面积缩放（仅巨型瓦片生效）)
        bool lootScaleBySize = false;
        //genNextLine(Unit durability consumption|单位耐久度消耗)
        uint32_t unitDigCost = 1;
        //@genNextLine(Is durability calculated based on area?|是否按面积扣除耐久度)
        bool autoDigCostScale = true;
        //@genNextLine(physicsType|物理类型)
        uint8_t physicsType = 0;
        //@genNextLine(layerType|层级类型)
        uint8_t layerType = 0;
        //@genNextLine(allowChainMining|是否允许连锁挖掘)
        bool allowChainMining = false;
        //@genNextLine(lightSource|光源)
        ResourceRef lightSource;
        //@genNextLine(sideLightMask|侧面光源遮罩)
        ResourceRef sideLightMask;
        //@genNextLine(backLightMask|背面光源遮罩)
        ResourceRef backLightMask;
        //@genNextLine(Can a certain tile be directly placed on top|是否可以将某个瓦片直接覆盖上去)
        bool isOverwritable = false;
        //@genNextLine(When being destroyed/overwritten, will debris be generated|被销毁/覆盖时 是否生成掉落物)
        bool canDropLoot = true;
        //@genNextLine(The anchor point type of the tiles. When placing large tiles, the placement position is relative to the anchor point of the tile. The anchor point is based on the player's right side.|瓦片的锚点类型，当放置大型瓦片时放置位置相对于瓦片的锚点，锚点以玩家右侧为准)
        uint8_t tileAnchorType = 6;
        //@genNextLine(The anchor point coordinates of the tiles, when tileAnchorType is set to "Custom", can be defined; for other values, the engine will calculate them automatically.|瓦片的锚点坐标，tileAnchorType为Custom值时可定义，其他值为引擎自动计算。)
        Vector2DIResource customTileAnchor = {1, 1};
        //@genNextLine(Allow anchor adjustment by facing direction.|允许按朝向调整锚点，放置瓦片时，开启就会跟着左右朝向自动调换锚点，让瓦片顺着朝向方向延伸，关闭则固定原始锚点不动。)
        bool allowDirAdjustAnchor = true;
        //@genNextLine(tags|标签)
        std::vector<ItemTagResource> tags = {};
    };

    //@genNextLine(IBiomeDecoratorResource|生物群系装饰器接口)
    struct IBiomeDecoratorResource : Resource
    {
        ~IBiomeDecoratorResource() = default;

        //@genNextLine(biomeDecoratorType|装饰器类型)
        uint8_t biomeDecoratorType = 0;
        //@genNextLine(layerType|层级类型)
        uint8_t layerType = 0;
    };


    //@genNextLine(FillBiomeDecoratorResource|填充生物群系装饰器)
    struct FillBiomeDecoratorResource : IBiomeDecoratorResource
    {
        //@genNextLine(tile|瓦片)
        ResourceRef tile;
    };

    //@genNextLine(MineralBiomeDecoratorResource|矿脉生物群系装饰器)
    struct MineralBiomeDecoratorResource : IBiomeDecoratorResource
    {
    private:
        std::unique_ptr<FastNoiseLite> fastNoiseLite_ = nullptr;

    public:
        //@genNextLine(ore|矿石)
        ResourceRef ore;
        //@genNextLine(noiseType|噪声类型)
        uint8_t noiseType;
        //@genNextLine(frequency|频率)
        float frequency = 0.01F;
        //@genNextLine(oreSpawnMinNoiseThreshold|矿石生成最小噪声阈值)
        float oreSpawnMinNoiseThreshold = 0.5F;
        //@genNextLine(oreSpawnMaxNoiseThreshold|矿石生成最大噪声阈值)
        float oreSpawnMaxNoiseThreshold = 0.8F;
        //@genNextLine(invertOreSpawnByDepth|是否随深度增加矿石生成概率)
        bool invertOreSpawnByDepth = true;
        //The formation of ores will be based on the world seed plus the offset.
        //矿石的生成会按照世界种子加上偏移
        //@genNextLine(seedOffset|种子偏移量)
        int seedOffset = 1024;
        //@genNextLine(minSpawnElevation|矿石最小生成高度(地下深处))
        float minSpawnElevation = 0.0F;
        //@genNextLine(maxSpawnElevation|矿石最大生成高度(地表浅层))
        float maxSpawnElevation = 0.5F;

        FastNoiseLite* GetFastNoiseLite(int seed);
    };

    //@genNextLine(SurfaceBiomeDecoratorResource|表面生物群系装饰器)
    struct SurfaceBiomeDecoratorResource : IBiomeDecoratorResource
    {
        //@genNextLine(tile|瓦片)
        ResourceRef tile;
        //@genNextLine(allowAir|允许上方为空气)
        bool allowAir = true;
        //@genNextLine(allowWater|允许上方为液体)
        bool allowWater = false;
    };

    /**
     * BiomeResource
     * 生物群系
     */
    //@genNextLine(BiomeResource|生物群系)
    struct BiomeResource : Resource
    {
        //@genNextLine(decors|生物群系装饰器列表)
        std::vector<ResourceRef> decors;
        //@genNextLine(humidity|湿度)
        float humidity = 0.5F;
        //@genNextLine(temperature|温度)
        float temperature = 0.5F;
        //@genNextLine(weirdness|怪异度)
        float weirdness = 0.5F;
        //@genNextLine(erosion|侵蚀度)
        float erosion = 0.5F;
        //@genNextLine(elevation|海拔高度)
        float elevation = 0.5F;
        //@genNextLine(surfaceProximity|地表贴近度)
        float surfaceProximity = 0.5F;
        // Biome property strictness
        // Higher = stricter requirements; small differences result in poor matching
        // Lower = more lenient; large differences have little effect
        // Default 1.0 = standard strictness (matches original behavior without weighting)
        // 生物群系属性严格度
        // 越高 = 要求越严格，差一点就不匹配
        // 越低 = 要求越宽松，差很多也无所谓
        // 默认 1.0 = 标准严格度（和原来无权重逻辑一致）
        //@genNextLine(strictnessHumidity|湿度严格度)
        float strictnessHumidity = 1.0F;
        //@genNextLine(strictnessTemperature|温度严格度)
        float strictnessTemperature = 1.0F;
        //@genNextLine(strictnessWeirdness|怪异度严格度)
        float strictnessWeirdness = 1.0F;
        //@genNextLine(strictnessErosion|侵蚀度严格度)
        float strictnessErosion = 1.0F;
        //@genNextLine(strictnessElevation|海拔高度严格度)
        float strictnessElevation = 1.0F;
        //@genNextLine(surfaceProximity|地表贴近严格度)
        float strictnessSurfaceProximity = 1.0F;
        //@genNextLine(BGM|BGM)
        ResourceRef bgm;
        //@genNextLine(parallaxBackground|视差背景)
        ResourceRef parallaxBackground;
    };

    //@genNextLine(LootResource|战利品资源)
    struct LootResource : Resource
    {
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

        static std::vector<ItemMessage> GetLootItems(const LootResource* lootResource);
    };

    //@genNextLine(ItemMessageResource|物品消息资源)
    struct ItemMessageResource
    {
        //@genNextLine(item|物品)
        ResourceRef item;
        //@genNextLine(amount|数量)
        uint64_t amount = 1;
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
    struct ComposableItemResource : Resource
    {
        //@genNextLine(name|名称资源引用)
        ResourceRef name;
        //@genNextLine(description|描述资源引用)
        ResourceRef description = {};
        //@genNextLine(texture|纹理资源引用)
        ResourceRef texture;
        //@genNextLine(slotSize|槽位大小)
        size_t slotSize;
        //@genNextLine(maxDurability|最大耐久度)
        uint32_t maxDurability = 16;
        //@genNextLine(isUnbreakable|是否坚不可摧)
        bool isUnbreakable = false;
        //@genNextLine(defaultAbilityList|默认能力列表资源引用)
        std::vector<ItemMessageResource> defaultAbilityList = {};
        //@genNextLine(tags|标签)
        std::vector<ItemTagResource> tags = {};
    };

    //@genNextLine(InitialInventoryResource|初始化库存资源)
    struct InitialInventoryResource : Resource
    {
        //@genNextLine(addItems|初始添加物品列表)
        std::vector<ItemMessageResource> addItems;
    };

    //@genNextLine(RequiredTag|需要的标签)
    class RequiredTag
    {
        uint64_t cachedTagId_ = 0;

    public:
        //@genNextLine(requiredTag|需要的标签)
        std::string requiredTag;
        //@genNextLine(requiredWeight|需要的权重)
        uint16_t requiredWeight = 1;
        //@genNextLine(exactMatch The number of true labels must be equal to requiredWeight for the condition to be met; otherwise, it fails. False values are considered as passing if they are greater than or equal to the requiredWeight.|精准匹配 true标签数量必须等于requiredWeight才通过，false大于等于都通过。)
        bool exactMatch = true;

        void MakeCachedTag();

        [[nodiscard]] uint64_t GetCachedTagId() const;
    };


    //@genNextLine(RecipeResource|配方资源)
    struct RecipeResource : Resource
    {
        //@genNextLine(input|输入)
        std::vector<RequiredTag> input;
        //@genNextLine(output|输出)
        ItemMessageResource output;
        //@genNextLine(duration|执行时间)
        float duration = 0.0F;
        //@genNextLine(min Technology level: The recipe can only be unlocked when there are corresponding blocks of the same technology level nearby.|最低科技等级，当附近有对应科技等级的方块才解锁配方。)
        uint8_t minTechnologyLevel = 0;
        //@genNextLine(RecipeGroup|配方组)
        uint8_t recipeGroup = 0;
    };
}
