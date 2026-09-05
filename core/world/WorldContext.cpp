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

#include <cmath>
#include <ranges>
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
#include "core/ecs/component/RigidBody2DComponent.h"
#include "core/ecs/component/TileLayerComponent.h"
#include "core/log/LogCat.h"
#include "core/math/CoordinateTransformer.h"
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
        LogCat::w(std::source_location::current(), "entityManager is nullptr");
        return nullptr;
    }
    return entityManager_.get();
}

glimmer::EntityShortCut *glimmer::WorldContext::GetEntityShortCut() const {
    if (entityShortCut_ == nullptr) {
        LogCat::w(std::source_location::current(), "entityShortCut is nullptr");
        return nullptr;
    }
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
        LogCat::w(std::source_location::current(), "saves is nullptr");
        return nullptr;
    }
    return saves_;
}

glimmer::MapManifest *glimmer::WorldContext::GetMapManifest() const {
    return mapManifest_;
}

glimmer::Dimension *glimmer::WorldContext::GetOrCreateDimension(DimensionResource *dimensionResource) {
    if (dimensionResource == nullptr) {
        return nullptr;
    }
    const std::string id = Resource::GenerateId(dimensionResource->packId, dimensionResource->resourceId);
    if (const auto it = dimensions_.find(id); it != dimensions_.end()) {
        return it->second.get();
    }
    auto dimension = std::make_unique<Dimension>(this, dimensionResource);
    dimension->Init();
    Dimension *ptr = dimension.get();
    dimensions_[id] = std::move(dimension);
    return ptr;
}

glimmer::Dimension *glimmer::WorldContext::GetCurrentDimension() const {
    return currentDimension_;
}

std::string glimmer::WorldContext::GetCurrentDimensionId() const {
    if (currentDimension_ == nullptr) {
        return {};
    }
    return currentDimension_->GetDimensionId();
}

const std::vector<glimmer::LightKeyframe> &glimmer::WorldContext::GetAmbientLightKeyframes() const {
    if (currentDimension_ != nullptr && currentDimension_->GetDimensionResource() != nullptr) {
        return currentDimension_->GetDimensionResource()->ambientLightKeyframes;
    }
    return DimensionResource::GetDefaultAmbientLightKeyframes();
}

void glimmer::WorldContext::SwitchDimension(const ResourceRef &dimensionRef) {
    if (appContext_ == nullptr) {
        return;
    }
    DimensionRegistry *dimensionRegistry = appContext_->GetModContext()->GetDimensionRegistry();
    if (dimensionRegistry == nullptr) {
        return;
    }
    DimensionResource *resource = dimensionRegistry->Find(dimensionRef.GetPackageId(), dimensionRef.GetResourceKey());
    if (resource == nullptr) {
        LogCat::w(std::source_location::current(), "Dimension resource not found: ",
                  dimensionRef.GetPackageId(), ":", dimensionRef.GetResourceKey());
        return;
    }
    const std::string targetId = Resource::GenerateId(resource->packId, resource->resourceId);
    if (currentDimension_ != nullptr && currentDimension_->GetDimensionId() == targetId) {
        return;
    }
    if (currentDimension_ != nullptr) {
        currentDimension_->SaveTime();
        auto *chunks = currentDimension_->GetChunkManager()->GetAllChunks();
        std::vector<TileVector2D> positions;
        positions.reserve(chunks->size());
        for (const auto &entry: *chunks) {
            positions.push_back(entry.first);
        }
        for (const auto &position: positions) {
            currentDimension_->GetChunkManager()->UnloadChunkAt(position);
        }
    }
    currentDimension_ = GetOrCreateDimension(resource);
    if (currentDimension_ == nullptr) {
        return;
    }
    ResourceRef ref;
    ref.ReadResource(*resource, RESOURCE_DIMENSION);
    mapManifest_->currentDimension = ref;
    if (currentDimension_->GetChunkGenerator() != nullptr) {
        const int firstTileTerrainY = currentDimension_->GetChunkGenerator()->GetFirstTileTerrainY(0);
        const WorldVector2D spawn = CoordinateTransformer::TileToWorld(TileVector2D(0, firstTileTerrainY + 3));
        const GameEntityID player = entityShortCut_->GetPlayer();
        if (!IsEmptyEntityId(player)) {
            auto *rigidBody = entityManager_->GetComponent<RigidBody2DComponent>(player);
            if (rigidBody != nullptr && rigidBody->IsReady()) {
                const b2Vec2 newPos = Box2DUtils::ToMeters(spawn);
                const b2Rot currentRot = b2Body_GetRotation(rigidBody->GetBodyId());
                b2Body_SetTransform(rigidBody->GetBodyId(), newPos, currentRot);
            }
        }
    }
    LogCat::i("Switched to dimension: ", currentDimension_->GetDimensionId());
}

glimmer::ChunkGenerator *glimmer::WorldContext::GetChunkGenerator() const {
    return currentDimension_ != nullptr ? currentDimension_->GetChunkGenerator() : nullptr;
}

glimmer::ChunkLoader *glimmer::WorldContext::GetChunkLoader() const {
    return currentDimension_ != nullptr ? currentDimension_->GetChunkLoader() : nullptr;
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

float glimmer::WorldContext::GetTimeOfDay() const {
    return currentDimension_ != nullptr ? currentDimension_->GetTimeOfDay() : 0.0F;
}

void glimmer::WorldContext::SetTimeOfDay(const float time) {
    if (currentDimension_ != nullptr) {
        currentDimension_->SetTimeOfDay(time);
    }
}

void glimmer::WorldContext::AdvanceTime(const float delta) {
    if (!running) {
        return;
    }
    if (currentDimension_ != nullptr) {
        currentDimension_->AdvanceTime(delta, dayLengthSeconds_);
    }
}

float glimmer::WorldContext::GetDayLengthSeconds() const {
    return dayLengthSeconds_;
}

bool glimmer::WorldContext::IsEmptyEntityId(const uint32_t id) {
    return id == GAME_ENTITY_ID_INVALID;
}


glimmer::ChunkManager *glimmer::WorldContext::GetChunkManager() const {
    return currentDimension_ != nullptr ? currentDimension_->GetChunkManager() : nullptr;
}

glimmer::TerrainManager *glimmer::WorldContext::GetTerrainManager() const {
    return currentDimension_ != nullptr ? currentDimension_->GetTerrainManager() : nullptr;
}

glimmer::SystemScheduler *glimmer::WorldContext::GetSystemScheduler() const {
    if (systemScheduler_ == nullptr) {
        LogCat::w(std::source_location::current(), "systemScheduler is nullptr");
        return nullptr;
    }
    return systemScheduler_.get();
}

glimmer::PlayerContext *glimmer::WorldContext::GetPlayerContext() const {
    if (playerContext_ == nullptr) {
        LogCat::w(std::source_location::current(), "playerContext is nullptr");
        return nullptr;
    }
    return playerContext_.get();
}


void glimmer::WorldContext::SaveEntity(EntityItemMessage *entityItemMessage, const GameEntityID entityId) const {
    LogCat::d("SaveEntity: entityId=", entityId);
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
    LogCat::d("SaveEntity completed: entityId=", entityId, ", components=", components.size());
}

void glimmer::WorldContext::SaveGame() {
    if (saving_) {
        LogCat::w(std::source_location::current(), "Save already in progress, ignoring");
        return;
    }
    LogCat::i("Starting game save: ", mapManifest_->name);
    saving_ = true;
    const Saves *saves = GetSaves();
    if (saves == nullptr) {
        LogCat::e(std::source_location::current(), "saves is nullptr");
        saving_ = false;
        return;
    }
    auto mapManifestMessageData = saves->ReadMapManifest();
    if (!mapManifestMessageData.has_value()) {
        LogCat::w(std::source_location::current(), "Failed to read map manifest");
        saving_ = false;
        return;
    }
    const long endTime = TimeUtils::GetCurrentTimeMs();
    mapManifestMessageData->set_totalplaytime(
        mapManifestMessageData->totalplaytime() + (endTime - startTime_));
    mapManifestMessageData->set_lastplayedtime(endTime);
    mapManifestMessageData->set_entityidindex(entityManager_->GetEntityIndex());
    if (mapManifest_->currentDimension.IsValid()) {
        mapManifest_->currentDimension.WriteResourceRefMessage(
            *mapManifestMessageData->mutable_currentdimension());
    }
    if (!saves->WriteMapManifest(mapManifestMessageData.value())) {
        LogCat::w(std::source_location::current(), "Failed to write map manifest");
        saving_ = false;
        return;
    }
    auto player = entityShortCut_->GetPlayer();
    if (!IsEmptyEntityId(player) && entityManager_->IsPersistable(player)) {
        PlayerMessage playerMessage;
        SaveEntity(playerMessage.mutable_entity(), player);
        (void) saves->WritePlayer(playerMessage);
        LogCat::i("Player saved");
    } else {
        LogCat::d("Player save skipped: isEmpty=", IsEmptyEntityId(player), ", persistable=",
                  entityManager_->IsPersistable(player));
    }

    int chunkCount = 0;
    for (auto &[id, dimension]: dimensions_) {
        if (dimension == nullptr) {
            continue;
        }
        dimension->SaveTime();
        auto *allChunks = dimension->GetChunkManager()->GetAllChunks();
        for (const auto &pos: *allChunks | std::views::keys) {
            (void) dimension->GetChunkManager()->SaveChunk(pos);
            chunkCount++;
        }
    }
    LogCat::i("Game save completed, chunks saved: ", chunkCount);
    saving_ = false;
}


glimmer::LightBuffer *glimmer::WorldContext::GetLightingBuffer() const {
    return currentDimension_ != nullptr ? currentDimension_->GetChunkManager()->GetLightingBuffer() : nullptr;
}

glimmer::TileInstancePool *glimmer::WorldContext::GetTileInstancePool() const {
    return currentDimension_ != nullptr ? currentDimension_->GetChunkManager()->GetTileInstancePool() : nullptr;
}


glimmer::WorldContext::~WorldContext() {
    LogCat::i("Destroying WorldContext: worldName=", mapManifest_ ? mapManifest_->name : "unknown");
    playerContext_.reset();
    systemScheduler_.reset();
    LogCat::d("PlayerContext and SystemScheduler released");
    if (entityManager_) {
        entityManager_->Clear();
    }
    dimensions_.clear();
    currentDimension_ = nullptr;
    LogCat::d("EntityManager cleared, dimensions released");
    b2DestroyWorld(worldId_);
    worldId_ = b2_nullWorldId;
    if (appContext_) {
        appContext_->GetConsoleContext()->GetCommandManager()->UnbindWorldContext();
    }
    LogCat::i("WorldContext destroyed");
}

glimmer::WorldContext::WorldContext(AppContext *appContext, MapManifest *mapManifest,
                                    Saves *saves) : worldSeed_(mapManifest->seed), saves_(saves),
                                                    mapManifest_(mapManifest), appContext_(appContext) {
    LogCat::i("Creating WorldContext, world name: ", mapManifest->name, ", seed: ", worldSeed_);
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2(0.0F, -10.0F);
    worldId_ = b2CreateWorld(&worldDef);
    appContext_->GetModContext()->GetBiomeDecoratorManager()->SetWorldSeed(worldSeed_);
    entityManager_ = std::make_unique<EntityManager>();
    entityShortCut_ = std::make_unique<EntityShortCut>();
    entityManager_->SetEntityIndex(mapManifest_->entityIDIndex);

    DimensionRegistry *dimensionRegistry = appContext->GetModContext()->GetDimensionRegistry();
    DimensionResource *dimensionResource = nullptr;
    if (mapManifest_->currentDimension.IsValid()) {
        dimensionResource = dimensionRegistry->Find(mapManifest_->currentDimension.GetPackageId(),
                                                    mapManifest_->currentDimension.GetResourceKey());
    }
    if (dimensionResource == nullptr) {
        dimensionResource = dimensionRegistry->GetDefaultDimension();
    }
    if (dimensionResource != nullptr) {
        currentDimension_ = GetOrCreateDimension(dimensionResource);
        ResourceRef ref;
        ref.ReadResource(*dimensionResource, RESOURCE_DIMENSION);
        mapManifest_->currentDimension = ref;
    } else {
        LogCat::e(std::source_location::current(), "No dimension resource registered");
    }

    auto *commandManager = appContext->GetConsoleContext()->GetCommandManager();
    commandManager->BindWorldContext(this);
    commandManager->SetAllowCheats(mapManifest->allowCheats);
    if (const Config *config = appContext->GetConfig(); config != nullptr) {
        dayLengthSeconds_ = config->lighting.dayLengthSeconds;
    }
    startTime_ = TimeUtils::GetCurrentTimeMs();

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
    LogCat::i("Core subsystems created: dimensions, PlayerContext");

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
    LogCat::i("Player initialized, SystemScheduler initialized");
    LogCat::i("WorldContext created successfully");
}
