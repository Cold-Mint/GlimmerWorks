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
#include <vector>

#include "core/TilePlacementConfig.h"
#include "core/TileStateBackup.h"
#include "core/ecs/GameSystem.h"
#include "core/ecs/TileBreakParams.h"
#include "core/ecs/component/TileLayerComponent.h"
#include "core/mod/ResourceRef.h"


namespace glimmer
{
    class DiggingComponent;
    class CameraComponent;
    class Transform2DComponent;
    class TileVector2D;
    class Item;

    class DiggingSystem : public GameSystem
    {
        bool cacheTexture_ = false;
        std::vector<std::shared_ptr<TextureResourceResult>> textureResultList_ = {};
        DiggingComponent* diggingComponent_ = nullptr;
        CameraComponent* cameraComponent_ = nullptr;
        Transform2DComponent* cameraTransform2DComponent_ = nullptr;
        std::vector<const TileLayerComponent*> tileLayerComponents_;

        static bool CanProcessTile(const Tile* tile, bool isPlaceMode);

        static void SaveTileState(const TileStateMessage* tileState, TileStateBackup& backup);

        static void RestoreTileState(TileStateMessage* tileState, const TileStateBackup& backup);

        static bool TryPlaceTile(const TileLayerComponent* tileLayerComponent,
                                 TileStateMessage* tileState,
                                 const TileVector2D& currentVector,
                                 const TileVector2D& topLeftVector,
                                 const TilePlacementConfig& config,
                                 TileStateBackup& backup);

        static void ApplyItemDurability(Item* item, const Tile* tile, bool isCenter);

        static void DropDefaultLoot(WorldContext* worldContext,
                                    EntityManager* entityManager,
                                    const std::shared_ptr<Tile>& tile,
                                    const TileVector2D& position,
                                    const ResourceRef& oldResourceRef);

        static void DropTileLoot(WorldContext* worldContext,
                                 EntityManager* entityManager,
                                 const std::shared_ptr<Tile>& tile,
                                 const TileVector2D& position,
                                 const ResourceRef& oldResourceRef,
                                 bool precisionMining);

        static void DropCustomLoot(WorldContext* worldContext,
                                   EntityManager* entityManager,
                                   const AppContext* appContext,
                                   const LootResource* lootResource,
                                   const TileVector2D& topLeftVector);

        static void ProcessSingleTile(const TileBreakParams& params,
                                      const TileVector2D& currentVector,
                                      Item* item,
                                      bool isCenter,
                                      uint8_t& sum);

        void ProcessMiningComplete(const TileLayerComponent* tileLayer, TileLayerType tileLayerType);

        void RenderDiggingPoint(SDL_Renderer* renderer, const MiningRangeDataPoint* point, float zoom);

    public:
        static uint16_t BreakTile(const TileBreakParams& params);

        void OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) override;

        explicit DiggingSystem(WorldContext* worldContext);

        void Update(float delta) override;

        void Render(SDL_Renderer* renderer) override;

        uint8_t GetRenderOrder() override;

        [[nodiscard]] GameSystemType GetGameSystemType() const override;
    };
}
