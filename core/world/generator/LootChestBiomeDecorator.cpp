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
#include "LootChestBiomeDecorator.h"

#include "TerrainMath.h"
#include "core/log/LogCat.h"
#include "core/math/CoordinateTransformer.h"
#include "core/world/Dimension.h"
#include "core/world/WorldContext.h"


void glimmer::LootChestBiomeDecorator::DecorationImpl(const ChunkVertexVector2D &chunkVertex,
                                                      WorldContext *worldContext, TerrainResult *terrainResult,
                                                      LootChestBiomeDecoratorResource *decoratorResource,
                                                      BiomeResource *biomeResource,
                                                      std::unordered_map<TileLayerType, std::array<ResourceRef,
                                                          CHUNK_AREA> > *tilesRefMap) {
    if (decoratorResource == nullptr || tilesRefMap == nullptr ||
        !decoratorResource->chestTile.IsValid()) {
        return;
    }
    std::array<ResourceRef, CHUNK_AREA> &targetLayer =
            tilesRefMap->at(static_cast<TileLayerType>(decoratorResource->layer));

    const DimensionResource *dimensionResource = nullptr;
    if (worldContext != nullptr) {
        if (const Dimension *dimension = worldContext->GetDimension(); dimension != nullptr) {
            dimensionResource = dimension->GetDimensionResource();
        }
    }
    const FastNoiseLite *noise = decoratorResource->GetFastNoiseLite(GetWorldSeed());
    if (noise == nullptr) {
        LogCat::w(LogLabel::BIOME_DECORATOR, std::source_location::current(), "loot_chest_decorator_noise_is_null",
                  "Loot chest decorator noise is null, skipping");
        return;
    }
    //Log the decorator parameters so they can be verified at a glance while tuning.
    //打印装饰器参数，便于调参时快速核对。
    LogCat::d(LogLabel::BIOME_DECORATOR, "loot_chest_decorator_start",
              "Loot chest decorator start: chunk=({}, {}), seedOffset={}, noiseType={}, frequency={:.3f}, "
              "threshold={:.3f}, limit={}",
              chunkVertex.x, chunkVertex.y, decoratorResource->seedOffset, decoratorResource->noiseType,
              decoratorResource->frequency, decoratorResource->spawnThreshold, decoratorResource->limit);
    const int limit = decoratorResource->limit;
    int number = 0;
    for (int localY = 0; localY < CHUNK_SIZE && number < limit; ++localY) {
        for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
            const size_t idx = CoordinateTransformer::GetArrayIndex(localX, localY, CHUNK_SHIFT);
            const TileVector2D absolutePosition = CoordinateTransformer::ChunkRelativeToTile(
                chunkVertex, ChunkRelativeVector2D(localX, localY));
            const TerrainRelativeVector2D terrainRelative = CoordinateTransformer::TileToTerrainRelative(
                absolutePosition);
            const TerrainTileResult &selfTerrainTileResult = terrainResult->QueryTerrain(terrainRelative);
            if (const BiomeResource *selfBiomeResource = selfTerrainTileResult.GetBiomeResource();
                selfBiomeResource != biomeResource) {
                continue;
            }
            //Elevation filter. 海拔过滤。
            if (dimensionResource != nullptr) {
                const float elevation = TerrainMath::GetElevation(dimensionResource, absolutePosition.y);
                if (elevation < decoratorResource->minSpawnElevation ||
                    elevation > decoratorResource->maxSpawnElevation) {
                    continue;
                }
            }
            //Optionally require open air above so the chest is not buried.
            //可选：要求上方为空气，避免箱子被埋入地下。
            if (decoratorResource->requireSurface) {
                //The cave biome is solid rock hollowed out by the air-mineral decorator, so
                //"open air above" must be judged against the tiles already written into
                //tilesRefMap (solid floor + air above), not the raw solid terrain.
                //洞穴生物群系是实心岩石被空气矿物装饰器挖空而成，因此"上方露天"应依据
                //tilesRefMap 里已写入的实际瓦片（实心底板 + 上方空气）判断，而非原始实心地形。
                if (localY + 1 >= CHUNK_SIZE) {
                    //The cell above lies in the neighbouring chunk, absent from tilesRefMap.
                    //上方格子位于相邻区块，tilesRefMap 中不存在，跳过。
                    continue;
                }
                const size_t upIdx = CoordinateTransformer::GetArrayIndex(localX, localY + 1, CHUNK_SHIFT);
                //Require air above...
                //要求上方为空气……
                if (targetLayer[upIdx].GetResourceKey() != TILE_ID_AIR) {
                    continue;
                }
                //...and a solid cell under the chest (not inside the hollowed-out air).
                //……且箱子下方为实心（不落在被挖空的空气里）。
                if (targetLayer[idx].GetResourceKey() == TILE_ID_AIR) {
                    continue;
                }
            }
            //Sample the noise field and normalize to [0,1]; place a chest above the threshold.
            //采样噪声场并归一化到 [0,1]，超过阈值则放置箱子。
            const float n = (noise->GetNoise(
                                 static_cast<float>(absolutePosition.x),
                                 static_cast<float>(absolutePosition.y)) + 1.0F) * 0.5F;
            if (n > decoratorResource->spawnThreshold) {
                targetLayer[idx] = decoratorResource->chestTile;
                ++number;
                //Log each placed chest with its absolute position and the sampled noise value.
                //记录每个箱子放置的绝对坐标与采样噪声值。
                LogCat::d(LogLabel::BIOME_DECORATOR, "loot_chest_decorator_place",
                          "Loot chest decorator placing chest at ({}, {}), noise={:.3f}, count={}",
                          absolutePosition.x, absolutePosition.y, n, number);
                if (number >= limit) {
                    break;
                }
            }
        }
    }

    //Summarize how many chests were actually placed in this chunk.
    //汇总本区块实际放置了多少个箱子。
    LogCat::d(LogLabel::BIOME_DECORATOR, "loot_chest_decorator_placed",
              "Loot chest decorator placed {} chests in chunk ({}, {})", number, chunkVertex.x, chunkVertex.y);
}

glimmer::BiomeDecoratorType glimmer::LootChestBiomeDecorator::GetBiomeDecoratorType() {
    return BiomeDecoratorType::LOOT_CHEST;
}

glimmer::BiomeDecoratorPass glimmer::LootChestBiomeDecorator::GetPass() {
    return BiomeDecoratorPass::ContentPlacing;
}
