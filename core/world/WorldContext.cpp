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

#include <utility>


#include "../Constants.h"
#include "../ecs/component/DebugDrawComponent.h"
#include "../ecs/component/PauseComponent.h"
#include "../ecs/system/CameraSystem.h"
#include "../ecs/system/ChunkSystem.h"
#include "../ecs/system/DebugDrawBox2dSystem.h"
#include "../ecs/system/DebugDrawSystem.h"
#include "../ecs/system/DebugPanelSystem.h"
#include "../ecs/system/AutoPickSystem.h"
#include "../ecs/system/DiggingSystem.h"
#include "../ecs/system/DroppedItemSystem.h"
#include "../ecs/system/HotBarSystem.h"
#include "../ecs/system/ItemSlotSystem.h"
#include "../ecs/system/MagnetSystem.h"
#include "../ecs/system/PauseSystem.h"
#include "../ecs/system/PhysicsSystem.h"
#include "../ecs/system/PlayerControlSystem.h"
#include "../ecs/system/TileLayerSystem.h"
#include "../log/LogCat.h"
#include "../saves/Saves.h"
#include "../utils/Box2DUtils.h"
#include "box2d/box2d.h"
#include "core/ecs/DroppedItemCreator.h"
#include "core/ecs/MobEntityCreator.h"
#include "core/ecs/component/DraggableComponent.h"
#include "core/ecs/component/DroppedItemComponent.h"
#include "core/ecs/component/GuiTransform2DComponent.h"
#include "core/ecs/component/HotBarComponent.h"
#include "core/ecs/system/AreaMarkerSystem.h"
#include "core/ecs/system/BiomeBGMSystem.h"
#include "core/ecs/system/BlueprintSystem.h"
#include "core/ecs/system/DebugMultiMapSystem.h"
#include "core/ecs/system/DraggableSystem.h"
#include "core/ecs/system/FloatingTextSystem.h"
#include "core/ecs/system/InventoryGUISystem.h"
#include "core/ecs/system/Light2DSystem.h"
#include "core/ecs/system/ParallaxBackgroundSystem.h"
#include "core/ecs/system/RayCast2DSystem.h"
#include "core/ecs/system/RecipeGUISystem.h"
#include "core/ecs/system/SpiritRendererSystem.h"
#include "core/math/CoordinateTransformer.h"
#include "core/utils/TimeUtils.h"
#include "generator/Chunk.h"
#include "generator/ChunkPhysicsHelper.h"
#ifdef __ANDROID__
#include "core/ecs/system/AndroidControlSystem.h"
#endif

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


void glimmer::WorldContext::OnChunkTileChange(Chunk* chunk, const std::shared_ptr<Tile>& tile, TileLayerType layerType,
                                              int index) const
{
    if (layerType == Ground)
    {
        ChunkPhysicsHelper::UpdatePhysicsBodyToChunk(this, chunk);
    }
    UpdateTileLight(chunk, layerType, index);
}


void glimmer::WorldContext::UpdateTileLight(const Chunk* chunk, const TileLayerType layerType, const int index) const
{
    if (appContext_ == nullptr)
    {
        return;
    }
#if  !defined(NDEBUG)
    const Config* config = appContext_->GetConfig();
    if (config == nullptr)
    {
        return;
    }
    if (!config->light.enable)
    {
        return;
    }
#endif
    if (chunk == nullptr)
    {
        return;
    }
    const Tile* tile = chunk->GetTile(layerType, index);
    if (tile == nullptr)
    {
        return;
    }
    const ResourceLocator* resourceLocator = appContext_->GetResourceLocator();
    if (resourceLocator == nullptr)
    {
        return;
    }
    const TileVector2D chunkPosition = chunk->GetPosition();
    const int localX = index & CHUNK_MASK;
    const int localY = index >> CHUNK_SHIFT;
    auto lightSourcePosition = TileVector2D(chunkPosition.x + localX, chunkPosition.y + localY);
    const LightMaskResource* sideLightMaskResource = resourceLocator->FindLightMask(tile->GetSideLightMaskResource());
    if (sideLightMaskResource != nullptr)
    {
        const std::unique_ptr<Color> sideLightMaskColorPtr = resourceLocator->FindColor(
            &sideLightMaskResource->lightMaskColor);
        if (sideLightMaskColorPtr == nullptr)
        {
            return;
        }
        if (sideLightMaskColorPtr->a == 0)
        {
            lightBuffer_->ClearSideLightMask(lightSourcePosition, tile->GetLayerType());
        }
        else
        {
            lightBuffer_->SetSideLightMask(lightSourcePosition, layerType,
                                           std::make_unique<LightMask>(sideLightMaskColorPtr.get(),
                                                                       sideLightMaskResource->tintFactor));
        }
    }
    const LightMaskResource* backLightMaskResource = resourceLocator->FindLightMask(tile->GetBackLightMaskResource());
    if (backLightMaskResource != nullptr)
    {
        const std::unique_ptr<Color> backLightMaskColorPtr = resourceLocator->FindColor(
            &backLightMaskResource->lightMaskColor);
        if (backLightMaskColorPtr == nullptr)
        {
            return;
        }
        if (backLightMaskColorPtr->a == 0)
        {
            lightBuffer_->ClearBackLightMask(lightSourcePosition, tile->GetLayerType());
        }
        else
        {
            lightBuffer_->SetBackLightMask(lightSourcePosition, layerType,
                                           std::make_unique<LightMask>(backLightMaskColorPtr.get(),
                                                                       backLightMaskResource->tintFactor));
        }
    }
    const LightSourceResource* lightSourceResource = resourceLocator->FindLightSource(
        tile->GetLightSourceResource());
    if (lightSourceResource != nullptr)
    {
        const std::unique_ptr<Color> lightColorPtr = resourceLocator->
            FindColor(&lightSourceResource->lightColor);
        if (lightColorPtr == nullptr)
        {
            return;
        }
        if (lightColorPtr->a == 0)
        {
            lightBuffer_->ClearLightSource(lightSourcePosition, layerType);
        }
        else
        {
            lightBuffer_->SetLightSource(lightSourcePosition, layerType,
                                         std::make_unique<LightSource>(
                                             lightSourcePosition, lightSourceResource->lightRadius,
                                             *lightColorPtr));
        }
    }
}


void glimmer::WorldContext::UpdateChunkLight(const Chunk* chunk) const
{
    for (int index = 0; index < CHUNK_AREA; ++index)
    {
        for (int i = 0; i < TILE_LAYER_TYPE_COUNT; ++i)
        {
            UpdateTileLight(chunk, static_cast<TileLayerType>(1 << i), index);
        }
    }
}

glimmer::WorldContext::~WorldContext()
{
    if (onComponentCountChangedId_ != 0 && entityManager_ != nullptr)
    {
        entityManager_->UnRegisterOnComponentCountChanged(onComponentCountChangedId_);
    }
    Config* config = appContext_->GetConfig();
    if (config != nullptr)
    {
        if (configChangedId_ != INVALID_CONFIG_CALL_BACK)
        {
            config->UnregisterOnConfigChanged(configChangedId_);
        }
    }

    if (entityShortCut_ != nullptr && entityManager_ != nullptr)
    {
        auto player = entityShortCut_->GetPlayer();
        if (!IsEmptyEntityId(player))
        {
            const ItemContainerComponent* itemContainerComponent = entityManager_->GetComponent<
                ItemContainerComponent>(player);
            if (itemCallback_ != nullptr && itemContainerComponent != nullptr)
            {
                ItemContainer* itemContainer = itemContainerComponent->GetItemContainer();
                if (itemContainer != nullptr)
                {
                    itemContainer->RemoveOnContentChanged(itemCallback_);
                }
            }
        }
    }
    entityManager_->Clear();
    activeSystems.clear();
    inactiveSystems.clear();
    chunks_.clear();
    terrainTileData_.clear();
    processedTerrainTiles_.clear();
    chunksCache_.clear();
    terrainTileDataCache_.clear();
    b2DestroyWorld(worldId_);
    worldId_ = b2_nullWorldId;
    appContext_->GetCommandManager()->UnbindWorldContext();
    ResourceRef ref;
    ref.SetSelfPackageId(RESOURCE_REF_CORE);
    ref.SetResourceType(RESOURCE_AUDIO);
    ref.SetResourceKey("sfx/item_break");
    itemBreakSFX_ = appContext_->GetResourceLocator()->FindAudio(&ref);
    audioManager_ = appContext_->GetAudioManager();
}

void glimmer::WorldContext::SetActiveGuiSystem(const GameSystemType systemType)
{
    activeGuiSystem_ = systemType;
}

glimmer::GameSystemType glimmer::WorldContext::GetActiveGuiSystemType() const
{
    return activeGuiSystem_;
}

glimmer::TerrainResult* glimmer::WorldContext::GetTerrainData(TileVector2D position)
{
    if (auto it = terrainTileData_.find(position); it != terrainTileData_.end())
    {
        return it->second.get();
    }
    return nullptr;
}

glimmer::TerrainResult* glimmer::WorldContext::GetOrCreateTerrainData(TileVector2D position)
{
    if (auto it = terrainTileData_.find(position); it != terrainTileData_.end())
    {
        return it->second.get();
    }

    auto terrainResult = chunkGenerator_->GenerateTerrain(position);
    if (terrainResult == nullptr)
    {
        return nullptr;
    }
    auto* terrainPtr = terrainResult.get();
    terrainTileData_.emplace(position, std::move(terrainResult));
    terrainTileDataCache_.emplace(position, terrainPtr);
    return terrainPtr;
}

bool glimmer::WorldContext::IsRuning() const
{
    return running;
}

std::vector<glimmer::GameSystemType> glimmer::WorldContext::GetAllActiveSystemType() const
{
    std::vector<GameSystemType> result;
    for (auto& activeSystem : activeSystems)
    {
        if (activeSystem == nullptr)
        {
            continue;
        }
        result.push_back(activeSystem->GetGameSystemType());
    }
    return result;
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

void glimmer::WorldContext::InitPlayer(const ResourceRef& resourceRef)
{
    if (entityManager_ == nullptr)
    {
        return;
    }
    if (entityShortCut_ == nullptr)
    {
        return;
    }
    GameEntityID player = entityShortCut_->GetPlayer();
    if (!IsEmptyEntityId(player))
    {
        return;
    }
    uint32_t playerEntity = GAME_ENTITY_ID_INVALID;
    if (saves_->PlayerExists())
    {
        auto playerMessage = saves_->ReadPlayer();
        if (playerMessage.has_value())
        {
            playerEntity = chunkLoader_->RecoveryEntity(playerMessage->entity());
        }
    }
    if (IsEmptyEntityId(playerEntity))
    {
        const auto firstTileTerrainY = chunkGenerator_->GetFirstTileTerrainY(0);
        playerEntity = entityManager_->AddEntity();
        MobEntityCreator mobEntityCreator{this};
        mobEntityCreator.LoadTemplateComponents(playerEntity, resourceRef);
        mobEntityCreator.MergeEntityItemMessage(playerEntity,
                                                MobEntityCreator::GetEntityItemMessage(
                                                    CoordinateTransformer::TileToWorld(
                                                        TileVector2D(0, firstTileTerrainY + 3))));
    }
    if (!entityManager_->HasComponent(playerEntity, COMPONENT_ITEM_CONTAINER))
    {
        const auto* itemContainerComponent = entityManager_->AddComponent<ItemContainerComponent>(
            playerEntity);
        if (itemContainerComponent != nullptr)
        {
            auto& allInitialInventory = appContext_->GetInitialInventoryManager()->GetAllInitialInventory();
            if (ItemContainer* itemContainer = itemContainerComponent->GetItemContainer(); itemContainer != nullptr)
            {
                itemContainer->Resize(HOT_BAR_SIZE * INVENTORY_ROW_COUNT);
                for (auto& initialInventory : allInitialInventory)
                {
                    for (auto& addItem : initialInventory->addItems)
                    {
                        auto item = appContext_->GetResourceLocator()->FindItem(this, addItem);
                        if (item == nullptr)
                        {
                            continue;
                        }
                        std::unique_ptr<Item> returnItem = itemContainer->AddItem(
                            std::move(item));
                        if (returnItem != nullptr)
                        {
                            const uint32_t droppedEntity = entityManager_->AddEntity();
                            DroppedItemCreator droppedItemCreator{this};
                            droppedItemCreator.LoadTemplateComponents(droppedEntity,
                                                                      DroppedItemCreator::GetResourceRef());
                            droppedItemCreator.MergeEntityItemMessage(droppedEntity,
                                                                      DroppedItemCreator::GetEntityItemMessage(
                                                                          entityManager_->GetComponent<
                                                                              Transform2DComponent>(
                                                                              playerEntity)->
                                                                          GetPosition(), std::move(returnItem), 2));
                        }
                    }
                }
            }
        }
    }
    auto itemContainerComponent = entityManager_->GetComponent<ItemContainerComponent>(playerEntity);
    if (itemContainerComponent != nullptr)
    {
        ItemContainer* itemContainer = itemContainerComponent->GetItemContainer();
        if (itemContainer != nullptr)
        {
            auto tempPlayerComponent = entityManager_->GetComponent<PlayerComponent>(playerEntity);
            if (tempPlayerComponent != nullptr)
            {
                tempPlayerComponent->item = itemContainer->GetItem(0);
            }
            itemCallback_ = itemContainer->AddOnContentChanged(
                [this, playerEntity](size_t index, Item* item, ContainerChangeType changeType)
                {
                    auto hotBarComponent = entityShortCut_->GetHotBarComponent();
                    if (hotBarComponent == nullptr)
                    {
                        return;
                    }
                    auto& slotEntityList = hotBarComponent->GetSlotEntities();
                    auto total = slotEntityList.size();
                    if (index >= total)
                    {
                        return;
                    }
                    auto ItemSlotEntityId = slotEntityList[index];
                    auto* itemSlot = entityManager_->GetComponent<ItemSlotComponent>(ItemSlotEntityId);
                    if (itemSlot == nullptr)
                    {
                        return;
                    }
                    if (!itemSlot->IsSelected())
                    {
                        return;
                    }
                    auto playerComponent = entityManager_->GetComponent<PlayerComponent>(playerEntity);
                    if (playerComponent == nullptr)
                    {
                        return;
                    }
                    if (item == nullptr)
                    {
                        playerComponent->item = nullptr;
                        return;
                    }
                    const size_t amount = item->GetAmount();
                    if (amount == 0)
                    {
                        playerComponent->item = nullptr;
                        return;
                    }
                    if (!item->IsUnbreakable() && item->GetRemaining() == 0)
                    {
                        audioManager_->TryPlayFree(AMBIENT, itemBreakSFX_.get(), 0);
                        auto composableItem = dynamic_cast<ComposableItem*>(item);
                        if (composableItem != nullptr)
                        {
                            ItemContainer* itemContainer = composableItem->GetItemContainer();
                            if (itemContainer != nullptr)
                            {
                                size_t size = itemContainer->GetCapacity();
                                for (size_t i = 0; i < size; i++)
                                {
                                    Item* abilityItem = itemContainer->GetItem(i);
                                    if (abilityItem != nullptr)
                                    {
                                        const size_t abilityRemaining = abilityItem->GetRemaining();
                                        if (abilityRemaining > 0)
                                        {
                                            std::unique_ptr<Item> takeItem = itemContainer->TakeItem(
                                                i, abilityItem->GetAmount());
                                            const uint32_t droppedEntity = entityManager_->AddEntity();
                                            DroppedItemCreator droppedItemCreator{this};
                                            droppedItemCreator.LoadTemplateComponents(droppedEntity,
                                                DroppedItemCreator::GetResourceRef());
                                            droppedItemCreator.MergeEntityItemMessage(droppedEntity,
                                                DroppedItemCreator::GetEntityItemMessage(
                                                    entityShortCut_->GetCameraTransform2DComponent()->
                                                                     GetPosition(), std::move(takeItem), 2));
                                        }
                                    }
                                }
                            }
                        }
                        playerComponent->item = nullptr;
                        return;
                    }
                    playerComponent->item = item;
                });
        }
    }
    entityShortCut_->SetPlayer(playerEntity);
}

void glimmer::WorldContext::InitHotbar(ItemContainer* itemContainer)
{
    if (entityManager_ == nullptr || entityShortCut_ == nullptr || itemContainer == nullptr)
    {
        return;
    }
    auto hotBar = entityManager_->AddEntity();
    auto* hotBarComponent = entityManager_->AddComponent<HotBarComponent>(hotBar, HOT_BAR_SIZE);
    if (hotBarComponent == nullptr)
    {
        LogCat::e("initHotbar Error.");
        return;
    }
    entityShortCut_->SetHotBarComponent(hotBarComponent);
    constexpr float slotStep = 0.002F + ITEM_SLOT_SIZE_NORMALIZED;
    for (int i = 0; i < HOT_BAR_SIZE; ++i)
    {
        const auto slotEntity = entityManager_->AddEntity();
        auto* itemSlotComponent = entityManager_->AddComponent<ItemSlotComponent>(slotEntity, itemContainer, i, true);
        if (itemSlotComponent == nullptr)
        {
            continue;
        }
        itemSlotComponent->SetSelected(i == 0);
        auto* guiTransform2DComponent = entityManager_->AddComponent<GuiTransform2DComponent>(slotEntity);
        if (guiTransform2DComponent == nullptr)
        {
            continue;
        }
        guiTransform2DComponent->SetSize(DesignVector2D(ITEM_SLOT_SIZE_NORMALIZED, ITEM_SLOT_SIZE_NORMALIZED));
        guiTransform2DComponent->SetPosition(DesignVector2D(
            slotStep * static_cast<float>(i),
            slotStep
        ));
        auto draggableComponent = entityManager_->AddComponent<DraggableComponent>(slotEntity);
        if (draggableComponent == nullptr)
        {
            continue;
        }
        draggableComponent->SetSize(guiTransform2DComponent->GetSize());
        hotBarComponent->AddSlotEntity(slotEntity);
    }
}

void glimmer::WorldContext::InitInventory(ItemContainer* itemContainer)
{
    if (entityManager_ == nullptr || entityShortCut_ == nullptr || itemContainer == nullptr)
    {
        return;
    }
    uint32_t capacity = itemContainer->GetCapacity();
    constexpr float slotStep = ITEM_SLOT_PADDING_NORMALIZED + 0.02F;

    for (uint32_t i = 0; i < capacity; ++i)
    {
        const auto slotEntity = entityManager_->AddEntity();
        auto* itemSlotComponent = entityManager_->AddComponent<ItemSlotComponent>(slotEntity, itemContainer, i, true);
        if (itemSlotComponent == nullptr)
        {
            continue;
        }
        auto* guiTransform2DComponent = entityManager_->AddComponent<GuiTransform2DComponent>(slotEntity);
        if (guiTransform2DComponent == nullptr)
        {
            continue;
        }
        guiTransform2DComponent->SetSize(DesignVector2D(ITEM_SLOT_SIZE_NORMALIZED, ITEM_SLOT_SIZE_NORMALIZED));
        int row = i / 9;
        int column = i % 9;
        guiTransform2DComponent->SetPosition(DesignVector2D(
            0.3 + ITEM_SLOT_PADDING_NORMALIZED + slotStep * static_cast<float>(column),
            0.2 + ITEM_SLOT_PADDING_NORMALIZED + slotStep * static_cast<float>(row)
        ));
        auto draggableComponent = entityManager_->AddComponent<DraggableComponent>(slotEntity);
        if (draggableComponent == nullptr)
        {
            continue;
        }
        draggableComponent->SetSize(guiTransform2DComponent->GetSize());
    }
}

void glimmer::WorldContext::OnWatchedComponentChanged(const GameComponentTypeMessage type, const uint32_t count)
{
    onComponentCountChangeBuffer_[type] = count;
}

std::unordered_map<glimmer::TileVector2D, glimmer::Chunk*, glimmer::Vector2DIHash>*
glimmer::WorldContext::GetAllChunks()
{
    if (lastChunkSnapshot_ != chunkSnapshot_)
    {
        chunksCache_.clear();
        chunksCache_.reserve(chunks_.size());

        for (auto& [pos, chunkPtr] : chunks_)
        {
            chunksCache_[pos] = chunkPtr.get();
        }
        lastChunkSnapshot_ = chunkSnapshot_;
    }
    return &chunksCache_;
}

std::unordered_map<glimmer::TileVector2D, glimmer::TerrainResult*, glimmer::Vector2DIHash>* glimmer::WorldContext::
GetTerrainResults()
{
    return &terrainTileDataCache_;
}

void glimmer::WorldContext::LoadTerrainAt(TileVector2D position)
{
    if (processedTerrainTiles_.contains(position))
    {
        return;
    }
    chunkGenerator_->GenerateStructure(position);
    processedTerrainTiles_.emplace(position);
}

void glimmer::WorldContext::UnloadTerrainAt(TileVector2D position)
{
    if (!processedTerrainTiles_.contains(position))
    {
        return;
    }
    processedTerrainTiles_.erase(position);
    terrainTileDataCache_.erase(position);
    terrainTileData_.erase(position);
}

void glimmer::WorldContext::LoadChunkAt(TileVector2D position)
{
    if (chunks_.contains(position))
    {
        return;
    }
    std::unique_ptr<Chunk> newlyCreatedChunk = chunkLoader_->LoadChunkFromSaves(position);
    if (newlyCreatedChunk == nullptr)
    {
        newlyCreatedChunk = chunkGenerator_->GenerateChunkAt(position);
    }
    if (newlyCreatedChunk == nullptr)
    {
        return;
    }
    UpdateChunkLight(newlyCreatedChunk.get());
    ChunkPhysicsHelper::AttachPhysicsBodyToChunk(appContext_, worldId_, newlyCreatedChunk.get());
    newlyCreatedChunk->AddReplaceTileCallback([this](Chunk* chunk, TileLayerType layerType,
                                                     int index,
                                                     std::shared_ptr<Tile>, const std::shared_ptr<Tile>& newTile)
    {
        OnChunkTileChange(chunk, newTile, layerType, index);
    });
    chunks_.insert({position, std::move(newlyCreatedChunk)});
    chunkSnapshot_++;
}

glimmer::ChunkGenerator* glimmer::WorldContext::GetChunkGenerator() const
{
    return chunkGenerator_.get();
}


void glimmer::WorldContext::UnloadChunkAt(TileVector2D position)
{
    auto it = chunks_.find(position);
    if (it == chunks_.end())
    {
        return;
    }
    if (SaveChunk(position))
    {
        //Uninstall the light sources and shadows of the current chunk.
        //卸载当前区块的光源和遮挡。
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            for (int y = 0; y < CHUNK_SIZE; y++)
            {
                lightBuffer_->ClearTileLightData(TileVector2D(position.x + x, position.y + y));
            }
        }
        ChunkPhysicsHelper::DetachPhysicsBodyToChunk(appContext_, it->second.get());
        chunks_.erase(it);
        chunkSnapshot_++;
    }
    else
    {
        LogCat::e("Saving the chunk failed, x=", position.x, ",y=", position.y);
    }
}

glimmer::Chunk* glimmer::WorldContext::GetChunk(const TileVector2D position)
{
#if  !defined(NDEBUG)
    const TileVector2D relativeVector = Chunk::TileCoordinatesToChunkRelativeCoordinates(position);
    if (relativeVector.x != 0 || relativeVector.y != 0)
    {
        LogCat::e("The coordinates are not the vertices of the chunk.");
        assert(false);
    }
#endif

    const auto it = chunks_.find(position);
    if (it == chunks_.end())
    {
        return nullptr;
    }
    return it->second.get();
}

glimmer::TileInstancePool* glimmer::WorldContext::GetTileInstancePool() const
{
    return tileInstancePool_.get();
}

bool glimmer::WorldContext::SaveChunk(TileVector2D position)
{
    const auto it = chunks_.find(position);
    if (it == chunks_.end())
    {
        return false;
    }
    ChunkMessage chunkMessage;
    Chunk* chunk = it->second.get();
    chunk->WriteChunkMessage(chunkMessage);
    (void)saves_->WriteChunk(position, chunkMessage);
    const WorldVector2D startWorldVector2d = chunk->GetStartWorldPosition();
    const WorldVector2D endWorldVector2d = chunk->GetEndWorldPosition();
    const float minX = std::min(startWorldVector2d.x, endWorldVector2d.x);
    const float maxX = std::max(startWorldVector2d.x, endWorldVector2d.x);
    const float minY = std::min(startWorldVector2d.y, endWorldVector2d.y);
    const float maxY = std::max(startWorldVector2d.y, endWorldVector2d.y);
    auto transform2DEntities = entityManager_->GetEntityIDWithComponents({COMPONENT_TRANSFORM_2D});
    ChunkEntityMessage chunkEntityMessage;
    std::vector<uint32_t> entitiesToRemove;
    GameEntityID player = entityShortCut_->GetPlayer();
    for (auto& transform2dEntity : transform2DEntities)
    {
        if (transform2dEntity == player)
        {
            continue;
        }
        auto* transform2dComponent = entityManager_->GetComponent<Transform2DComponent>(transform2dEntity);
        if (transform2dComponent == nullptr)
        {
            continue;
        }
        WorldVector2D pos = transform2dComponent->GetPosition();
        if (pos.x < minX || pos.x >= maxX ||
            pos.y < minY || pos.y >= maxY)
        {
            continue;
        }
        if (entityManager_->IsPersistable(transform2dEntity))
        {
            SaveEntity(chunkEntityMessage.add_entities(), transform2dEntity);
        }
        //Whether this entity is successfully saved or not, it will disappear due to the block unloading.
        //无论这个实体是否成功保存，它都会因为区块卸载而消失。
        entitiesToRemove.push_back(transform2dEntity);
    }
    if (chunkEntityMessage.entities_size() > 0)
    {
        //Create a file and save it
        //创建文件并保存
        (void)saves_->WriteChunkEntity(position, chunkEntityMessage);
    }
    else
    {
        (void)saves_->DeleteChunkEntity(position);
    }
    for (auto id : entitiesToRemove)
    {
        entityManager_->RemoveEntity(id);
    }
    return true;
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

bool glimmer::WorldContext::HasChunk(const TileVector2D position) const
{
    return chunks_.contains(position);
}

bool glimmer::WorldContext::ChunkIsOutOfBounds(const TileVector2D position)
{
    return position.y >= WORLD_MAX_Y || position.y < WORLD_MIN_Y || position.x >= WORLD_MAX_X || position.x <
        WORLD_MIN_X;
}


bool glimmer::WorldContext::HandleEvent(const SDL_Event& event) const
{
    bool handled = false;
    for (auto& system : activeSystems)
    {
        if (system == nullptr)
        {
            continue;
        }
        if (!running && !system->CanRunWhilePaused())
        {
            continue;
        }
        if (system->HandleEvent(event))
        {
            handled = true;
        }
    }
    return handled;
}

void glimmer::WorldContext::Update(const float delta) const
{
    for (auto& system : activeSystems)
    {
        if (system == nullptr)
        {
            continue;
        }
        if (!running && !system->CanRunWhilePaused())
        {
            continue;
        }
        system->Update(delta);
    }
}

bool glimmer::WorldContext::OnBackPressed() const
{
    bool handled = false;
    for (auto& system : activeSystems)
    {
        if (system == nullptr)
        {
            continue;
        }
        if (!running && !system->CanRunWhilePaused())
        {
            continue;
        }
        if (system->OnBackPressed())
        {
            handled = true;
        }
    }
    return handled;
}

void glimmer::WorldContext::Render(SDL_Renderer* renderer) const
{
    for (const std::unique_ptr<GameSystem>& system : activeSystems)
    {
#if  defined(NDEBUG)
        system->Render(renderer);
#else
        SDL_Color oldColor;
        SDL_GetRenderDrawColor(renderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);
        system->Render(renderer);
        SDL_Color newColor;
        SDL_GetRenderDrawColor(renderer, &newColor.r, &newColor.g, &newColor.b, &newColor.a);
        if (oldColor.a != newColor.a || oldColor.r != newColor.r || oldColor.g != newColor.g || oldColor.b != newColor.
            b)
        {
            LogCat::e("The color of the renderer has been changed by the game system.",
                      static_cast<uint8_t>(system->GetGameSystemType()),
                      " invoke AppContext::RestoreColorRenderer(renderer);");
            assert(false);
        }
#endif
    }
}

void glimmer::WorldContext::OnFrameStart()
{
    std::queue<GameSystem*> toActivate;
    std::queue<GameSystem*> toDeactivate;
    bool changed = false;
    for (auto& buffer : onComponentCountChangeBuffer_)
    {
        const GameComponentTypeMessage gameComponentType = buffer.first;
        const uint32_t count = buffer.second;
        for (auto& system : activeSystems)
        {
            if (system == nullptr)
            {
                continue;
            }
            if (system->IsWatchingComponent(gameComponentType))
            {
                if (count == 0)
                {
                    system->RemoveActiveWatchComponent(gameComponentType);
                }
                system->OnWatchedComponentChanged(gameComponentType, count);
            }
        }
        for (auto& system : inactiveSystems)
        {
            if (system == nullptr)
            {
                continue;
            }
            if (system->IsWatchingComponent(gameComponentType))
            {
                system->AddActiveWatchComponent(gameComponentType);
                system->OnWatchedComponentChanged(gameComponentType, count);
            }
        }
    }
    for (auto& system : inactiveSystems)
    {
        if (system == nullptr)
        {
            continue;
        }
        if (system->IsAllWatchComponentsReady() && system->CanActive())
        {
            toActivate.push(system.get());
            changed = true;
        }
    }
    for (auto& system : activeSystems)
    {
        if (system == nullptr)
        {
            continue;
        }
        if (!system->IsAllWatchComponentsReady() || !system->CanActive())
        {
            toDeactivate.push(system.get());
            changed = true;
        }
    }
    onComponentCountChangeBuffer_.clear();
    // Batch mobile activation system
    // 批量移动激活系统
    while (!toActivate.empty())
    {
        GameSystem* system = toActivate.front();
        toActivate.pop();
        auto it = std::ranges::find_if(inactiveSystems,
                                       [system](auto& inactiveSystem) { return inactiveSystem.get() == system; });
        if (it == inactiveSystems.end())
        {
            continue;
        }
        GameSystem* systemPtr = it->get();
        if (systemPtr != nullptr)
        {
            systemPtr->OnActivationChanged(true);
        }
        activeSystems.push_back(std::move(*it));
        inactiveSystems.erase(it);
    }

    // Batch mobile deactivation of the system
    // 批量移动停用系统
    while (!toDeactivate.empty())
    {
        GameSystem* system = toDeactivate.front();
        toDeactivate.pop();
        auto it = std::ranges::find_if(activeSystems,
                                       [system](auto& activeSystem) { return activeSystem.get() == system; });
        if (it == activeSystems.end())
        {
            continue;
        }
        GameSystem* systemPtr = it->get();
        if (systemPtr != nullptr)
        {
            systemPtr->OnActivationChanged(false);
        }
        inactiveSystems.push_back(std::move(*it));
        activeSystems.erase(it);
    }
    // //Sort by rendering order (lower layers at the bottom, upper layers at the top)
    // //按渲染顺序排序（低层在底，高层在上）
    if (changed)
    {
        std::ranges::stable_sort(activeSystems,
                                 [](const std::unique_ptr<GameSystem>& a, const std::unique_ptr<GameSystem>& b)
                                 {
                                     return a->GetRenderOrder() < b->GetRenderOrder();
                                 });
    }
}

void glimmer::WorldContext::InitSystem()
{
    allowRegisterSystem = true;
    RegisterSystem(std::make_unique<CameraSystem>(this));
    RegisterSystem(std::make_unique<PlayerControlSystem>(this));
    RegisterSystem(std::make_unique<TileLayerSystem>(this));
    RegisterSystem(std::make_unique<ChunkSystem>(this));
    RegisterSystem(std::make_unique<PhysicsSystem>(this));
    RegisterSystem(std::make_unique<HotBarSystem>(this));
    RegisterSystem(std::make_unique<ItemSlotSystem>(this));
    RegisterSystem(std::make_unique<MagnetSystem>(this));
    RegisterSystem(std::make_unique<ParallaxBackgroundSystem>(this));
    RegisterSystem(std::make_unique<FloatingTextSystem>(this));
    RegisterSystem(std::make_unique<DroppedItemSystem>(this));
    RegisterSystem(std::make_unique<AutoPickSystem>(this));
    RegisterSystem(std::make_unique<AreaMarkerSystem>(this));
    RegisterSystem(std::make_unique<DiggingSystem>(this));
    RegisterSystem(std::make_unique<DraggableSystem>(this));
    RegisterSystem(std::make_unique<SpiritRendererSystem>(this));
    RegisterSystem(std::make_unique<PauseSystem>(this));
    RegisterSystem(std::make_unique<RayCast2DSystem>(this));
    RegisterSystem(std::make_unique<BiomeBGMSystem>(this));
    RegisterSystem(std::make_unique<Light2DSystem>(this));
    RegisterSystem(std::make_unique<BlueprintSystem>(this));
    RegisterSystem(std::make_unique<InventoryGUISystem>(this));
    RegisterSystem(std::make_unique<RecipeGUISystem>(this));
#if  !defined(NDEBUG)
    RegisterSystem(std::make_unique<DebugDrawSystem>(this));
    RegisterSystem(std::make_unique<DebugDrawBox2dSystem>(this));
    RegisterSystem(std::make_unique<DebugPanelSystem>(this));
    RegisterSystem(std::make_unique<DebugMultiMapSystem>(this));
#endif
#ifdef __ANDROID__
    RegisterSystem(std::make_unique<AndroidControlSystem>(this));
#endif
    allowRegisterSystem = false;
}


glimmer::LightBuffer* glimmer::WorldContext::GetLightingBuffer() const
{
    return lightBuffer_.get();
}

void glimmer::WorldContext::BindCameraComponent(CameraComponent* cameraComponent)
{
    if (entityShortCut_ == nullptr)
    {
        return;
    }
    entityShortCut_->SetCameraComponent(cameraComponent);
    Config* config = appContext_->GetConfig();
    if (config == nullptr)
    {
        return;
    }
    if (configChangedId_ != INVALID_CONFIG_CALL_BACK)
    {
        config->UnregisterOnConfigChanged(configChangedId_);
    }
    configChangedId_ = config->RegisterOnConfigChanged(true, std::make_unique<std::function<void(const Config*)>>(
                                                           [ cameraComponent](const Config* newConfig)
                                                           {
                                                               float oldZoom = cameraComponent->GetZoom();
                                                               float newZoom = newConfig->window.cameraScale;
                                                               if (oldZoom == newZoom)
                                                               {
                                                                   return;
                                                               }
                                                               cameraComponent->SetZoom(newZoom);
                                                           }));
}


void glimmer::WorldContext::RegisterSystem(std::unique_ptr<GameSystem> system)
{
    if (allowRegisterSystem)
    {
        system->LockWatchComponent();
        inactiveSystems.push_back(std::move(system));
    }
    else
    {
        LogCat::e("WorldContext is not allowed to register system");
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
        mapManifestMessageData->totalplaytime() + (endTime - GetStartTime()));
    mapManifestMessageData->set_lastplayedtime(endTime);
    mapManifestMessageData->set_entityidindex(entityManager_->GetEntityIndex());
    if (!saves->WriteMapManifest(mapManifestMessageData.value()))
    {
        saving_ = false;
        return;
    }
    //保存玩家
    auto player = entityShortCut_->GetPlayer();
    if (!IsEmptyEntityId(player) && entityManager_->IsPersistable(player))
    {
        PlayerMessage playerMessage;
        SaveEntity(playerMessage.mutable_entity(), player);
        (void)saves->WritePlayer(playerMessage);
    }


    //Save all blocks.
    //保存所有区块。
    auto allChunks = GetAllChunks();
    for (const auto& pos : *allChunks | std::views::keys)
    {
        (void)SaveChunk(pos);
    }
    saving_ = false;
}


bool glimmer::WorldContext::IsEmptyEntityId(const uint32_t id)
{
    return id == GAME_ENTITY_ID_INVALID;
}

int glimmer::WorldContext::GetWorldSeed() const
{
    return worldSeed_;
}

glimmer::WorldContext::WorldContext(AppContext* appContext, MapManifest* mapManifest, Saves* saves) : saves_(
    saves)
{
    worldSeed_ = mapManifest->seed;
    mapManifest_ = mapManifest;
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2(0.0F, -10.0F);
    worldId_ = b2CreateWorld(&worldDef);
    appContext_ = appContext;
    appContext->GetCommandManager()->BindWorldContext(this);
    appContext_->GetBiomeDecoratorManager()->SetWorldSeed(worldSeed_);
    appContext->GetStructureGeneratorManager()->SetWorldSeed(worldSeed_);
    entityManager_ = std::make_unique<EntityManager>();
    onComponentCountChangedId_ = entityManager_->RegisterOnComponentCountChanged(
        [this](GameComponentTypeMessage type, uint32_t count)
        {
            OnWatchedComponentChanged(type, count);
        });
    entityShortCut_ = std::make_unique<EntityShortCut>();
    entityManager_->SetEntityIndex(mapManifest_->entityIDIndex);
    chunkLoader_ = std::make_unique<ChunkLoader>(this, saves);
    lightBuffer_ = std::make_unique<LightBuffer>();
    tileInstancePool_ = std::make_unique<TileInstancePool>();
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
    ResourceRef playerResourceRef{};
    playerResourceRef.ReadResource(*appContext->GetMobManager()->GetPlayerResourceList()[0],
                                   RESOURCE_MOB);
    InitPlayer(
        playerResourceRef);
    ItemContainer* itemContainerPtr = entityManager_->
                                      GetComponent<ItemContainerComponent>(entityShortCut_->GetPlayer())->
                                      GetItemContainer();
    InitHotbar(itemContainerPtr);
    InitInventory(itemContainerPtr);
    InitSystem();
    LogCat::i("Camera entity created with CameraComponent, WorldPositionComponent and PlayerControlComponent");
}

long glimmer::WorldContext::GetStartTime() const
{
    return startTime_;
}

glimmer::AppContext* glimmer::WorldContext::GetAppContext() const
{
    return appContext_;
}

b2WorldId glimmer::WorldContext::GetWorldId() const
{
    return worldId_;
}
