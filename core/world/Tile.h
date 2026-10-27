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
#include <memory>

#include "BreakSource.h"
#include "TileBlueprintData.h"
#include "TileDimensions.h"
#include "TileLightResourceData.h"
#include "TileLootData.h"
#include "TileMiningData.h"
#include "TileResourceData.h"
#include "core/context/AppContext.h"
#include "core/ecs/EcsTypes.h"
#include "core/math/WorldAABB.h"
#include "generator/TileLayerType.h"
#include "src/core/place_source_message.pb.h"

class TileStateMessage;

namespace glimmer {
    enum class TileAnchorType : uint8_t;

    class Tile {
        friend class TileInstancePool;
        TileLightResourceData lightData_;
        TileResourceData resourceData_;
        TileLootData lootData_;
        ResourceRef growthTarget_;
        ResourceRef primaryActionTarget_;
        ResourceRef secondaryActionTarget_;

        std::unordered_map<Vector2DIFingerprint, GameEntityID> gameEntities_;
        std::optional<std::string> description_;
        std::string id_;
        std::string name_;
        TileDimensions dimensions_;
        TileBlueprintData blueprintData_;
        std::vector<ResourceRef> growthConditions_;
        TileMiningData miningData_;
        uint64_t growthMinTicks_ = 0;
        uint64_t growthMaxTicks_ = 0;
        //Offset from the crop origin to the spawn point used when it matures.
        //作物成熟时，生成点相对于原点的偏移。
        Vector2DIResource spawnOriginOffset_ = {0, 0};
        uint8_t technologyLevel_ = 0;
        uint8_t recipeGroup_ = 0;
        uint8_t collisionMask_ = COLLISION_MASK_SOLID;
        //碰撞箱尺寸（以瓦片为单位），默认 1x1 占满整格。
        Vector2DResource collisionBoxSize_ = {1.0F, 1.0F};
        //碰撞箱偏移（以瓦片为单位），碰撞箱左下角相对格子左下角的偏移。
        Vector2DResource collisionBoxOffset_ = {0.0F, 0.0F};
        //附着点掩码，0表示不启用附着。
        uint8_t attachmentMask_ = 0;
        TileLayerType layerType_ = TileLayerType::Ground;
        //Whether offline growth is allowed: when the chunk is reloaded, interpolate the elapsed time.
        //是否允许离线生长：区块重新加载时补算离线时长。
        bool allowOfflineGrowth_ = true;
        bool destroySelfAfterGrowth_ = false;
        bool isOverwritable_ = false;
        //附着点标签缓存ID列表，目标瓦片必须拥有其中任一标签才被视为有效附着点。
        std::vector<uint64_t> attachmentTagIds_;

        static TileVector2D CalculateTileAnchor(TileAnchorType tileAnchorType, uint8_t tileWidth, uint8_t tileHeight,
                                                const Vector2DIResource &customTileAnchor);

        static void AppendDerivedTag(std::unordered_set<uint64_t> &existingTagIds, std::vector<ItemTagResource> &tags,
                                     const std::string &name);

        static std::unique_ptr<Tile> FromTileResource(const AppContext *appContext,
                                                      const TileResource *tileResource);

    public:
        [[nodiscard]] uint8_t GetCollisionMask() const;

        /**
         * GetCollisionBoxSize
         * 获取碰撞箱尺寸（以瓦片为单位）。
         * @return
         */
        [[nodiscard]] const Vector2DResource &GetCollisionBoxSize() const;

        /**
         * GetCollisionBoxOffset
         * 获取碰撞箱偏移（以瓦片为单位，左下角相对格子左下角）。
         * @return
         */
        [[nodiscard]] const Vector2DResource &GetCollisionBoxOffset() const;

        /**
         * GetCollisionAABB
         * 获取该瓦片在指定格子坐标处的碰撞箱世界坐标 AABB（中心 + 半宽半高）。
         * @param tilePos tilePos 瓦片坐标
         * @return 碰撞箱 AABB
         */
        [[nodiscard]] WorldAABB GetCollisionAABB(const TileVector2D &tilePos) const;

        [[nodiscard]] const std::string &GetId() const;

        [[nodiscard]] bool IsOverwritable() const;

        [[nodiscard]] bool DestroySelfAfterGrowth() const;

        const ResourceRef &GetGrowthTarget() const;

        const ResourceRef &GetPrimaryActionTarget() const;

        const ResourceRef &GetSecondaryActionTarget() const;

        /**
         * GetSpawnOriginOffset
         * 获取作物成熟时生成点相对于原点的偏移。
         * @return
         */
        [[nodiscard]] const Vector2DIResource &GetSpawnOriginOffset() const;

        /**
         * AllowOfflineGrowth
         * 是否允许离线生长。
         * @return
         */
        [[nodiscard]] bool AllowOfflineGrowth() const;

        /**
         * IsWorkBlock
         * 是否为工作方块？
         * @return
         */
        [[nodiscard]] bool IsWorkBlock() const;

        /**
         * IsCropsBlock
         * 是否为作物方块？
         * @return
         */
        [[nodiscard]] bool IsCropsBlock() const;

        /**
         * Static，IsCropsBlock
         * 静态方法，判断是否为作物方块
         * @param growthMinTicks
         * @param growthTarget
         * @return
         */
        static bool IsCropsBlock(uint64_t growthMinTicks, const ResourceRef &growthTarget);

        [[nodiscard]] const std::string &GetName() const;

        [[nodiscard]] TileLayerType GetLayerType() const;

        [[nodiscard]] const std::vector<ResourceRef> &GetGrowthConditions() const;

        [[nodiscard]] const std::optional<std::string> &GetDescription() const;

        void OnPlace(const WorldContext *worldContext, PlaceSourceMessage placeSource, const TileVector2D &position,
                     const TileStateMessage *tileState);

        void OnBreak(const WorldContext *worldContext, BreakSource breakSource, const TileVector2D &position);

        [[nodiscard]] const TileResourceData &GetResourceData() const;

        [[nodiscard]] TileBlueprintData &GetMutableBlueprintData();

        [[nodiscard]] const TileBlueprintData &GetBlueprintData() const;

        [[nodiscard]] TileDimensions &GetMutableDimensions();

        [[nodiscard]] const TileDimensions &GetDimensions() const;

        [[nodiscard]] TileMiningData &GetMutableMiningData();

        [[nodiscard]] const TileMiningData &GetMiningData() const;

        [[nodiscard]] TileLootData &GetMutableLootData();

        [[nodiscard]] const TileLootData &GetLootData() const;

        [[nodiscard]] TileLightResourceData *GetMutableLightResourceData();

        [[nodiscard]] const TileLightResourceData *GetLightResourceData() const;

        [[nodiscard]] uint8_t GetAttachmentMask() const;

        [[nodiscard]] const std::vector<uint64_t> &GetAttachmentTagIds() const;
    };
}
