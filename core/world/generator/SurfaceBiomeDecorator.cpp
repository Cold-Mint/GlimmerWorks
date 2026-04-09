//
// Created by Cold-Mint on 2026/2/2.
//

#include "SurfaceBiomeDecorator.h"

#include "core/world/WorldContext.h"


void glimmer::SurfaceBiomeDecorator::DecorationImp(WorldContext *worldContext, TerrainResult *terrainResult,
                                                   SurfaceBiomeDecoratorResource *decoratorResource,
                                                   BiomeResource *biomeResource,
                                                   std::unordered_map<TileLayerType, std::array<ResourceRef,
                                                       CHUNK_AREA> > *tilesRefMap) {
    std::array<ResourceRef, CHUNK_AREA> &targetLayer = tilesRefMap->at(
        static_cast<TileLayerType>(decoratorResource->layerType));
    for (int localX = 0; localX < CHUNK_SIZE; localX++) {
        for (int localY = 0; localY < CHUNK_SIZE; localY++) {
            const int idx = localY * CHUNK_SIZE + localX;
            const TerrainTileResult &self = terrainResult->QueryTerrain(localX, localY);
            if (self.terrainType != SOLID) {
                //Not solid tiles.
                //不是固体瓦片。
                continue;
            }
            if (self.biomeResource != biomeResource) {
                //Tiles do not belong to the current biome.
                //瓦片不属于当前生物群系。
                continue;
            }
            const TerrainTileResult &up = terrainResult->QueryTerrain(localX, localY + 1);
            if (decoratorResource->allowAir && up.terrainType == AIR) {
                targetLayer[idx] = decoratorResource->tile;
                continue;
            }

            if (decoratorResource->allowWater && up.terrainType == WATER) {
                targetLayer[idx] = decoratorResource->tile;
            }
        }
    }
}

glimmer::BiomeDecoratorType glimmer::SurfaceBiomeDecorator::GetBiomeDecoratorType() {
    return BiomeDecoratorType::SURFACE;
}
