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
#include "DiggingSystem.h"
#include <cstddef>
#include "core/world/TileInstancePool.h"
#include "core/mod/dataPack/TileResourceManager.h"
#include "core/world/WorldContext.h"
#include "core/ecs/component/CameraComponent.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/ecs/component/TileLayerComponent.h"
#include "core/inventory/TileItem.h"
#include "core/world/generator/Chunk.h"
#include "core/ecs/DroppedItemCreator.h"
#include "core/ecs/component/DiggingComponent.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/math/CoordinateTransformer.h"
#include "core/math/Vector2DI.h"


bool glimmer::DiggingSystem::CanProcessTile(const Tile* tile, bool isPlaceMode)
{
    if (tile == nullptr)
    {
        return false;
    }
    if (isPlaceMode && !tile->IsOverwritable())
    {
        return false;
    }
    if (!isPlaceMode && !tile->IsBreakable())
    {
        return false;
    }
    return true;
}

void glimmer::DiggingSystem::SaveTileState(const TileStateMessage* tileState, TileStateBackup& backup)
{
    backup.ReadResourceRefMessage(tileState->resourceref());
    backup.SetWidth(tileState->width());
    backup.SetHeight(tileState->height());
    backup.ReadOffsetMessage(tileState->offset());
}

void glimmer::DiggingSystem::RestoreTileState(TileStateMessage* tileState, const TileStateBackup& backup)
{
    backup.WriteResourceRefMessage(*tileState->mutable_resourceref());
    tileState->set_width(backup.GetWidth());
    tileState->set_height(backup.GetHeight());
    backup.WriteOffsetMessage(*tileState->mutable_offset());
}

bool glimmer::DiggingSystem::TryPlaceTile(const TileLayerComponent* tileLayerComponent,
                                          TileStateMessage* tileState,
                                          const TileVector2D& currentVector,
                                          const TileVector2D& topLeftVector,
                                          const TilePlacementConfig& config,
                                          TileStateBackup& backup)
{
    SaveTileState(tileState, backup);
    config.WriteResourceRefMessage(*tileState->mutable_resourceref());
    tileState->set_width(config.GetTileWidth());
    tileState->set_height(config.GetTileHeight());
    if (config.IsPlaceMode())
    {
        tileState->set_placesource(PLACE_SOURCE_PLAYER);
    }
    else
    {
        tileState->set_placesource(PLACE_SOURCE_WORLD_GEN);
    }
    TileVector2D offset = topLeftVector - currentVector;
    offset.WriteVector2DIMessage(*tileState->mutable_offset());
    if (tileLayerComponent->CommitTileState(config.GetBreakSource(), tileLayerComponent->GetTileLayerType(),
                                            currentVector,
                                            false))
    {
        return true;
    }
    RestoreTileState(tileState, backup);
    return false;
}

void glimmer::DiggingSystem::ApplyItemDurability(Item* item, const Tile* tile, bool isCenter)
{
    if (item == nullptr)
    {
        return;
    }
    if (tile->IsAutoDigCostScale() || isCenter)
    {
        item->Reduce(tile->GetUnitDigCost());
    }
}

void glimmer::DiggingSystem::DropDefaultLoot(WorldContext* worldContext, EntityManager* entityManager,
                                             const std::shared_ptr<Tile>& tile, const TileVector2D& position,
                                             const ResourceRef& oldResourceRef)
{
    const uint32_t droppedEntity = entityManager->AddEntity();
    DroppedItemCreator droppedItemCreator{worldContext};
    droppedItemCreator.LoadTemplateComponents(droppedEntity,
                                              DroppedItemCreator::GetResourceRef());
    droppedItemCreator.MergeEntityItemMessage(droppedEntity,
                                              DroppedItemCreator::GetEntityItemMessage(
                                                  CoordinateTransformer::TileToWorld(position),
                                                  std::make_unique<TileItem>(tile, oldResourceRef),
                                                  0));
}

void glimmer::DiggingSystem::DropTileLoot(WorldContext* worldContext, EntityManager* entityManager,
                                          const std::shared_ptr<Tile>& tile, const TileVector2D& position,
                                          const ResourceRef& oldResourceRef, bool precisionMining)
{
    const AppContext* appContext = worldContext->GetAppContext();
    if (appContext == nullptr)
    {
        return;
    }
    if (!tile->CanDropLoot())
    {
        return;
    }
    const auto lootResource = appContext->GetResourceLocator()->FindLoot(tile->GetLootTableRef());
    if (precisionMining || !tile->IsCustomLootTable() || lootResource == nullptr)
    {
        DropDefaultLoot(worldContext, entityManager, tile, position, oldResourceRef);
        return;
    }
    DropCustomLoot(worldContext, entityManager, appContext, lootResource, position);
}

void glimmer::DiggingSystem::DropCustomLoot(WorldContext* worldContext, EntityManager* entityManager,
                                            const AppContext* appContext, const LootResource* lootResource,
                                            const TileVector2D& topLeftVector)
{
    std::vector<ItemMessage> itemMessageList = LootResource::GetLootItems(lootResource);
    for (auto& itemMessage : itemMessageList)
    {
        auto itemPtr = appContext->GetResourceLocator()->FindItem(worldContext, itemMessage);
        if (itemPtr == nullptr)
        {
            continue;
        }
        itemPtr->ReadItemMessage(worldContext, itemMessage);
        const uint32_t droppedEntity = entityManager->AddEntity();
        DroppedItemCreator droppedItemCreator{worldContext};
        droppedItemCreator.LoadTemplateComponents(droppedEntity,
                                                  DroppedItemCreator::GetResourceRef());
        droppedItemCreator.MergeEntityItemMessage(droppedEntity,
                                                  DroppedItemCreator::GetEntityItemMessage(
                                                      CoordinateTransformer::TileToWorld(topLeftVector),
                                                      std::move(itemPtr), 0));
    }
}


static bool CheckMiningEfficiency(const glimmer::Tile* tile, const glimmer::Item* item)
{
    if (item == nullptr)
    {
        return true;
    }
    if (tile->GetMinMiningEfficiency() > item->GetAbilityConfig()->miningEfficiency)
    {
        return false;
    }
    return true;
}

void glimmer::DiggingSystem::ProcessSingleTile(const TileBreakParams& params,
                                               const TileVector2D& currentVector,
                                               Item* item,
                                               bool isCenter,
                                               uint8_t& sum)
{
    const AppContext* appContext = params.worldContext->GetAppContext();
    EntityManager* entityManager = params.worldContext->GetEntityManager();
    const auto currentTile = params.tileLayerComponent->GetSelfLayerTileShared(currentVector);
    if (!CanProcessTile(currentTile.get(), params.isPlaceMode))
    {
        return;
    }
    TileStateMessage* tileStateMessage = params.tileLayerComponent->GetSelfLayerTileStateMutable(currentVector);
    TileStateBackup backup;
    TilePlacementConfig config;
    config.SetTileHeight(params.tileHeight);
    config.SetTileWidth(params.tileWidth);
    config.SetResourceRef(params.newTileRef);
    config.SetPlaceMode(params.isPlaceMode);
    config.SetBreakSource(params.breakSource);
    if (!TryPlaceTile(params.tileLayerComponent, tileStateMessage, currentVector, params.topLeftVector,
                      config, backup))
    {
        return;
    }
    sum++;
    ApplyItemDurability(item, currentTile.get(), isCenter);
    if (isCenter && !params.isPlaceMode)
    {
        auto breakSFX = currentTile->GetBreakSFX();
        if (breakSFX != nullptr)
        {
            appContext->GetAudioManager()->TryPlayFree(AMBIENT, breakSFX, 0);
        }
    }
    if (!isCenter && !currentTile->LootScaleBySize())
    {
        return;
    }
    if (!CheckMiningEfficiency(currentTile.get(), item))
    {
        return;
    }
    DropTileLoot(params.worldContext, entityManager, currentTile, currentVector,
                 backup.GetResourceRef(), params.precisionMining);
}

uint16_t glimmer::DiggingSystem::BreakTile(const TileBreakParams& params)
{
    if (params.worldContext == nullptr || params.tileLayerComponent == nullptr)
    {
        return 0;
    }
    const AppContext* appContext = params.worldContext->GetAppContext();
    if (appContext == nullptr)
    {
        return 0;
    }
    const EntityShortCut* entityShortCut = params.worldContext->GetEntityShortCut();
    if (entityShortCut == nullptr)
    {
        return 0;
    }
    GameEntityID player = entityShortCut->GetPlayer();
    if (WorldContext::IsEmptyEntityId(player))
    {
        return 0;
    }
    EntityManager* entityManager = params.worldContext->GetEntityManager();
    if (entityManager == nullptr)
    {
        return 0;
    }
    auto playerComponent = entityManager->GetComponent<PlayerComponent>(player);
    Item* item = nullptr;
    if (playerComponent != nullptr)
    {
        item = playerComponent->GetItem();
        if (item == nullptr)
        {
            item = playerComponent->GetEmptyHandAutoUseItem();
        }
    }
    uint8_t sum = 0;
    auto centerX = static_cast<std::byte>(params.tileWidth >> 1);
    auto centerY = static_cast<std::byte>(params.tileHeight >> 1);
    for (int x = 0; x < params.tileWidth; x++)
    {
        for (int y = 0; y < params.tileHeight; y++)
        {
            auto currentVector = TileVector2D(params.topLeftVector.x + x, params.topLeftVector.y - y);
            bool isCenter = (static_cast<std::byte>(x) == centerX && static_cast<std::byte>(y) == centerY);
            ProcessSingleTile(params, currentVector, item, isCenter, sum);
        }
    }
    return sum;
}

void glimmer::DiggingSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    const EntityShortCut* entityShortCut = GetEntityShortCut();
    EntityManager* entityManager = GetEntityManager();
    if (gameComponentType == COMPONENT_DIGGING && diggingComponent_ == nullptr)
    {
        diggingComponent_ = entityShortCut->GetDiggingComponent();
    }
    if (gameComponentType == COMPONENT_TRANSFORM_2D && cameraTransform2DComponent_ == nullptr)
    {
        cameraTransform2DComponent_ = entityShortCut->GetCameraTransform2DComponent();
    }
    if (gameComponentType == COMPONENT_CAMERA && cameraComponent_ == nullptr)
    {
        cameraComponent_ = entityShortCut->GetCameraComponent();
    }
    if (gameComponentType == COMPONENT_TILE_LAYER)
    {
        tileLayerComponents_.clear();
        auto tileLayerEntities = entityManager->GetEntityIDWithComponents({COMPONENT_TILE_LAYER});
        std::sort(tileLayerEntities.begin(), tileLayerEntities.end());
        for (GameEntityID tileLayerEntity : tileLayerEntities)
        {
            const auto tileLayer = entityManager->GetComponent<TileLayerComponent>(tileLayerEntity);
            if (tileLayer == nullptr)
            {
                continue;
            }
            tileLayerComponents_.emplace_back(tileLayer);
        }
    }
}

glimmer::DiggingSystem::DiggingSystem(WorldContext* worldContext) : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_DIGGING);
    WatchComponent(COMPONENT_TRANSFORM_2D);
    WatchComponent(COMPONENT_CAMERA);
    WatchComponent(COMPONENT_TILE_LAYER);
    Init();
}

void glimmer::DiggingSystem::Update(const float delta)
{
    WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        return;
    }
    if (!diggingComponent_->CheckAndResetActive())
    {
        diggingComponent_->SetEnable(false);
        diggingComponent_->SetProgress(0.0F);
        return;
    }
    diggingComponent_->SetEnable(true);
    for (auto tileLayer : tileLayerComponents_)
    {
        const TileLayerType tileLayerType = tileLayer->GetTileLayerType();
        if (tileLayerType != diggingComponent_->GetLayerType())
        {
            continue;
        }
        // Accumulate progress
        // 积累进度
        diggingComponent_->AddProgress(
            diggingComponent_->GetEfficiency() / diggingComponent_->GetMiningRangeData()->GetMaxHardness() * delta);
        if (diggingComponent_->GetProgress() >= 1.0F)
        {
            ProcessMiningComplete(tileLayer, tileLayerType);
        }
        break;
    }
}

void glimmer::DiggingSystem::ProcessMiningComplete(const TileLayerComponent* tileLayer, TileLayerType tileLayerType)
{
    WorldContext* worldContext = GetWorldContext();
    const MiningRangeData* miningRangeData = diggingComponent_->GetMiningRangeData();
    const size_t pointsCount = miningRangeData->GetPointsCount();
    for (size_t i = 0; i < pointsCount; i++)
    {
        const MiningRangeDataPoint* point = miningRangeData->GetPoint(i);
        if (point == nullptr)
        {
            continue;
        }
        BreakTile({BreakSource::PlayerMining, worldContext, tileLayer, point->GetTileTopLeftPosition(),
                  diggingComponent_->IsPrecisionMining(), false, point->GetWidth(),
                  point->GetHeight(),
                  TileResourceManager::GetAirResourceRef(tileLayerType)});
    }
    diggingComponent_->SetProgress(0.0F);
    diggingComponent_->SetEnable(false);
}

void glimmer::DiggingSystem::Render(SDL_Renderer* renderer)
{
    const WorldContext* worldContext = GetWorldContext();
    const AppContext* appContext = worldContext->GetAppContext();
    if (!cacheTexture_)
    {
        for (uint8_t i = 0; i < 10; i++)
        {
            ResourceRef resourceRef;
            resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
            resourceRef.SetResourceType(RESOURCE_TEXTURE);
            resourceRef.SetResourceKey("cracks/cracks_" + std::to_string(i));
            textureResultList_.push_back(appContext->GetResourceLocator()->FindTexture(
                &resourceRef
            ));
        }
        cacheTexture_ = true;
        return;
    }
    if (!diggingComponent_->IsEnable())
    {
        return;
    }
    const MiningRangeData* miningRangeData = diggingComponent_->GetMiningRangeData();
    float zoom = cameraComponent_->GetZoom();
    size_t pointsCount = miningRangeData->GetPointsCount();
    for (size_t i = 0; i < pointsCount; i++)
    {
        const MiningRangeDataPoint* point = miningRangeData->GetPoint(i);
        if (point == nullptr)
        {
            continue;
        }
        RenderDiggingPoint(renderer, point, zoom);
    }
}

void glimmer::DiggingSystem::RenderDiggingPoint(SDL_Renderer* renderer, const MiningRangeDataPoint* point, float zoom)
{
    const TileVector2D& tileTopLeftPosition = point->GetTileTopLeftPosition();
    const WorldVector2D tileTopLeftPositionWorld = CoordinateTransformer::TileToWorld({
        tileTopLeftPosition.x, tileTopLeftPosition.y
    });
    const ScreenVector2D ScreenVector2D = CoordinateTransformer::WorldToScreen(
        cameraTransform2DComponent_->GetPosition(), tileTopLeftPositionWorld, cameraComponent_->GetSize(),
        cameraComponent_->GetZoom());
    const auto maxIndex = static_cast<float>(textureResultList_.size() - 1);
    const uint8_t crackIndex = static_cast<uint8_t>(std::min(diggingComponent_->GetProgress() * maxIndex,
                                                             maxIndex));
    float w = TILE_SIZE * zoom * static_cast<float>(point->GetWidth());
    float h = TILE_SIZE * zoom * static_cast<float>(point->GetHeight());
    SDL_FRect dstRect = {
        ScreenVector2D.x - w * 0.5F, ScreenVector2D.y - h * 0.5F, w,
        h
    };
    auto& crackTextureResult = textureResultList_[crackIndex];
    if (crackTextureResult == nullptr)
    {
        return;
    }
    SDL_Texture* texture = crackTextureResult->GetResource();
    if (texture != nullptr)
    {
        SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
    }
}

uint8_t glimmer::DiggingSystem::GetRenderOrder()
{
    return RENDER_ORDER_DIGGING;
}

glimmer::GameSystemType glimmer::DiggingSystem::GetGameSystemType() const
{
    return GameSystemType::DiggingSystem;
}
