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
#include "WorldContext.h"

#include <vector>

#include "Dimension.h"
#include "ChunkManager.h"
#include "TerrainManager.h"
#include "SystemScheduler.h"
#include "PlayerContext.h"
#include "box2d/box2d.h"
#include "core/config/Constants.h"
#include "core/ecs/component/AreaMarkerComponent.h"
#include "core/ecs/component/BlueprintComponent.h"
#include "core/ecs/component/ItemContainerComponent.h"
#include "core/ecs/component/ItemToolTipComponent.h"
#include "core/ecs/component/PauseComponent.h"
#include "core/ecs/component/TileLayerComponent.h"
#include "core/log/LogCat.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceRef.h"
#include "core/mod/dataPack/DimensionRegistry.h"
#include "core/saves/Saves.h"
#include "core/utils/Box2DUtils.h"
#include "core/context/AppContext.h"
#include "core/utils/TimeUtils.h"
#include "generator/ChunkGenerator.h"
#include "generator/ChunkLoader.h"
#include "src/saves/entity_item.pb.h"


bool glimmer::WorldContext::IsDragMode() const {
    return dragMode_;
}

void glimmer::WorldContext::SetDragMode(const bool dragMode) {
    dragMode_ = dragMode;
}

glimmer::EntityManager *glimmer::WorldContext::GetEntityManager() const {
    if (entityManager_ == nullptr) {
        LogCat::w(std::source_location::current(), "entity_manager_is_null", "entityManager is nullptr");
        return nullptr;
    }
    return entityManager_.get();
}

glimmer::EntityShortCut *glimmer::WorldContext::GetEntityShortCut() const {
    return entityShortCut_.get();
}

bool glimmer::WorldContext::IsRuning() const {
    return running;
}

void glimmer::WorldContext::SetRuning(const bool run) {
    running = run;
}

glimmer::Saves *glimmer::WorldContext::GetSaves() const {
    if (saves_ == nullptr) {
        LogCat::w(std::source_location::current(), "saves_is_null", "saves is nullptr");
        return nullptr;
    }
    return saves_;
}

glimmer::MapManifest *glimmer::WorldContext::GetMapManifest() const {
    return mapManifest_.get();
}

glimmer::ChunkGenerator *glimmer::WorldContext::GetChunkGenerator() const {
    return chunkGenerator_.get();
}

glimmer::ChunkLoader *glimmer::WorldContext::GetChunkLoader() const {
    return chunkLoader_.get();
}

glimmer::AppContext *glimmer::WorldContext::GetAppContext() const {
    return appContext_;
}

b2WorldId glimmer::WorldContext::GetWorldId() const {
    return worldId_;
}

int glimmer::WorldContext::GetWorldSeed() const {
    return worldSeed_;
}

bool glimmer::WorldContext::IsEmptyEntityId(const uint32_t id) {
    return id == GAME_ENTITY_ID_INVALID;
}


glimmer::ChunkManager *glimmer::WorldContext::GetChunkManager() const {
    return chunkManager_.get();
}

glimmer::TerrainManager *glimmer::WorldContext::GetTerrainManager() const {
    return terrainManager_.get();
}

glimmer::SystemScheduler *glimmer::WorldContext::GetSystemScheduler() const {
    return systemScheduler_.get();
}

glimmer::PlayerContext *glimmer::WorldContext::GetPlayerContext() const {
    return playerContext_.get();
}


void glimmer::WorldContext::SaveEntity(EntityItemMessage *entityItemMessage, const GameEntityID entityId) const {
    LogCat::d("world_context_save_entity", "SaveEntity: entityId={}", entityId);
    entityItemMessage->mutable_gameentity()->set_id(entityId);
    const ResourceRef *resourceRef = entityManager_->GetResourceRef(entityId);
    if (resourceRef != nullptr) {
        resourceRef->WriteResourceRefMessage(*entityItemMessage->mutable_resourceref());
    }
    std::vector<GameComponent *> components = entityManager_->GetAllComponent(entityId);
    auto mutableComponents = entityItemMessage->mutable_components();
    for (auto &componentItem: components) {
        auto stringOptional = componentItem->Serialize();
        if (stringOptional.has_value()) {
            ComponentMessage *componentMessage = mutableComponents->Add();
            componentMessage->set_type(componentItem->GetComponentType());
            componentMessage->set_data(stringOptional.value());
        }
    }
    LogCat::d("world_context_save_entity_completed", "SaveEntity completed: entityId={}, components={}", entityId,
              components.size());
}

void glimmer::WorldContext::SaveGame() {
    if (saving_) {
        LogCat::w(std::source_location::current(), "world_context_save_in_progress",
                  "Save already in progress, ignoring");
        return;
    }
    LogCat::i("world_context_save_starting", "Starting game save: {}", mapManifest_->name);
    saving_ = true;
    const Saves *saves = GetSaves();
    if (saves == nullptr) {
        LogCat::e(std::source_location::current(), "saves_is_null", "saves is nullptr");
        saving_ = false;
        return;
    }
    auto mapManifestMessageData = saves->ReadMapManifest();
    if (!mapManifestMessageData.has_value()) {
        LogCat::w(std::source_location::current(), "world_context_read_map_manifest_failed",
                  "Failed to read map manifest");
        saving_ = false;
        return;
    }
    const long endTime = TimeUtils::GetCurrentTimeMs();
    mapManifestMessageData->set_globaltickcount(GetGlobalTick());
    mapManifestMessageData->set_entityidindex(entityManager_->GetEntityIndex());
    if (!saves->WriteMapManifest(mapManifestMessageData.value())) {
        LogCat::w(std::source_location::current(), "world_context_write_map_manifest_failed",
                  "Failed to write map manifest");
        saving_ = false;
        return;
    }
    auto player = entityShortCut_->GetPlayer();
    if (!IsEmptyEntityId(player) && entityManager_->IsPersistable(player)) {
        PlayerMessage playerMessage;
        playerMessage.set_lastplayedtime(endTime);

        SaveEntity(playerMessage.mutable_entity(), player);
        (void) saves->WriteLocalPlayer(playerMessage);
        LogCat::i("world_context_player_saved", "Player saved");
    } else {
        LogCat::d("world_context_player_save_skipped", "Player save skipped: isEmpty={}, persistable={}",
                  IsEmptyEntityId(player), entityManager_->IsPersistable(player));
    }
    saving_ = false;
}


glimmer::LightBuffer *glimmer::WorldContext::GetLightingBuffer() const {
    LightBuffer *result = chunkManager_->GetLightingBuffer();
    if (result == nullptr) {
        LogCat::w(std::source_location::current(), "light_buffer_is_null", "light buffer is null");
    }
    return result;
}

glimmer::TileInstancePool *glimmer::WorldContext::GetTileInstancePool() const {
    return tileInstancePool_.get();
}

glimmer::Dimension *glimmer::WorldContext::GetDimension() const {
    return dimension_.get();
}


glimmer::WorldContext::~WorldContext() {
    LogCat::i("world_context_destroying", "Destroying WorldContext: worldName={}",
              mapManifest_ ? mapManifest_->name : "unknown");
    playerContext_.reset();
    systemScheduler_.reset();
    LogCat::d("world_context_player_scheduler_released", "PlayerContext and SystemScheduler released");
    if (entityManager_) {
        entityManager_->Clear();
    }
    LogCat::d("world_context_entity_manager_cleared", "EntityManager cleared, dimensions released");
    b2DestroyWorld(worldId_);
    worldId_ = b2_nullWorldId;
    if (appContext_) {
        appContext_->GetConsoleContext()->GetCommandManager()->UnbindWorldContext();
    }
    LogCat::i("world_context_destroyed", "WorldContext destroyed");
}

void glimmer::WorldContext::OnTick(const uint64_t tick) {
    if (!initedTick_) {
        startTick_ = tick;
        initedTick_ = true;
    }
    lastTick_ = tick;
}

uint64_t glimmer::WorldContext::GetGlobalTick() const {
    return fixedGlobalTick_ + (lastTick_ - startTick_);
}

glimmer::WorldContext::WorldContext(AppContext *appContext, Saves *saves) : saves_(saves),
                                                                            appContext_(appContext) {
    std::optional<MapManifestMessage> mapManifestOptional = saves->ReadMapManifest();
    if (!mapManifestOptional.has_value()) {
        return;
    }
    std::optional<PlayerMessage> playerOptional = saves->ReadLocalPlayer();
    if (!playerOptional.has_value()) {
        return;
    }
    PlayerMessage &playerMessage = playerOptional.value();
    playerManifest_ = std::make_unique<PlayerManifest>();
    playerManifest_->FromMessage(playerMessage);
    mapManifest_ = std::make_unique<MapManifest>();
    mapManifest_->FromMessage(mapManifestOptional.value());
    worldSeed_ = mapManifest_->seed;
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2(0.0F, -10.0F);
    worldId_ = b2CreateWorld(&worldDef);
    ModContext *modContext = appContext_->GetModContext();
    if (modContext == nullptr) {
        return;
    }
    BiomeDecoratorManager *biomeDecoratorManager = modContext->GetBiomeDecoratorManager();
    if (biomeDecoratorManager == nullptr) {
        return;
    }
    biomeDecoratorManager->SetWorldSeed(worldSeed_);
    entityManager_ = std::make_unique<EntityManager>();
    entityShortCut_ = std::make_unique<EntityShortCut>();
    entityManager_->SetEntityIndex(mapManifest_->entityIDIndex);
    DimensionRegistry *dimensionRegistry = modContext->GetDimensionRegistry();
    if (dimensionRegistry == nullptr) {
        return;
    }
    ResourceRef &customDimension = playerManifest_->customDimension;
    dimension_ = std::make_unique<Dimension>();
    DimensionResource *dimensionResource = dimensionRegistry->Find(customDimension.GetPackageId(),
                                                                   customDimension.GetResourceKey());
    if (dimensionResource == nullptr) {
        return;
    }
    std::string dimensionFolderName = StringUtils::GetDimensionFolderName(
        customDimension.GetPackageId(),
        customDimension.GetResourceKey());
    dimension_->SetDimensionResource(dimensionResource);
    chunkLoader_ = std::make_unique<ChunkLoader>(this, saves, dimensionFolderName);
    chunkManager_ = std::make_unique<ChunkManager>(this, dimensionFolderName);
    chunkGenerator_ = std::make_unique<ChunkGenerator>(this, worldSeed_, dimensionResource);
    terrainManager_ = std::make_unique<TerrainManager>(this);
    tileInstancePool_ = std::make_unique<TileInstancePool>();
    fixedGlobalTick_ = mapManifest_->globalTickCount;
    auto *commandManager = appContext->GetConsoleContext()->GetCommandManager();
    commandManager->BindWorldContext(this);
    commandManager->SetAllowCheats(true);
    auto pause = entityManager_->AddEntity();
    entityManager_->AddComponent<PauseComponent>(pause);

    auto recipeSelectionId = entityManager_->AddEntity();
    entityShortCut_->SetRecipeSelectionComponent(
        entityManager_->AddComponent<RecipeSelectionComponent>(recipeSelectionId));

    auto groundTileLayerEntity = entityManager_->AddEntity();
    entityManager_->AddComponent<
        TileLayerComponent>(groundTileLayerEntity, this, TileLayerType::Ground);
    entityShortCut_->SetAreaMarkerComponent(entityManager_->AddComponent<AreaMarkerComponent>(groundTileLayerEntity));
    entityShortCut_->SetBlueprintComponent(entityManager_->AddComponent<BlueprintComponent>(groundTileLayerEntity));
    auto backgroundTileLayerEntity = entityManager_->AddEntity();
    entityManager_->AddComponent<
        TileLayerComponent>(backgroundTileLayerEntity, this, TileLayerType::BackGround);
    playerContext_ = std::make_unique<PlayerContext>(this);
    LogCat::i("world_context_core_subsystems_created", "Core subsystems created: dimensions, PlayerContext");

    ResourceRef playerResourceRef{};
    playerResourceRef.ReadResource(*appContext->GetModContext()->GetMobRegistry()->GetPlayerResourceList()[0],
                                   RESOURCE_MOB);
    playerContext_->InitPlayer(playerResourceRef);
    auto itemContainerPtr = entityManager_->
            GetComponent<ItemContainerComponent>(entityShortCut_->GetPlayer());
    entityShortCut_->SetItemContainerComponent(itemContainerPtr);
    entityShortCut_->SetItemToolTipComponent(
        entityManager_->AddComponent<ItemToolTipComponent>(entityManager_->AddEntity()));
    systemScheduler_ = std::make_unique<SystemScheduler>(this);
    systemScheduler_->InitSystem();
    LogCat::i("world_context_player_initialized", "Player initialized, SystemScheduler initialized");
    LogCat::i("world_context_created", "WorldContext created successfully");
}
