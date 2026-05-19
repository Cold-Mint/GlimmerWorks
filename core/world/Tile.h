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
#include "src/saves/tile.pb.h"


namespace glimmer {
    class Tile {
        friend class TileInstancePool;

        ResourceRef tileRef_;
        std::string id_;
        std::string name_;
        std::optional<std::string> description_;
        std::shared_ptr<SDL_Texture> texture_;
        std::shared_ptr<MIX_Audio> breakSFX_;
        std::shared_ptr<MIX_Audio> placeSFX_;
        bool customLootTable_ = false;
        ResourceRef lootTable_;
        float hardness_ = 1.0F;
        bool breakable = true;
        bool allowChainMining_ = false;
        bool isPlayerPlaced_ = false;
        uint8_t tileWidth_ = 1;
        uint8_t tileHeight_ = 1;
        float colliderWidth_ = 1.0F;
        float colliderHeight_ = 1.0F;
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
       * From Tile Resource
       * 从资源创建瓦片
       * @param appContext appContext 应用上下文
       * @param tileResource tileResource 瓦片资源
       * @param resourceRef resourceRef 瓦片引用
       * @return
       */
        static std::unique_ptr<Tile> FromTileResource(const AppContext *appContext,
                                                      const TileResource *tileResource,
                                                      const ResourceRef &resourceRef);

    public:
        [[nodiscard]] uint8_t GetTileWidth() const;

        [[nodiscard]] uint8_t GetTileHeight() const;

        [[nodiscard]] const ResourceRef &GetLootTableRef();

        [[nodiscard]] const ResourceRef &GetResourceRef();

        [[nodiscard]] const ResourceRef &GetLightSourceResource();

        [[nodiscard]] const ResourceRef &GetSideLightMaskResource();

        [[nodiscard]] const ResourceRef &GetBackLightMaskResource();

        [[nodiscard]] bool IsCustomLootTable() const;

        [[nodiscard]] TilePhysicsType GetTilePhysicsType() const;

        [[nodiscard]] bool IsAllowChainMining() const;

        [[nodiscard]] const std::string &GetId() const;

        [[nodiscard]] SDL_Texture *GetTexture() const;

        [[nodiscard]] MIX_Audio *GetBreakSFX() const;

        [[nodiscard]] MIX_Audio *GetPlaceSFX() const;

        void SetPlayerPlaced(bool playerPlaced);

        [[nodiscard]] bool IsPlayerPlaced() const;

        [[nodiscard]] const std::optional<std::string> &GetDescription() const;

        [[nodiscard]] const float &GetHardness() const;

        [[nodiscard]] bool IsBreakable() const;

        [[nodiscard]] bool IsOverwritable() const;

        [[nodiscard]] bool CanDropLoot() const;

        [[nodiscard]] const std::string &GetName() const;

        [[nodiscard]] TileLayerType GetLayerType() const;

        void ReadTileMessage(const TileMessage &tileMessage);

        void WriteTileMessage(TileMessage &tileMessage) const;

        [[nodiscard]] float GetColliderWidth() const;

        [[nodiscard]] float GetColliderHeight() const;
    };
}
