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
#include "CropSystem.h"

#include "core/context/AppContext.h"
#include "core/context/ModContext.h"
#include "core/ecs/DroppedItemCreator.h"
#include "core/ecs/component/CropComponent.h"
#include "core/inventory/Item.h"
#include "core/log/LogCat.h"
#include "core/math/CoordinateTransformer.h"
#include "core/mod/GrowthConditionProcessorManager.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/ResourceRef.h"
#include "core/mod/dataPack/GrowthConditionsRegistry.h"
#include "core/mod/dataPack/TileResourceManager.h"
#include "core/scene/MainThreadDispatcher.h"
#include "core/world/ChunkManager.h"
#include "core/world/WorldContext.h"
#include "core/world/generator/Chunk.h"
#include "core/world/growth/IGrowthConditionProcessor.h"
#include "core/world/structure/StructureGeneratorManager.h"
#include "src/saves/tile_state.pb.h"

glimmer::CropSystem::CropSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    WatchComponent(COMPONENT_CROP);
    Init();
}

void glimmer::CropSystem::OnWatchedComponentChanged(const GameComponentTypeMessage gameComponentType,
                                                    const uint32_t count) {
    if (gameComponentType != COMPONENT_CROP) {
        return;
    }
    std::lock_guard lock(cropMutex_);
    cropCount_ = count;
    cropComponents_.clear();
    EntityManager *entityManager = GetEntityManager();
    if (entityManager == nullptr || cropCount_ == 0) {
        return;
    }
    const std::vector<GameEntityID> cropEntities = entityManager->GetEntityIDWithComponents({COMPONENT_CROP});
    for (const GameEntityID entity: cropEntities) {
        auto *cropComponent = entityManager->GetComponent<CropComponent>(entity);
        if (cropComponent != nullptr) {
            cropComponents_.emplace_back(cropComponent);
        }
    }
    LogCat::d(LogLabel::DEFAULT, "crop_system_entities_rebuilt", "CropSystem rebuilt entities: {}",
              cropComponents_.size());
}

bool glimmer::CropSystem::OnGrowMature(WorldContext *worldContext, Chunk *chunk, const ChunkRelativeVector2D &position,
                                       TileLayerType layerType, const ResourceRef *growthTargetRef) {
    if (worldContext == nullptr || chunk == nullptr || growthTargetRef == nullptr) {
        return false;
    }
    const AppContext *appContext = worldContext->GetAppContext();
    if (appContext == nullptr) {
        return false;
    }
    ResourceLocator *resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator == nullptr) {
        return false;
    }
    const ResourceTypeMessage resourceType = growthTargetRef->GetResourceType();
    const size_t index = CoordinateTransformer::GetArrayIndex(position.x, position.y, CHUNK_SHIFT);
    const TileVector2D absolutePosition = CoordinateTransformer::ChunkRelativeToTile(chunk->GetPosition(), position);
    Vector2DIResource spawnOriginOffset{0, 0};
    if (const Tile *cropTile = chunk->GetTile(layerType, index); cropTile != nullptr) {
        spawnOriginOffset = cropTile->GetSpawnOriginOffset();
    }
    const TileVector2D spawnOrigin = {
        absolutePosition.x + spawnOriginOffset.x, absolutePosition.y + spawnOriginOffset.y
    };
    const WorldVector2D spawnWorldPosition = CoordinateTransformer::TileToWorld(spawnOrigin);
    if (resourceType == RESOURCE_TILE) {
        const TileResource *tileResource = resourceLocator->FindTileRaw(growthTargetRef);
        if (tileResource == nullptr) {
            return false;
        }
        for (int x = 0; x < tileResource->tileWidth; x++) {
            for (int y = 0; y < tileResource->tileHeight; y++) {
                const size_t unitIndex = index + CoordinateTransformer::GetArrayIndex(x, y, CHUNK_SHIFT);
                chunk->PlaceTile(layerType, unitIndex, *growthTargetRef, tileResource, BreakSource::CropGrowthOverwrite,
                                 PLACE_SOURCE_CROP_GROWTH, x, y, false);
            }
        }
        return true;
    }
    if (resourceType == RESOURCE_ABILITY_ITEM || resourceType == RESOURCE_MATERIAL_ITEM || resourceType ==
        RESOURCE_COMPOSABLE_ITEM) {
        ItemMessageResource itemMessageResource;
        itemMessageResource.item = *growthTargetRef;
        itemMessageResource.amount = 1;
        std::unique_ptr<Item> item = resourceLocator->FindItem(worldContext, itemMessageResource);
        if (item == nullptr) {
            return false;
        }
        DroppedItemCreator::SpawnDroppedItem(worldContext, spawnWorldPosition, std::move(item), 0);
        return true;
    }
    if (resourceType == RESOURCE_LOOT_TABLE) {
        LootResource *lootResource = resourceLocator->FindLoot(growthTargetRef);
        if (lootResource == nullptr) {
            return false;
        }
        const std::vector<ItemMessage> itemMessageList = LootResource::GetLootItems(lootResource);
        for (const auto &itemMessage: itemMessageList) {
            std::unique_ptr<Item> item = resourceLocator->FindItem(worldContext, itemMessage);
            if (item == nullptr) {
                continue;
            }
            item->ReadItemMessage(worldContext, itemMessage);
            DroppedItemCreator::SpawnDroppedItem(worldContext, spawnWorldPosition, std::move(item), 0);
        }
        return true;
    }
    if (resourceType == RESOURCE_STRUCTURE) {
        IStructureResource *structureResource = resourceLocator->FindStructure(growthTargetRef);
        if (structureResource == nullptr) {
            return false;
        }
        ModContext *modContext = appContext->GetModContext();
        if (modContext == nullptr) {
            return false;
        }
        StructureGeneratorManager *structureGeneratorManager = modContext->GetStructureGeneratorManager();
        if (structureGeneratorManager == nullptr) {
            return false;
        }
        ChunkManager *chunkManager = worldContext->GetChunkManager();
        if (chunkManager == nullptr) {
            return false;
        }
        const std::unique_ptr<StructureInfo> structureInfo = structureGeneratorManager->Generate(
            worldContext, spawnOrigin, structureResource);
        if (structureInfo == nullptr) {
            return false;
        }
        for (const StructureInfo *structureInfoPtr = structureInfo.get(); const auto &[structureLayerType, tileMap]:
             structureInfoPtr->GetStructureMap()) {
            for (const auto &[coord, resourceRef]: tileMap) {
                const TileResource *tileResource = resourceLocator->FindTileRaw(&resourceRef);
                if (tileResource == nullptr) {
                    continue;
                }
                for (int x = 0; x < tileResource->tileWidth; x++) {
                    for (int y = 0; y < tileResource->tileHeight; y++) {
                        TileVector2D worldVector2D = {
                            spawnOrigin.x + coord.x + x, spawnOrigin.y + coord.y + y
                        };
                        Chunk *targetChunk = chunkManager->GetChunk(
                            CoordinateTransformer::TileToChunkVertex(worldVector2D));
                        if (targetChunk == nullptr) {
                            continue;
                        }
                        const ChunkRelativeVector2D relative =
                                CoordinateTransformer::TileToChunkRelative(worldVector2D);
                        const size_t unitIndex =
                                CoordinateTransformer::GetArrayIndex(relative.x, relative.y, CHUNK_SHIFT);
                        targetChunk->PlaceTile(structureLayerType, unitIndex, resourceRef, tileResource,
                                               BreakSource::CropGrowthOverwrite, PLACE_SOURCE_CROP_GROWTH, x, y, false);
                    }
                }
            }
        }
        return true;
    }
    return false;
}


void glimmer::CropSystem::OnTick(const uint64_t tick) {
    WorldContext *worldContext = GetWorldContext();
    if (worldContext == nullptr) {
        return;
    }
    std::vector<CropComponent *> cropComponents;
    {
        std::lock_guard lock(cropMutex_);
        if (cropComponents_.empty()) {
            return;
        }
        cropComponents = cropComponents_;
    }
    ChunkManager *chunkManager = worldContext->GetChunkManager();
    const AppContext *appContext = worldContext->GetAppContext();
    if (chunkManager == nullptr || appContext == nullptr) {
        return;
    }
    const WindowContext *windowContext = appContext->GetWindowContext();
    if (windowContext == nullptr) {
        return;
    }
    ModContext *modContext = appContext->GetModContext();
    if (modContext == nullptr) {
        return;
    }
    GrowthConditionProcessorManager *growthConditionProcessorManager = modContext->
            GetGrowthConditionProcessorManager();
    GrowthConditionsRegistry *growthConditionsRegistry = modContext->GetGrowthConditionsRegistry();
    if (growthConditionProcessorManager == nullptr || growthConditionsRegistry == nullptr) {
        return;
    }
    for (CropComponent *cropComponent: cropComponents) {
        if (cropComponent == nullptr) {
            LogCat::d(LogLabel::DEFAULT, "crop_system_component_null", "[CropSystem] cropComponent is null, skip");
            continue;
        }
        const TileVector2D &position = cropComponent->GetPosition();
        const TileLayerType layerType = cropComponent->GetLayerType();
        Chunk *chunk = chunkManager->GetChunk(CoordinateTransformer::TileToChunkVertex(position));
        if (chunk == nullptr) {
            LogCat::d(LogLabel::DEFAULT, "crop_system_chunk_not_found",
                      "[CropSystem] chunk not found: position=({}, {}), layerType={}, skip", position.x, position.y,
                      static_cast<int>(layerType));
            continue;
        }
        const ChunkRelativeVector2D relative = CoordinateTransformer::TileToChunkRelative(position);
        const size_t index = CoordinateTransformer::GetArrayIndex(relative.x, relative.y, CHUNK_SHIFT);
        const Tile *tile = chunk->GetTile(layerType, index);
        if (tile == nullptr || !tile->IsCropsBlock()) {
            LogCat::d(LogLabel::DEFAULT, "crop_system_tile_not_crop",
                      "[CropSystem] tile is null or not a crops block: position=({}, {}), layerType={}, index={}, skip",
                      position.x, position.y, static_cast<int>(layerType), index);
            continue;
        }

        bool conditionsMet = true;
        for (const ResourceRef &conditionRef: tile->GetGrowthConditions()) {
            IGrowthConditionResource *conditionResource = growthConditionsRegistry->Find(
                conditionRef.GetPackageId(), conditionRef.GetResourceKey());
            if (conditionResource == nullptr) {
                LogCat::w(LogLabel::DEFAULT, std::source_location::current(),
                          "crop_system_condition_resource_not_found",
                          "[CropSystem] growth condition resource not found: packageId={}, resourceKey={}",
                          conditionRef.GetPackageId(), conditionRef.GetResourceKey());
                conditionsMet = false;
                break;
            }
            const auto processorType = static_cast<GrowthConditionProcessorType>(conditionResource->processorId);
            IGrowthConditionProcessor *processor = growthConditionProcessorManager->FindConditionProcessors(
                processorType);
            if (processor == nullptr) {
                LogCat::w(LogLabel::DEFAULT, std::source_location::current(),
                          "crop_system_condition_processor_not_found",
                          "[CropSystem] growth condition processor not found: processorType={}",
                          static_cast<int>(processorType));
                conditionsMet = false;
                break;
            }
            if (!processor->Match(worldContext, position, conditionResource)) {
                LogCat::d(LogLabel::DEFAULT, "crop_system_condition_not_met",
                          "[CropSystem] growth condition not met: processorType={}, packageId={}, resourceKey={}, position=({}, {})",
                          static_cast<int>(processorType), conditionRef.GetPackageId(),
                          conditionRef.GetResourceKey(), position.x, position.y);
                conditionsMet = false;
                break;
            }
        }
        TileStateMessage *tileState = chunk->GetOrCreateTileState(layerType, index);
        if (tileState == nullptr) {
            LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "crop_system_tile_state_null",
                      "[CropSystem] tile state is null: position=({}, {}), layerType={}, index={}",
                      position.x, position.y, static_cast<int>(layerType), index);
            continue;
        }
        if (!conditionsMet) {
            //Record that conditions were not met this round, so offline growth knows not to advance this crop.
            //记录本轮条件未满足，供离线生长判断不推进该作物。
            tileState->set_tilegrowthconditionsmet(false);
            LogCat::d(LogLabel::DEFAULT, "crop_system_conditions_not_met",
                      "[CropSystem] growth conditions not fully met, skip growth: position=({}, {}), layerType={}",
                      position.x, position.y, static_cast<int>(layerType));
            continue;
        }
        //Conditions are met: mark them in the persisted state and record the current tick as the progress
        //timestamp (used for offline catch-up when the chunk is reloaded).
        //条件满足：写入持久化状态，并把当前 tick 记为进度时间戳（用于区块重新加载时补算离线时长）。
        tileState->set_tilegrowthconditionsmet(true);
        tileState->set_tilegrowthstarttick(tick);
        tileState->set_tilegrowthaccumulatedtick(tileState->tilegrowthaccumulatedtick() + 1);
        LogCat::d(LogLabel::DEFAULT, "crop_system_growth_ticks",
                  "[CropSystem] growth ticks: required={}, accumulated={}, position=({}, {})",
                  tileState->tilegrowthrequiredtick(), tileState->tilegrowthaccumulatedtick(), position.x,
                  position.y);
        if (tileState->tilegrowthrequiredtick() > tileState->tilegrowthaccumulatedtick()) {
            LogCat::d(LogLabel::DEFAULT, "crop_system_growth_not_ready",
                      "[CropSystem] growth tick not ready: accumulated={}, required={}, position=({}, {})",
                      tileState->tilegrowthaccumulatedtick(), tileState->tilegrowthrequiredtick(), position.x,
                      position.y);
            continue;
        }
        const ResourceRef *growthTargetRef = tile->GetGrowthTarget();
        if (growthTargetRef == nullptr) {
            LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "crop_system_growth_target_null",
                      "[CropSystem] growth target is null: position=({}, {}), layerType={}, index={}",
                      position.x, position.y, static_cast<int>(layerType), index);
            continue;
        }
        const bool isTileTarget = growthTargetRef->GetResourceType() == RESOURCE_TILE;
        const bool destroySelfAfterGrowth = tile->DestroySelfAfterGrowth();
        MainThreadDispatcher *mainThreadDispatcher = appContext->GetMainThreadDispatcher();
        if (mainThreadDispatcher == nullptr) {
            LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "crop_system_main_thread_dispatcher_null",
                      "[CropSystem] main thread dispatcher is null, skip growth: position=({}, {}), layerType={}",
                      position.x, position.y, static_cast<int>(layerType));
            continue;
        }
        tileState->set_tilegrowthaccumulatedtick(0);
        const ResourceRef &growthTarget = *growthTargetRef;
        const ChunkVertexVector2D chunkPositionCopy = chunk->GetPosition();

        mainThreadDispatcher->PostToNextMainFrame(
            [worldContext, chunkPositionCopy, relative, layerType, index, growthTarget, isTileTarget,
                destroySelfAfterGrowth, tick, position] {
                const ChunkManager *chunkManager = worldContext->GetChunkManager();
                if (chunkManager == nullptr) {
                    return;
                }
                Chunk *chunk = chunkManager->GetChunk(chunkPositionCopy);
                if (chunk == nullptr) {
                    return;
                }
                if (OnGrowMature(worldContext, chunk, relative, layerType, &growthTarget)) {
                    LogCat::d(LogLabel::DEFAULT, "crop_system_grow_mature_success",
                              "[CropSystem] crop matured: position=({}, {}), layerType={}, targetPackageId={}, targetResourceKey={}, isTileTarget={}, destroySelfOnGrowth={}",
                              position.x, position.y, static_cast<int>(layerType), growthTarget.GetPackageId(),
                              growthTarget.GetResourceKey(), isTileTarget, destroySelfAfterGrowth);
                    if (!isTileTarget && destroySelfAfterGrowth) {
                        const AppContext *appContext = worldContext->GetAppContext();
                        if (appContext == nullptr) {
                            return;
                        }

                        ResourceRef airRef;
                        TileResourceManager::WriteAirResourceRef(layerType, airRef);
                        if (ResourceLocator *resourceLocator = appContext->GetResourceLocator();
                            resourceLocator != nullptr) {
                            if (const TileResource *airTileResource = resourceLocator->FindTileRaw(&airRef);
                                airTileResource != nullptr) {
                                chunk->PlaceTile(layerType, index, airRef, airTileResource,
                                                 BreakSource::CropSelfDestruct,
                                                 PLACE_SOURCE_CROP_GROWTH, 0, 0, false);
                                LogCat::d(LogLabel::DEFAULT, "crop_system_replace_with_air",
                                          "[CropSystem] crop replaced with air after growth: position=({}, {}), layerType={}, index={}",
                                          position.x, position.y, static_cast<int>(layerType), index);
                            }
                        }
                    } else {
                        TileStateMessage *tileState = chunk->GetOrCreateTileState(layerType, index);
                        if (tileState == nullptr) {
                            return;
                        }
                        tileState->set_maturecount(tileState->maturecount() + 1);
                        tileState->set_tilegrowthaccumulatedtick(0);
                        tileState->set_tilegrowthstarttick(tick);
                        tileState->set_tilegrowthconditionsmet(false);
                        LogCat::d(LogLabel::DEFAULT, "crop_system_growth_reset",
                                  "[CropSystem] crop growth reset after mature: matureCount={}, position=({}, {})",
                                  tileState->maturecount(), position.x, position.y);
                    }
                } else {
                    LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "crop_system_grow_mature_failed",
                              "[CropSystem] OnGrowMature failed: position=({}, {}), layerType={}, targetPackageId={}, targetResourceKey={}",
                              position.x, position.y, static_cast<int>(layerType), growthTarget.GetPackageId(),
                              growthTarget.GetResourceKey());
                }
            });
    }
}

glimmer::GameSystemType glimmer::CropSystem::GetGameSystemType() const {
    return GameSystemType::CropSystem;
}
