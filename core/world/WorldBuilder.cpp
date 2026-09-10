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
#include "WorldBuilder.h"

#include "WorldContext.h"
#include "Dimension.h"
#include "ChunkManager.h"
#include "TerrainManager.h"
#include "SystemScheduler.h"
#include "PlayerContext.h"
#include "box2d/box2d.h"
#include "core/config/Constants.h"
#include "core/context/AppContext.h"
#include "core/context/ModContext.h"
#include "core/ecs/component/AreaMarkerComponent.h"
#include "core/ecs/component/BlueprintComponent.h"
#include "core/ecs/component/ItemContainerComponent.h"
#include "core/ecs/component/ItemToolTipComponent.h"
#include "core/ecs/component/PauseComponent.h"
#include "core/ecs/component/RecipeSelectionComponent.h"
#include "core/ecs/component/TileLayerComponent.h"
#include "core/log/LogCat.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceRef.h"
#include "core/mod/dataPack/DimensionRegistry.h"
#include "core/saves/Saves.h"
#include "core/utils/StringUtils.h"
#include "generator/TileLayerType.h"

glimmer::WorldBuilder::WorldBuilder(WorldContext *worldContext) : worldContext_(worldContext) {
}

void glimmer::WorldBuilder::Build() {
    std::optional<MapManifestMessage> mapManifestOptional = worldContext_->saves_->ReadMapManifest();
    if (!mapManifestOptional.has_value()) {
        LogCat::w(std::source_location::current(), "world_builder_map_manifest_missing",
                  "Map manifest is missing, cannot build world");
        return;
    }
    std::optional<PlayerMessage> playerOptional = worldContext_->saves_->ReadLocalPlayer();
    if (!playerOptional.has_value()) {
        LogCat::w(std::source_location::current(), "world_builder_player_missing",
                  "Player data is missing, cannot build world");
        return;
    }
    PlayerMessage &playerMessage = playerOptional.value();
    worldContext_->playerManifest_ = std::make_unique<PlayerManifest>();
    worldContext_->playerManifest_->FromMessage(playerMessage);
    worldContext_->mapManifest_ = std::make_unique<MapManifest>();
    worldContext_->mapManifest_->FromMessage(mapManifestOptional.value());
    worldContext_->worldSeed_ = worldContext_->mapManifest_->seed;
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2(0.0F, -10.0F);
    worldContext_->worldId_ = b2CreateWorld(&worldDef);
    ModContext *modContext = worldContext_->appContext_->GetModContext();
    if (modContext == nullptr) {
        LogCat::w(std::source_location::current(), "world_builder_mod_context_null",
                  "Mod context is null, cannot build world");
        return;
    }
    BiomeDecoratorManager *biomeDecoratorManager = modContext->GetBiomeDecoratorManager();
    if (biomeDecoratorManager == nullptr) {
        return;
    }
    biomeDecoratorManager->SetWorldSeed(worldContext_->worldSeed_);
    worldContext_->entityManager_ = std::make_unique<EntityManager>();
    worldContext_->entityShortCut_ = std::make_unique<EntityShortCut>();
    worldContext_->entityManager_->SetEntityIndex(worldContext_->mapManifest_->entityIDIndex);
    DimensionRegistry *dimensionRegistry = modContext->GetDimensionRegistry();
    if (dimensionRegistry == nullptr) {
        return;
    }
    ResourceRef &customDimension = worldContext_->playerManifest_->customDimension;
    worldContext_->dimension_ = std::make_unique<Dimension>();
    DimensionResource *dimensionResource = dimensionRegistry->Find(customDimension.GetPackageId(),
                                                                   customDimension.GetResourceKey());
    if (dimensionResource == nullptr) {
        LogCat::w(std::source_location::current(), "world_builder_dimension_resource_null",
                  "Dimension resource is not found, cannot build world");
        return;
    }
    std::string dimensionFolderName = StringUtils::GetDimensionFolderName(
        customDimension.GetPackageId(),
        customDimension.GetResourceKey());
    worldContext_->dimension_->SetDimensionResource(dimensionResource);
    worldContext_->chunkLoader_ = std::make_unique<ChunkLoader>(worldContext_, worldContext_->saves_,
                                                                dimensionFolderName);
    worldContext_->chunkManager_ = std::make_unique<ChunkManager>(worldContext_, dimensionFolderName);
    worldContext_->chunkGenerator_ = std::make_unique<ChunkGenerator>(worldContext_, worldContext_->worldSeed_,
                                                                      dimensionResource);
    worldContext_->terrainManager_ = std::make_unique<TerrainManager>(worldContext_);
    worldContext_->tileInstancePool_ = std::make_unique<TileInstancePool>();
    worldContext_->fixedGlobalTick_ = worldContext_->mapManifest_->globalTickCount;
    auto *commandManager = worldContext_->appContext_->GetConsoleContext()->GetCommandManager();
    commandManager->BindWorldContext(worldContext_);
    commandManager->SetAllowCheats(true);
    auto pause = worldContext_->entityManager_->AddEntity();
    worldContext_->entityManager_->AddComponent<PauseComponent>(pause);

    auto recipeSelectionId = worldContext_->entityManager_->AddEntity();
    worldContext_->entityShortCut_->SetRecipeSelectionComponent(
        worldContext_->entityManager_->AddComponent<RecipeSelectionComponent>(recipeSelectionId));

    auto groundTileLayerEntity = worldContext_->entityManager_->AddEntity();
    worldContext_->entityManager_->AddComponent<
        TileLayerComponent>(groundTileLayerEntity, worldContext_, TileLayerType::Ground);
    worldContext_->entityShortCut_->SetAreaMarkerComponent(
        worldContext_->entityManager_->AddComponent<AreaMarkerComponent>(groundTileLayerEntity));
    worldContext_->entityShortCut_->SetBlueprintComponent(
        worldContext_->entityManager_->AddComponent<BlueprintComponent>(groundTileLayerEntity));
    auto backgroundTileLayerEntity = worldContext_->entityManager_->AddEntity();
    worldContext_->entityManager_->AddComponent<
        TileLayerComponent>(backgroundTileLayerEntity, worldContext_, TileLayerType::BackGround);
    worldContext_->playerContext_ = std::make_unique<PlayerContext>(worldContext_);
    LogCat::i("world_context_core_subsystems_created", "Core subsystems created: dimensions, PlayerContext");

    ResourceRef playerResourceRef{};
    playerResourceRef.ReadResource(
        *worldContext_->appContext_->GetModContext()->GetMobRegistry()->GetPlayerResourceList()[
            0],
        RESOURCE_MOB);
    worldContext_->playerContext_->InitPlayer(playerResourceRef);
    auto itemContainerPtr = worldContext_->entityManager_->
            GetComponent<ItemContainerComponent>(worldContext_->entityShortCut_->GetPlayer());
    worldContext_->entityShortCut_->SetItemContainerComponent(itemContainerPtr);
    worldContext_->entityShortCut_->SetItemToolTipComponent(
        worldContext_->entityManager_->AddComponent<ItemToolTipComponent>(
            worldContext_->entityManager_->AddEntity()));
    worldContext_->systemScheduler_ = std::make_unique<SystemScheduler>(worldContext_);
    worldContext_->systemScheduler_->InitSystem();
    LogCat::i("world_context_player_initialized", "Player initialized, SystemScheduler initialized");
    LogCat::i("world_context_created", "WorldContext created successfully");
}
