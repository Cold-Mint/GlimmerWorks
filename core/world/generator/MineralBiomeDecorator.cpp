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
#include "MineralBiomeDecorator.h"

#include <FastNoiseLite.h>

#include "ChunkGenerator.h"


void glimmer::MineralBiomeDecorator::DecorationImp(WorldContext *worldContext, TerrainResult *terrainResult,
                                                   MineralBiomeDecoratorResource *decoratorResource,
                                                   BiomeResource *biomeResource,
                                                   std::unordered_map<TileLayerType, std::array<ResourceRef,
                                                       CHUNK_AREA> > *tilesRefMap) {
    const FastNoiseLite *noiseLite = decoratorResource->GetFastNoiseLite(worldSeed_);
    if (noiseLite == nullptr) {
        return;
    }
    const float heightRange = decoratorResource->maxSpawnElevation - decoratorResource->minSpawnElevation;
    if (heightRange <= 0.0001F) {
        return;
    }
    std::array<ResourceRef, CHUNK_AREA> &targetLayer = tilesRefMap->at(
        static_cast<TileLayerType>(decoratorResource->layerType));
    for (int localX = 0; localX < CHUNK_SIZE; localX++) {
        for (int localY = 0; localY < CHUNK_SIZE; localY++) {
            const int idx = localY * CHUNK_SIZE + localX;
            const Vector2DI absolutePosition = terrainResult->GetPosition() + Vector2DI(localX, localY);
            float elevation = ChunkGenerator::GetElevation(absolutePosition.y);
            if (elevation > decoratorResource->maxSpawnElevation || elevation < decoratorResource->minSpawnElevation) {
                continue;
            }
            const TerrainTileResult &self = terrainResult->QueryTerrain(localX, localY);
            if (self.terrainType != SOLID || self.biomeResource != biomeResource) {
                continue;
            }
            const float normalizedOreNoise = (noiseLite->GetNoise(
                                                  static_cast<float>(absolutePosition.x),
                                                  static_cast<float>(absolutePosition.y)
                                              ) + 1.0F) * 0.5F;
            float currentSpawnThreshold;
            const float elevationRatio = (elevation - decoratorResource->minSpawnElevation) / heightRange;
            if (decoratorResource->invertOreSpawnByDepth) {
                // Activate inversion: The deeper you go, the more ore there is.
                // 开启反转：越向下，矿石越多
                currentSpawnThreshold = std::lerp(
                    decoratorResource->oreSpawnMinNoiseThreshold,
                    decoratorResource->oreSpawnMaxNoiseThreshold,
                    elevationRatio
                );
            } else {
                // Closing inversion: The deeper you go, the less ore there is.
                // 关闭反转：越向下，矿石越少
                currentSpawnThreshold = std::lerp(
                    decoratorResource->oreSpawnMaxNoiseThreshold,
                    decoratorResource->oreSpawnMinNoiseThreshold,
                    elevationRatio
                );
            }

            if (normalizedOreNoise > currentSpawnThreshold) {
                targetLayer[idx] = decoratorResource->ore;
            }
        }
    }
}

glimmer::BiomeDecoratorType glimmer::MineralBiomeDecorator::GetBiomeDecoratorType() {
    return BiomeDecoratorType::MINERAL;
}
