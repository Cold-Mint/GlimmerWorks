//
// Created by coldmint on 2026/2/2.
//

#include "SurfaceBiomeDecorator.h"

#include "core/world/WorldContext.h"

void glimmer::SurfaceBiomeDecorator::Decoration(WorldContext *worldContext, TerrainResult *terrainResult,
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
            TerrainTileResult up = terrainResult->QueryTerrain(localX, localY + 1);
            if (up.terrainType != AIR) {
                // The tiles above are not air.
                //上方的瓦片不是空气。
                continue;
            }
            tilesRef[localX][localY] = resourceRef;
        }
    }
}

std::string glimmer::SurfaceBiomeDecorator::GetId() {
    return SURFACE_BIOME_DECORATOR;
}
