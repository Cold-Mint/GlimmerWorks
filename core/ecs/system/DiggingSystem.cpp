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


uint16_t glimmer::DiggingSystem::BreakTile(BreakSource breakSource, WorldContext* worldContext,
                                           const TileLayerComponent* tileLayerComponent,
                                           const TileVector2D& topLeftVector, bool precisionMining,
                                           bool isPlaceMode, uint8_t tileWidth, uint8_t tileHeight,
                                           const ResourceRef& newTileRef)
{
    if (worldContext == nullptr || tileLayerComponent == nullptr)
    {
        return 0;
    }
    const AppContext* appContext = worldContext->GetAppContext();
    if (appContext == nullptr)
    {
        return 0;
    }
    const EntityShortCut* entityShortCut = worldContext->GetEntityShortCut();
    if (entityShortCut == nullptr)
    {
        return 0;
    }
    GameEntityID player = entityShortCut->GetPlayer();
    if (WorldContext::IsEmptyEntityId(player))
    {
        return 0;
    }
    EntityManager* entityManager = worldContext->GetEntityManager();
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
    uint8_t centerX = tileWidth >> 1;
    uint8_t centerY = tileHeight >> 1;
    for (int x = 0; x < tileWidth; x++)
    {
        for (int y = 0; y < tileHeight; y++)
        {
            auto currentVector = TileVector2D(topLeftVector.x + x, topLeftVector.y - y);
            const auto currentTile = tileLayerComponent->GetSelfLayerTileShared(currentVector);
            if (currentTile == nullptr)
            {
                continue;
            }
            if (isPlaceMode && !currentTile->IsOverwritable())
            {
                continue;
            }
            if (!isPlaceMode && !currentTile->IsBreakable())
            {
                continue;
            }
            TileStateMessage* tileStateMessage = tileLayerComponent->GetSelfLayerTileStateMutable(currentVector);
            ResourceRef oldResourceRef;
            oldResourceRef.ReadResourceRefMessage(tileStateMessage->resourceref());
            uint8_t oldWidth = tileStateMessage->width();
            uint8_t oldHeight = tileStateMessage->height();
            auto oldVector2D = Vector2DI{0, 0};
            oldVector2D.ReadVector2DIMessage(tileStateMessage->offset());
            newTileRef.WriteResourceRefMessage(*tileStateMessage->mutable_resourceref());
            tileStateMessage->set_width(tileWidth);
            tileStateMessage->set_height(tileHeight);
            if (isPlaceMode)
            {
                tileStateMessage->set_placesource(PLACE_SOURCE_PLAYER);
            }
            else
            {
                tileStateMessage->set_placesource(PLACE_SOURCE_WORLD_GEN);
            }
            TileVector2D offset = topLeftVector - currentVector;
            offset.WriteVector2DIMessage(*tileStateMessage->mutable_offset());
            if (!tileLayerComponent->CommitTileState(breakSource, tileLayerComponent->GetTileLayerType(), currentVector,
                                                     false))
            {
                //If the placement fails, then restore to the previous state.
                //如果放置失败了那么还原到之前的状态。
                oldResourceRef.WriteResourceRefMessage(*tileStateMessage->mutable_resourceref());
                tileStateMessage->set_width(oldWidth);
                tileStateMessage->set_height(oldHeight);
                oldVector2D.WriteVector2DIMessage(*tileStateMessage->mutable_offset());
                continue;
            }
            sum++;
            bool center = x == centerX && y == centerY;
            auto breakSFX = currentTile->GetBreakSFX();
            if (item != nullptr)
            {
                if (currentTile->IsAutoDigCostScale() || center)
                {
                    item->Reduce(currentTile->GetUnitDigCost());
                }
            }
            //Only when the tiles are damaged, does the center point emit sound effects.
            //只有瓦片被破坏时，其中心点，发出音效。
            if (center && !isPlaceMode && breakSFX != nullptr)
            {
                appContext->GetAudioManager()->TryPlayFree(AMBIENT, breakSFX, 0);
            }
            if (center || currentTile->LootScaleBySize())
            {
                bool canDropLoot = currentTile->CanDropLoot();
                if (canDropLoot && item != nullptr)
                {
                    if (currentTile->GetMinMiningEfficiency() > item->GetAbilityConfig()->miningEfficiency)
                    {
                        canDropLoot = false;
                    }
                }
                if (canDropLoot)
                {
                    const auto lootResource = appContext->GetResourceLocator()->
                                                          FindLoot(currentTile->GetLootTableRef());
                    if (precisionMining || !currentTile->IsCustomLootTable() || lootResource == nullptr)
                    {
                        const uint32_t droppedEntity = entityManager->AddEntity();
                        DroppedItemCreator droppedItemCreator{worldContext};
                        droppedItemCreator.LoadTemplateComponents(droppedEntity,
                                                                  DroppedItemCreator::GetResourceRef());
                        droppedItemCreator.MergeEntityItemMessage(droppedEntity,
                                                                  DroppedItemCreator::GetEntityItemMessage(
                                                                      CoordinateTransformer::TileToWorld(
                                                                          currentVector),
                                                                      std::make_unique<TileItem>(
                                                                          currentTile, oldResourceRef),
                                                                      0));
                    }
                    else
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
                                                                          CoordinateTransformer::TileToWorld(
                                                                              topLeftVector),
                                                                          std::move(itemPtr), 0));
                        }
                    }
                }
            }
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
            const MiningRangeData* miningRangeData = diggingComponent_->GetMiningRangeData();
            const size_t pointsCount = miningRangeData->GetPointsCount();
            if (pointsCount > 0)
            {
                for (size_t i = 0; i < pointsCount; i++)
                {
                    const MiningRangeDataPoint* point = miningRangeData->GetPoint(i);
                    if (point == nullptr)
                    {
                        continue;
                    }
                    BreakTile(BreakSource::PlayerMining, worldContext, tileLayer, point->GetTileTopLeftPosition(),
                              diggingComponent_->IsPrecisionMining(), false, point->GetWidth(),
                              point->GetHeight(),
                              TileResourceManager::GetAirResourceRef(tileLayerType));
                }
            }
            // Reset digging after break
            // 破坏方块重置挖掘
            diggingComponent_->SetProgress(0.0F);
            diggingComponent_->SetEnable(false);
        }
        break;
    }
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
    if (pointsCount > 0)
    {
        for (size_t i = 0; i < pointsCount; i++)
        {
            const MiningRangeDataPoint* point = miningRangeData->GetPoint(i);
            if (point == nullptr)
            {
                continue;
            }
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
            if (crackTextureResult != nullptr)
            {
                SDL_Texture* texture = crackTextureResult->GetResource();
                if (texture != nullptr)
                {
                    SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
                }
            }
        }
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
