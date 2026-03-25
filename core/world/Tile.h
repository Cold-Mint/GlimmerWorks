//
// Created by Cold-Mint on 2025/11/2.
//

#ifndef GLIMMERWORKS_TILE_H
#define GLIMMERWORKS_TILE_H
#include <memory>

#include "core/mod/ResourceRef.h"
#include "core/scene/AppContext.h"
#include "generator/TileLayerType.h"
#include "generator/TilePhysicsType.h"
#include "SDL3/SDL_render.h"
#include "src/saves/tile.pb.h"


namespace glimmer {
    class Tile {
        ResourceRef tileRef_;
        std::string id_;
        std::string name_;
        std::optional<std::string> description_;
        std::shared_ptr<SDL_Texture> texture_;
        std::shared_ptr<MIX_Audio> breakSFX_;
        std::shared_ptr<MIX_Audio> blockPlaceSFX_;
        bool customLootTable_ = false;
        ResourceRef lootTable_;
        float hardness_ = 1.0F;
        bool breakable = true;
        bool allowChainMining_ = false;
        bool isPlayerPlaced_ = false;
        TilePhysicsType physicsType_ = TilePhysicsType::None;
        TileLayerType layerType = TileLayerType::Main;
        std::vector<std::string> contributors;

    public:
        [[nodiscard]] const ResourceRef &GetLootTableRef();

        [[nodiscard]] const ResourceRef &GetResourceRef();

        [[nodiscard]] bool IsCustomLootTable() const;

        [[nodiscard]] TilePhysicsType GetTilePhysicsType() const;

        [[nodiscard]] bool IsAllowChainMining() const;

        [[nodiscard]] const std::string &GetId() const;

        [[nodiscard]] SDL_Texture *GetTexture() const;

        [[nodiscard]] MIX_Audio *GetBreakSFX() const;

        [[nodiscard]] MIX_Audio *GetBlockPlaceSFX() const;

        void SetPlayerPlaced(bool playerPlaced);

        [[nodiscard]] bool IsPlayerPlaced() const;

        [[nodiscard]] const std::optional<std::string> &GetDescription() const;

        [[nodiscard]] const float &GetHardness() const;

        [[nodiscard]] bool IsBreakable() const;

        [[nodiscard]] const std::string &GetName() const;

        [[nodiscard]] TileLayerType GetLayerType() const;

        void ReadTileMessage(const TileMessage &tileMessage);

        void WriteTileMessage(TileMessage &tileMessage) const;

        /**
         * From Tile Resource
         * 从资源创建瓦片
         * @param appContext appContext 应用上下文
         * @param tileResource tileResource 瓦片资源
         * @param resourceRef resourceRef 瓦片引用
         * @return
         */
        static std::unique_ptr<Tile> FromTileResource(const AppContext *appContext, const TileResource *tileResource,
                                                      const ResourceRef &resourceRef);
    };
}


#endif //GLIMMERWORKS_TILE_H
