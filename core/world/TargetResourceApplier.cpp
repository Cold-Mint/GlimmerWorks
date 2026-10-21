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
#include "TargetResourceApplier.h"

#include "core/context/AppContext.h"
#include "core/context/ModContext.h"
#include "core/ecs/DroppedItemCreator.h"
#include "core/inventory/Item.h"
#include "core/math/CoordinateTransformer.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceLocator.h"
#include "core/world/ChunkManager.h"
#include "core/world/WorldContext.h"
#include "core/world/generator/Chunk.h"
#include "core/world/structure/StructureGeneratorManager.h"

bool glimmer::TargetResourceApplier::Apply(WorldContext *worldContext,
                                           const TileVector2D &originTilePosition,
                                           const TileLayerType layerType,
                                           const ResourceRef &targetRef,
                                           const BreakSource breakSource,
                                           const PlaceSourceMessage placeSource) {
    if (worldContext == nullptr) {
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
    ChunkManager *chunkManager = worldContext->GetChunkManager();
    if (chunkManager == nullptr) {
        return false;
    }

    const ResourceTypeMessage resourceType = targetRef.GetResourceType();

    //Resolve the origin chunk and index (used by the tile branch and to read the spawn origin offset).
    //解析原点区块与索引（瓦片分支使用，以及用于读取生成原点偏移）。
    Chunk *originChunk = chunkManager->GetChunk(CoordinateTransformer::TileToChunkVertex(originTilePosition));
    if (originChunk == nullptr) {
        return false;
    }
    const ChunkRelativeVector2D originRelative = CoordinateTransformer::TileToChunkRelative(originTilePosition);
    const size_t originIndex = CoordinateTransformer::GetArrayIndex(originRelative.x, originRelative.y, CHUNK_SHIFT);

    //Spawn origin offset: item / loot / structure spawn here, and the tile branch places its target here too.
    //生成原点偏移：物品 / 战利品 / 结构在此生成，瓦片分支也在此放置目标。
    Vector2DIResource spawnOriginOffset{0, 0};
    if (const Tile *originTile = originChunk->GetTile(layerType, originIndex); originTile != nullptr) {
        spawnOriginOffset = originTile->GetSpawnOriginOffset();
    }
    const TileVector2D spawnOrigin = {
        originTilePosition.x + spawnOriginOffset.x, originTilePosition.y + spawnOriginOffset.y
    };
    const WorldVector2D spawnWorldPosition = CoordinateTransformer::TileToWorld(spawnOrigin);

    if (resourceType == RESOURCE_TILE) {
        const TileResource *tileResource = resourceLocator->FindTileRaw(&targetRef);
        if (tileResource == nullptr) {
            return false;
        }
        for (int x = 0; x < tileResource->tileWidth; x++) {
            for (int y = 0; y < tileResource->tileHeight; y++) {
                const TileVector2D worldVector2D = {spawnOrigin.x + x, spawnOrigin.y + y};
                Chunk *targetChunk = chunkManager->GetChunk(
                    CoordinateTransformer::TileToChunkVertex(worldVector2D));
                if (targetChunk == nullptr) {
                    continue;
                }
                const ChunkRelativeVector2D relative =
                        CoordinateTransformer::TileToChunkRelative(worldVector2D);
                const size_t unitIndex =
                        CoordinateTransformer::GetArrayIndex(relative.x, relative.y, CHUNK_SHIFT);
                targetChunk->PlaceTile(layerType, unitIndex, targetRef, tileResource, breakSource, placeSource, x, y,
                                       false);
            }
        }
        return true;
    }
    if (resourceType == RESOURCE_ABILITY_ITEM || resourceType == RESOURCE_MATERIAL_ITEM || resourceType ==
        RESOURCE_COMPOSABLE_ITEM) {
        ItemMessageResource itemMessageResource;
        itemMessageResource.item = targetRef;
        itemMessageResource.amount = 1;
        std::unique_ptr<Item> item = resourceLocator->FindItem(worldContext, itemMessageResource);
        if (item == nullptr) {
            return false;
        }
        DroppedItemCreator::SpawnDroppedItem(worldContext, spawnWorldPosition, std::move(item), 0);
        return true;
    }
    if (resourceType == RESOURCE_LOOT_TABLE) {
        LootResource *lootResource = resourceLocator->FindLoot(&targetRef);
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
        IStructureResource *structureResource = resourceLocator->FindStructure(&targetRef);
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
                        targetChunk->PlaceTile(structureLayerType, unitIndex, resourceRef, tileResource, breakSource,
                                               placeSource, x, y, false);
                    }
                }
            }
        }
        return true;
    }
    return false;
}
