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
        std::shared_ptr<MIX_Audio> placeSFX_;
        bool customLootTable_ = false;
        ResourceRef lootTable_;
        float hardness_ = 1.0F;
        bool breakable = true;
        bool allowChainMining_ = false;
        bool isPlayerPlaced_ = false;
        TilePhysicsType physicsType_ = TilePhysicsType::None;
        TileLayerType layerType_ = Ground;
        /**
         * Is it allowed to disregard one's own Layer and place it onto any block layer?
         * 是否允许无视自身 Layer，放置到任意区块图层
         */
        bool allowCrossLayerPlacement_ = false;

        /**
         * lightTransmission
         * 透光性
         */
        SDL_Color lightTransmissionColor_ = {0, 0, 0, 255};

        /**
         * emissionColor
         * 发光颜色
         */
        SDL_Color emissionColor_ = {0, 0, 0, 0};

        /**
         * Lighting color (calculated result by Light2dSystem)
         * 光照颜色（由Light2dSystem计算完成后的结果）
         */
        SDL_Color lightColor_ = {0, 0, 0, 0};

        float emissionRadius_ = 0.0F;


    public:
        [[nodiscard]] const ResourceRef &GetLootTableRef();

        [[nodiscard]] const SDL_Color &GetLightTransmissionColor() const;

        [[nodiscard]] const ResourceRef &GetResourceRef();

        [[nodiscard]] bool IsCustomLootTable() const;

        [[nodiscard]] SDL_Color GetLightColor() const;

        [[nodiscard]] SDL_Color GetEmissionColor() const;

        [[nodiscard]] float GetEmissionRadius() const;

        void SetLightColor(SDL_Color lightColor);

        /**
         * SetLayerType
         * 设置图层
         * @param layerType TileLayerType 图层
         * @return 是否设置成功
         */
        [[nodiscard]] bool SetLayerType(TileLayerType layerType);

        [[nodiscard]] bool IsAllowCrossLayerPlacement() const;

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
