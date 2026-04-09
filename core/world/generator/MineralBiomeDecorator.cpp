//
// Created by coldmint on 2026/4/6.
//

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
