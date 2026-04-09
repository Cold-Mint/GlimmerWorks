//
// Created by Cold-Mint on 2025/12/11.
//

#include "FillBiomeDecorator.h"


void glimmer::FillBiomeDecorator::DecorationImp(WorldContext *worldContext, TerrainResult *terrainResult,
                                                FillBiomeDecoratorResource *decoratorResource,
                                                BiomeResource *biomeResource,
                                                std::unordered_map<TileLayerType, std::array<ResourceRef, CHUNK_AREA> >
                                                *tilesRefMap) {
    std::array<ResourceRef, CHUNK_AREA> &targetLayer = tilesRefMap->at(
        static_cast<TileLayerType>(decoratorResource->layerType));
    const ResourceRef &resourceRef = decoratorResource->tile;
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
            targetLayer[idx] = resourceRef;
        }
    }
}

glimmer::BiomeDecoratorType glimmer::FillBiomeDecorator::GetBiomeDecoratorType() {
    return BiomeDecoratorType::FILL;
}
