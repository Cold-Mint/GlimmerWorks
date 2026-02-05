//
// Created by Cold-Mint on 2025/12/11.
//

#include "FillBiomeDecorator.h"


void glimmer::FillBiomeDecorator::Decoration(WorldContext *worldContext, TerrainResult *terrainResult,
                                             BiomeDecoratorResource *biomeDecoratorResource,
                                             BiomeResource *biomeResource,
                                             std::array<std::array<ResourceRef, CHUNK_SIZE>, CHUNK_SIZE> &tilesRef) {
    const ResourceRef &resourceRef = biomeDecoratorResource->tiles[0];
    for (int localX = 0; localX < CHUNK_SIZE; localX++) {
        for (int localY = 0; localY < CHUNK_SIZE; localY++) {
            TerrainTileResult self = terrainResult->QueryTerrain(localX, localY);
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
            tilesRef[localX][localY] = resourceRef;
        }
    }
}

std::string glimmer::FillBiomeDecorator::GetId() {
    return FULL_BIOME_DECORATOR;
}
