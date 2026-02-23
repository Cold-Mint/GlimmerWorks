//
// Created by coldmint on 2026/2/23.
//

#include "HorizontalSpacingStructureConditionProcessor.h"

std::string glimmer::HorizontalSpacingStructureConditionProcessor::GetName() {
    return STRUCTURE_PLACEMENT_CONDITIONS_HORIZONTAL_SPACING;
}

std::bitset<CHUNK_AREA> glimmer::HorizontalSpacingStructureConditionProcessor::Match(TerrainResult *terrainResult,
    const VariableConfig &variableConfig) {
    int minDistance = 4;
    const VariableDefinition *minDistanceVariable = variableConfig.FindVariable("minDistance");
    if (minDistanceVariable != nullptr) {
        minDistance = minDistanceVariable->AsInt();
    }
    if (minDistance <= 0) {
        minDistance = 4;
    }
    std::bitset<CHUNK_AREA> result;
    for (int y = 0; y < CHUNK_SIZE; ++y) {
        int globalX0 = terrainResult->GetPosition().x;
        int remainder = globalX0 % minDistance;
        if (remainder < 0) {
            //Fix negative numbers
            //修复负数
            remainder += minDistance;
        }
        int x0 = remainder == 0 ? 0 : minDistance - remainder;
        for (int x = x0; x < CHUNK_SIZE; x += minDistance) {
            result.set(y * CHUNK_SIZE + x);
        }
    }
    return result;
}
