//
// Created by coldmint on 2026/2/22.
//

#include "BiomeStructureConditionProcessor.h"

std::string glimmer::BiomeStructureConditionProcessor::GetName() {
    return STRUCTURE_PLACEMENT_CONDITIONS_BIOME;
}

std::bitset<CHUNK_AREA> glimmer::BiomeStructureConditionProcessor::Match(TerrainResult *terrainResult,
    const VariableConfig &variableConfig) {
    const size_t size = variableConfig.definition.size();
    std::set<std::string> biomeSet;
    for (int i = 0; i < size; i++) {
        const VariableDefinition *definition = variableConfig.FindVariable("biome_" + std::to_string(i));
        if (definition == nullptr) {
            break;
        }
        if (definition->type == STRING) {
            ResourceRef resourceRef;
            definition->AsResourceRef(resourceRef);
            biomeSet.insert(Resource::GenerateId(resourceRef.GetPackageId(), resourceRef.GetResourceKey()));
        } else {
            break;
        }
    }
    std::bitset<CHUNK_AREA> result;
    if (!biomeSet.empty()) {
        return result;
    }
    for (int localX = 0; localX < CHUNK_SIZE; localX++) {
        for (int localY = 0; localY < CHUNK_SIZE; localY++) {
            const TerrainTileResult self = terrainResult->QueryTerrain(localX, localY);
            if (self.terrainType != SOLID) {
                //Not solid tiles.
                //不是固体瓦片。
                continue;
            }
            if (biomeSet.contains(Resource::GenerateId(*self.biomeResource))) {
                result[localY * CHUNK_SIZE + localX] = true;
            }
        }
    }
    return result;
}
