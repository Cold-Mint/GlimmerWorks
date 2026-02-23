//
// Created by coldmint on 2026/2/22.
//

#include "BiomeStructureConditionProcessor.h"

#include "core/log/LogCat.h"

std::string glimmer::BiomeStructureConditionProcessor::GetName() {
    return STRUCTURE_PLACEMENT_CONDITIONS_BIOME;
}

std::bitset<CHUNK_AREA> glimmer::BiomeStructureConditionProcessor::Match(TerrainResult *terrainResult,
                                                                         const VariableConfig &variableConfig) {
    LogCat::d("BiomeCondition", "Start biome structure condition matching");
    const size_t size = variableConfig.definition.size();
    std::set<std::string> biomeSet;
    LogCat::d("BiomeCondition", "Building biome set. Total definitions in config:", size);
    for (int i = 0; i < size; i++) {
        std::string varKey = "biome_" + std::to_string(i);
        const VariableDefinition *definition = variableConfig.FindVariable(varKey);
        LogCat::d("BiomeCondition", "Checking variable definition [", i, "], key: ", varKey);
        if (definition == nullptr) {
            LogCat::w("BiomeCondition", "Variable definition ", i, " not found, key: ", varKey,
                      ". Break biome set building");
            break;
        }

        if (definition->type == REF) {
            ResourceRef resourceRef;
            definition->AsResourceRef(resourceRef);
            std::string biomeId = Resource::GenerateId(resourceRef.GetPackageId(), resourceRef.GetResourceKey());
            LogCat::d("BiomeCondition", "Add biome to set. PackageId: ", resourceRef.GetPackageId(), ", ResourceKey: ",
                      resourceRef.GetResourceKey(), ", Generated biome ID: ", biomeId);

            biomeSet.insert(biomeId);
        } else {
            LogCat::w("BiomeCondition", "Variable definition ", i, " type is not REF (type: ", definition->type,
                      "). Break biome set building");
            break;
        }
    }
    LogCat::d("BiomeCondition", "Finish building biome set. Total biomes in set: ", biomeSet.size(), ", is empty: ",
              biomeSet.empty() ? "true" : "false");

    std::bitset<CHUNK_AREA> result;
    if (biomeSet.empty()) {
        LogCat::w("BiomeCondition", "Biome set is empty, return empty match result");
        return result;
    }
    LogCat::d("BiomeCondition", "Start checking terrain tiles (CHUNK_SIZE: ", CHUNK_SIZE, "). Total tiles to check: ",
              CHUNK_AREA);

    int matchedTileCount = 0;
    for (int localX = 0; localX < CHUNK_SIZE; localX++) {
        for (int localY = 0; localY < CHUNK_SIZE; localY++) {
            const TerrainTileResult self = terrainResult->QueryTerrain(localX, localY);
            if (self.terrainType != SOLID) {
                continue;
            }

            if (self.biomeResource == nullptr) {
                LogCat::w("BiomeCondition", "Tile (", localX, ", ", localY, ") has no biome resource, skip");
                continue;
            }

            std::string tileBiomeId = Resource::GenerateId(*self.biomeResource);
            if (biomeSet.contains(tileBiomeId)) {
                int tileIndex = localY * CHUNK_SIZE + localX;
                result[tileIndex] = true;
                matchedTileCount++;
            }
        }
    }
    LogCat::d("BiomeCondition", "Finish biome condition matching. Total matched tiles: ", matchedTileCount,
              ", Total set bits: ", result.count());

    return result;
}
