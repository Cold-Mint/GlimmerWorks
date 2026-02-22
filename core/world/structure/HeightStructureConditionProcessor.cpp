//
// Created by coldmint on 2026/2/22.
//

#include "HeightStructureConditionProcessor.h"

std::string glimmer::HeightStructureConditionProcessor::GetName() {
    return STRUCTURE_PLACEMENT_CONDITIONS_HEIGHT;
}

std::bitset<CHUNK_AREA> glimmer::HeightStructureConditionProcessor::Match(TerrainResult *terrainResult,
    const VariableConfig &variableConfig) {
    std::bitset<CHUNK_AREA> result;
    float maxHeightPercent = variableConfig.FindVariable("maxHeightPercent")->AsFloat();
    float minHeightPercent = variableConfig.FindVariable("minHeightPercent")->AsFloat();
    TileVector2D position = terrainResult->GetPosition();
    for (int localX = 0; localX < CHUNK_SIZE; localX++) {
        for (int localY = 0; localY < CHUNK_SIZE; localY++) {
            int y = localY + position.y;
            float percent = y / WORLD_MAX_Y;
            if (percent >= minHeightPercent && percent <= maxHeightPercent) {
                result[localX + localY * CHUNK_SIZE] = true;
            }
        }
    }
    return result;
}
