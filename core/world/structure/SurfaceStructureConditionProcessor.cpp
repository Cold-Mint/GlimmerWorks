//
// Created by coldmint on 2026/2/22.
//

#include "SurfaceStructureConditionProcessor.h"

std::string glimmer::SurfaceStructureConditionProcessor::GetName() {
    return STRUCTURE_PLACEMENT_CONDITIONS_SURFACE;
}

std::bitset<CHUNK_AREA> glimmer::SurfaceStructureConditionProcessor::Match(TerrainResult *terrainResult,
    const VariableConfig &variableConfig) {
    std::bitset<CHUNK_AREA> result;
    for (int localX = 0; localX < CHUNK_SIZE; localX++) {
        for (int localY = 0; localY < CHUNK_SIZE; localY++) {
            TerrainTileResult self = terrainResult->QueryTerrain(localX, localY);
            if (self.terrainType != SOLID) {
                //Not solid tiles.
                //不是固体瓦片。
                continue;
            }
            TerrainTileResult up = terrainResult->QueryTerrain(localX, localY + 1);
            if (up.terrainType != AIR) {
                // The tiles above are not air.
                //上方的瓦片不是空气。
                continue;
            }
            result[localY * CHUNK_SIZE + localX] = true;
        }
    }
    return result;
}
