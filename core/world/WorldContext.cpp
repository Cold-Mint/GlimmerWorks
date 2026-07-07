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

#include <ranges>

#include "ChunkManager.h"
#include "TerrainManager.h"
#include "SystemScheduler.h"
#include "PlayerContext.h"
#include "box2d/box2d.h"
#include "core/Constants.h"
#include "core/ecs/component/AreaMarkerComponent.h"
#include "core/ecs/component/BlueprintComponent.h"
#include "core/ecs/component/ItemContainerComponent.h"
#include "core/ecs/component/ItemToolTipComponent.h"
#include "core/ecs/component/PauseComponent.h"
#include "core/ecs/component/TileLayerComponent.h"
#include "core/inventory/ItemContainer.h"
#include "core/log/LogCat.h"
#include "core/mod/ResourceRef.h"
#include "core/saves/Saves.h"
#include "core/scene/AppContext.h"
#include "core/utils/TimeUtils.h"
#include "generator/ChunkGenerator.h"
#include "generator/ChunkLoader.h"
#include "src/saves/entity_item.pb.h"


bool glimmer::WorldContext::IsDragMode() const
{
    return dragMode_;
}

void glimmer::WorldContext::SetDragMode(const bool dragMode)
{
    dragMode_ = dragMode;
}

glimmer::EntityManager* glimmer::WorldContext::GetEntityManager() const
{
    return entityManager_.get();
}

glimmer::EntityShortCut* glimmer::WorldContext::GetEntityShortCut() const
{
    return entityShortCut_.get();
}

bool glimmer::WorldContext::IsRuning() const
{
    return running;
}

void glimmer::WorldContext::SetRuning(const bool run)
{
    running = run;
}

glimmer::Saves* glimmer::WorldContext::GetSaves() const
{
    return saves_;
}

glimmer::MapManifest* glimmer::WorldContext::GetMapManifest() const
{
    return mapManifest_;
}

glimmer::ChunkGenerator* glimmer::WorldContext::GetChunkGenerator() const
{
    return chunkGenerator_.get();
}

glimmer::ChunkLoader* glimmer::WorldContext::GetChunkLoader() const
{
    return chunkLoader_.get();
}

glimmer::AppContext* glimmer::WorldContext::GetAppContext() const
{
    return appContext_;
}

b2WorldId glimmer::WorldContext::GetWorldId() const
{
    return worldId_;
}

int glimmer::WorldContext::GetWorldSeed() const
{
    return worldSeed_;
}

bool glimmer::WorldContext::IsEmptyEntityId(const uint32_t id)
{
    return id == GAME_ENTITY_ID_INVALID;
}


glimmer::ChunkManager* glimmer::WorldContext::GetChunkManager() const
{
    return chunkManager_.get();
}

glimmer::TerrainManager* glimmer::WorldContext::GetTerrainManager() const
{
    return terrainManager_.get();
}

glimmer::SystemScheduler* glimmer::WorldContext::GetSystemScheduler() const
{
    return systemScheduler_.get();
}

glimmer::PlayerContext* glimmer::WorldContext::GetPlayerContext() const
{
    return playerContext_.get();
}


void glimmer::WorldContext::SaveEntity(EntityItemMessage* entityItemMessage, const GameEntityID entityId) const
{
    entityItemMessage->mutable_gameentity()->set_id(entityId);
    const ResourceRef* resourceRef = entityManager_->GetResourceRef(entityId);
    if (resourceRef != nullptr)
    {
        resourceRef->WriteResourceRefMessage(*entityItemMessage->mutable_resourceref());
    }
    std::vector<GameComponent*> components = entityManager_->GetAllComponent(entityId);
    auto mutableComponents = entityItemMessage->mutable_components();
    for (auto& componentItem : components)
    {
        auto stringOptional = componentItem->Serialize();
        if (stringOptional.has_value())
        {
            ComponentMessage* componentMessage = mutableComponents->Add();
            componentMessage->set_type(componentItem->GetComponentType());
            componentMessage->set_data(stringOptional.value());
        }
    }
}

void glimmer::WorldContext::SaveGame()
{
    if (saving_)
    {
        return;
    }
    saving_ = true;
    const Saves* saves = GetSaves();
    if (saves == nullptr)
    {
        saving_ = false;
        return;
    }
    auto mapManifestMessageData = saves->ReadMapManifest();
    if (!mapManifestMessageData.has_value())
    {
        saving_ = false;
        return;
    }
    const long endTime = TimeUtils::GetCurrentTimeMs();
    mapManifestMessageData->set_totalplaytime(
        mapManifestMessageData->totalplaytime() + (endTime - startTime_));
    mapManifestMessageData->set_lastplayedtime(endTime);
    mapManifestMessageData->set_entityidindex(entityManager_->GetEntityIndex());
    if (!saves->WriteMapManifest(mapManifestMessageData.value()))
    {
        saving_ = false;
        return;
    }
    //Save the player.
    //保存玩家。
    auto player = entityShortCut_->GetPlayer();
    if (!IsEmptyEntityId(player) && entityManager_->IsPersistable(player))
    {
        PlayerMessage playerMessage;
        SaveEntity(playerMessage.mutable_entity(), player);
        (void)saves->WritePlayer(playerMessage);
    }

    //Save all blocks.
    //保存所有区块。
    auto allChunks = chunkManager_->GetAllChunks();
    for (const auto& pos : *allChunks | std::views::keys)
    {
        (void)chunkManager_->SaveChunk(pos);
    }
    saving_ = false;
}


glimmer::LightBuffer* glimmer::WorldContext::GetLightingBuffer() const
{
    return chunkManager_->GetLightingBuffer();
}

glimmer::TileInstancePool* glimmer::WorldContext::GetTileInstancePool() const
{
    return chunkManager_->GetTileInstancePool();
}


glimmer::WorldContext::WorldContext(AppContext* appContext, MapManifest* mapManifest, Saves* saves) : saves_(saves)
{
    worldSeed_ = mapManifest->seed;
    mapManifest_ = mapManifest;
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2(0.0F, -10.0F);
    worldId_ = b2CreateWorld(&worldDef);
    appContext_ = appContext;
    appContext->GetCommandManager()->BindWorldContext(this);
    appContext_->GetBiomeDecoratorManager()->SetWorldSeed(worldSeed_);
    entityManager_ = std::make_unique<EntityManager>();
    entityShortCut_ = std::make_unique<EntityShortCut>();
    entityManager_->SetEntityIndex(mapManifest_->entityIDIndex);
    chunkLoader_ = std::make_unique<ChunkLoader>(this, saves);
    chunkGenerator_ = std::make_unique<ChunkGenerator>(this, worldSeed_);
    startTime_ = TimeUtils::GetCurrentTimeMs();

    auto pause = entityManager_->AddEntity();
    entityManager_->AddComponent<PauseComponent>(pause);
    auto groundTileLayerEntity = entityManager_->AddEntity();
    entityManager_->AddComponent<
        TileLayerComponent>(groundTileLayerEntity, this, Ground);
    entityShortCut_->SetAreaMarkerComponent(entityManager_->AddComponent<AreaMarkerComponent>(groundTileLayerEntity));
    entityShortCut_->SetBlueprintComponent(entityManager_->AddComponent<BlueprintComponent>(groundTileLayerEntity));
    auto backgroundTileLayerEntity = entityManager_->AddEntity();
    entityManager_->AddComponent<
        TileLayerComponent>(backgroundTileLayerEntity, this, BackGround);
    chunkManager_ = std::make_unique<ChunkManager>(this);
    terrainManager_ = std::make_unique<TerrainManager>(this);
    systemScheduler_ = std::make_unique<SystemScheduler>(this);
    playerContext_ = std::make_unique<PlayerContext>(this);

    ResourceRef playerResourceRef{};
    playerResourceRef.ReadResource(*appContext->GetMobManager()->GetPlayerResourceList()[0],
                                   RESOURCE_MOB);
    playerContext_->InitPlayer(playerResourceRef);
    auto itemContainerPtr = entityManager_->
        GetComponent<ItemContainerComponent>(entityShortCut_->GetPlayer());
    entityShortCut_->SetItemContainerComponent(itemContainerPtr);
    ItemContainer* itemContainer = itemContainerPtr->GetItemContainer();
    if (itemContainer != nullptr)
    {
        playerContext_->InitHotbar(itemContainer);
        playerContext_->InitInventory(itemContainer);
    }
    entityShortCut_->SetItemToolTipComponent(
        entityManager_->AddComponent<ItemToolTipComponent>(entityManager_->AddEntity()));
    systemScheduler_->InitSystem();
    LogCat::i("Camera entity created with CameraComponent, WorldPositionComponent and PlayerControlComponent");
}

glimmer::WorldContext::~WorldContext()
{
    playerContext_.reset();
    systemScheduler_.reset();
    if (entityManager_)
    {
        entityManager_->Clear();
    }
    chunkManager_.reset();
    terrainManager_.reset();
    b2DestroyWorld(worldId_);
    worldId_ = b2_nullWorldId;
    if (appContext_)
    {
        appContext_->GetCommandManager()->UnbindWorldContext();
    }
}
