//
// Created by Cold-Mint on 2025/11/2.
//
#pragma once
#include <memory>

#include "core/mod/ResourceRef.h"
#include "core/scene/AppContext.h"
#include "generator/TileLayerType.h"
#include "generator/TilePhysicsType.h"
#include "SDL3/SDL_render.h"
#include "SDL3_mixer/SDL_mixer.h"


namespace glimmer
{
    class Tile
    {
        friend class TileInstancePool;

        std::string id_;
        std::string name_;
        std::optional<std::string> description_;
        std::shared_ptr<SDL_Texture> texture_;
        std::shared_ptr<SDL_Texture> blueprintTexture_;
        std::shared_ptr<MIX_Audio> breakSFX_;
        std::shared_ptr<MIX_Audio> placeSFX_;
        bool customLootTable_ = false;
        ResourceRef lootTable_;
        float hardness_ = 1.0F;
        bool lootScaleBySize_ = false;
        bool breakable = true;
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

        [[nodiscard]] uint8_t GetTileHeight() const;

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

        [[nodiscard]] const std::string& GetName() const;

        [[nodiscard]] TileLayerType GetLayerType() const;
    };
}
