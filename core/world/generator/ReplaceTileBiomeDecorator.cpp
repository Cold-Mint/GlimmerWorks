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
#include "ReplaceTileBiomeDecorator.h"

#include "core/math/CoordinateTransformer.h"


void glimmer::ReplaceTileBiomeDecorator::DecorationImpl(const ChunkVertexVector2D &chunkVertex,
                                                        WorldContext *worldContext, TerrainResult *terrainResult,
                                                        ReplaceTileBiomeDecoratorResource *decoratorResource,
                                                        BiomeResource *biomeResource,
                                                        std::unordered_map<TileLayerType, std::array<ResourceRef,
                                                            CHUNK_AREA> > *tilesRefMap) {
    if (decoratorResource == nullptr || tilesRefMap == nullptr) {
        return;
    }
    std::array<ResourceRef, CHUNK_AREA> &targetLayer = tilesRefMap->at(
        static_cast<TileLayerType>(decoratorResource->layer));
    for (int localX = 0; localX < CHUNK_SIZE; localX++) {
        for (int localY = 0; localY < CHUNK_SIZE; localY++) {
            const size_t idx = CoordinateTransformer::GetArrayIndex(localX, localY, CHUNK_SHIFT);
            ChunkRelativeVector2D chunkRelativeVector2D(localX, localY);
            const TileVector2D absolutePosition = CoordinateTransformer::ChunkRelativeToTile(
                chunkVertex, chunkRelativeVector2D);
            const TerrainRelativeVector2D terrainRelativeVector2D = CoordinateTransformer::TileToTerrainRelative(
                absolutePosition);
            //The current cell must belong to the current biome.
            //当前格子必须属于当前生物群系。
            const TerrainTileResult &selfTerrainResult = terrainResult->QueryTerrain(terrainRelativeVector2D);
            if (selfTerrainResult.GetTerrainType() != TerrainResultType::SOLID) {
                continue;
            }
            if (const BiomeResource *selfBiomeResource = selfTerrainResult.GetBiomeResource();
                selfBiomeResource != biomeResource) {
                continue;
            }
            for (const auto &rule: decoratorResource->rules) {
                if (!rule.targetTile.IsValid()) {
                    continue;
                }
                //至少需要一个源条件，否则视为无效规则（空操作）。
                if (!rule.sourceTile.IsValid() && rule.sourceTerrainMask == 0) {
                    continue;
                }
                const int detectionLocalX = localX + rule.offset.x;
                const int detectionLocalY = localY + rule.offset.y;
                const bool inChunk = detectionLocalX >= 0 && detectionLocalX < CHUNK_SIZE &&
                                     detectionLocalY >= 0 && detectionLocalY < CHUNK_SIZE;

                //条件一：地形掩码（原始地形，可跨区块）
                if (rule.sourceTerrainMask != 0) {
                    const TileVector2D detectionAbsolutePosition(absolutePosition.x + rule.offset.x,
                                                                 absolutePosition.y + rule.offset.y);
                    const TerrainRelativeVector2D detectionTerrainRelative =
                            CoordinateTransformer::TileToTerrainRelative(detectionAbsolutePosition);
                    const TerrainResultType detectionTerrainType =
                            terrainResult->QueryTerrain(detectionTerrainRelative).GetTerrainType();
                    const auto terrainBit = static_cast<uint8_t>(detectionTerrainType);
                    if ((rule.sourceTerrainMask & terrainBit) == 0) {
                        continue;
                    }
                }

                //条件二：源瓦片（tilesRefMap，仅区块内可验证）
                if (rule.sourceTile.IsValid()) {
                    if (!inChunk) {
                        continue;
                    }
                    const size_t detectionIdx = CoordinateTransformer::GetArrayIndex(
                        detectionLocalX, detectionLocalY, CHUNK_SHIFT);
                    if (targetLayer[detectionIdx] != rule.sourceTile) {
                        continue;
                    }
                }

                //所有已设置的源条件都匹配 → 放置。
                targetLayer[idx] = rule.targetTile;
                break;
            }
        }
    }
}

glimmer::BiomeDecoratorPass glimmer::ReplaceTileBiomeDecorator::GetPass() {
    return BiomeDecoratorPass::ContentPlacing;
}

glimmer::BiomeDecoratorType glimmer::ReplaceTileBiomeDecorator::GetBiomeDecoratorType() {
    return BiomeDecoratorType::REPLACE_TILE;
}
