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
#include "core/mod/ResourceRef.h"
#include "core/scene/AppContext.h"
#include "generator/TileLayerType.h"
#include "generator/TilePhysicsType.h"
#include "SDL3/SDL_render.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "src/core/place_source_message.pb.h"


namespace glimmer
{
    enum class TileAnchorType : uint8_t;

    class Tile
    {
        friend class TileInstancePool;

        std::string id_;
        std::string name_;
        std::optional<std::string> description_;
        std::shared_ptr<TextureResourceResult> textureResult_ = nullptr;
        std::shared_ptr<TextureResourceResult> blueprintTextureResult_ = nullptr;
        std::shared_ptr<AudioResourceResult> breakSFXResult_ = nullptr;
        std::shared_ptr<AudioResourceResult> placeSFXResult_ = nullptr;
        std::vector<ItemTagResource> tags_;
        bool customLootTable_ = false;
        ResourceRef lootTable_;
        float hardness_ = 1.0F;
        bool lootScaleBySize_ = false;
        uint8_t technologyLevel_ = 0;
        uint8_t recipeGroup_ = 0;
        bool enableBlueprint_ = true;
        bool enableBlueprintMask_ = true;
        bool drawValidBlueprintColor_ = true;
        bool allowChainMining_ = false;
        uint8_t tileWidth_ = 1;
        uint8_t tileHeight_ = 1;
        bool allowDirAdjustAnchor_ = true;
        TileVector2D tileAnchor_ = TileVector2D(1, 1);
        /**
         * Can a certain tile be directly placed on top?
         * 是否可以将某个瓦片直接覆盖上去？
         */
        bool isOverwritable_ = false;
        /**
         * When being destroyed/overwritten, will debris be generated?
         * 被销毁/覆盖时 是否生成掉落物
         */
        bool canDropLoot_ = true;
        TilePhysicsType physicsType_ = TilePhysicsType::None;
        TileLayerType layerType_ = Ground;
        ResourceRef lightSource_;
        ResourceRef sideLightMask_;
        ResourceRef backLightMask_;
        bool autoDigCostScale_ = true;
        uint32_t unitDigCost_ = 1;

        /**
         * Calculate the anchor point coordinates of the tiles
         * 计算瓦片的锚点坐标
         * @param tileAnchorType tileAnchorType 瓦片锚点类型
         * @param tileWidth tileWidth 瓦片宽度
         * @param tileHeight tileHeight 瓦片高度
         * @param customTileAnchor customTileAnchor 自定义瓦片锚点
         * @return
         */
        static TileVector2D CalculateTileAnchor(TileAnchorType tileAnchorType, uint8_t tileWidth, uint8_t tileHeight,
                                                const Vector2DIResource& customTileAnchor);

        /**
       * From Tile Resource
       * 从资源创建瓦片
       * @param appContext appContext 应用上下文
       * @param tileResource tileResource 瓦片资源
       * @return
       */
        static std::unique_ptr<Tile> FromTileResource(const AppContext* appContext,
                                                      const TileResource* tileResource);

    public:
        [[nodiscard]] uint8_t GetTileWidth() const;

        void OnPlace(const WorldContext* worldContext, PlaceSourceMessage placeSource, const TileVector2D& position);

        void OnBreak(const WorldContext* worldContext, BreakSource breakSource, const TileVector2D& position);

        [[nodiscard]] const std::vector<ItemTagResource>& GetTags() const;

        [[nodiscard]] uint8_t GetTileHeight() const;

        [[nodiscard]] uint32_t GetUnitDigCost() const;

        [[nodiscard]] bool IsAutoDigCostScale() const;

        [[nodiscard]] const TileVector2D* GetTileAnchor() const;

        [[nodiscard]] const ResourceRef* GetLootTableRef() const;

        [[nodiscard]] const ResourceRef* GetLightSourceResource() const;

        [[nodiscard]] const ResourceRef* GetSideLightMaskResource() const;

        [[nodiscard]] const ResourceRef* GetBackLightMaskResource() const;

        [[nodiscard]] bool IsCustomLootTable() const;

        [[nodiscard]] TilePhysicsType GetTilePhysicsType() const;

        [[nodiscard]] bool IsAllowChainMining() const;

        [[nodiscard]] bool IsAllowDirAdjustAnchor() const;

        [[nodiscard]] bool LootScaleBySize() const;

        [[nodiscard]] const std::string& GetId() const;

        [[nodiscard]] SDL_Texture* GetTexture() const;

        [[nodiscard]] SDL_Texture* GetBlueprintTexture() const;

        [[nodiscard]] bool EnableBlueprint() const;

        [[nodiscard]] bool EnableBlueprintMask() const;

        [[nodiscard]] bool DrawValidBlueprintColor() const;

        [[nodiscard]] MIX_Audio* GetBreakSFX() const;

        [[nodiscard]] MIX_Audio* GetPlaceSFX() const;

        [[nodiscard]] const std::optional<std::string>& GetDescription() const;

        [[nodiscard]] const float& GetHardness() const;

        [[nodiscard]] bool IsBreakable() const;

        [[nodiscard]] bool IsOverwritable() const;

        [[nodiscard]] bool CanDropLoot() const;

        [[nodiscard]] bool IsWorkBlock() const;

        [[nodiscard]] const std::string& GetName() const;

        [[nodiscard]] TileLayerType GetLayerType() const;
    };
}
