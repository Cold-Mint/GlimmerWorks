//
// Created by Cold-Mint on 2026/2/22.
//

#include "HeightStructureConditionProcessor.h"

#include "core/log/LogCat.h"

std::string glimmer::HeightStructureConditionProcessor::GetName() {
    return STRUCTURE_PLACEMENT_CONDITIONS_HEIGHT;
}

std::bitset<CHUNK_AREA> glimmer::HeightStructureConditionProcessor::Match(TerrainResult *terrainResult,
                                                                          const VariableConfig &variableConfig) {
    LogCat::d("HeightCondition", "Start height structure condition matching");
    std::bitset<CHUNK_AREA> result;
    const VariableDefinition *maxHeightPercentVariable = variableConfig.FindVariable("maxHeightPercent");
    float maxHeightPercent = 0.0F;
    if (maxHeightPercentVariable != nullptr) {
        maxHeightPercent = maxHeightPercentVariable->AsFloat();
        LogCat::d("HeightCondition", "Read maxHeightPercent from config:", maxHeightPercent);
    } else {
        LogCat::w("HeightCondition", "maxHeightPercent variable not found in config, use default value: 0.0");
    }
    const VariableDefinition *minHeightPercentVariable = variableConfig.FindVariable("minHeightPercent");
    float minHeightPercent = 0.0F;
    if (minHeightPercentVariable != nullptr) {
        minHeightPercent = minHeightPercentVariable->AsFloat();
        LogCat::d("HeightCondition", "Read minHeightPercent from config:", minHeightPercent);
    } else {
        LogCat::w("HeightCondition", "minHeightPercent variable not found in config, use default value: 0.0");
    }
    LogCat::d("HeightCondition", "Height percent range: min=", minHeightPercent, ", max=", maxHeightPercent,
              ", WORLD_MAX_Y: ", WORLD_MAX_Y);

    TileVector2D position = terrainResult->GetPosition();
    LogCat::d("HeightCondition", "Terrain position (x: ", position.x, ", y: ", position.y, "). Start checking ",
              CHUNK_SIZE, " x ", CHUNK_SIZE, " tiles");

    int matchedTileCount = 0;
    for (int localX = 0; localX < CHUNK_SIZE; localX++) {
        for (int localY = 0; localY < CHUNK_SIZE; localY++) {
            int globalY = localY + position.y;
            float percent = static_cast<float>(globalY) / WORLD_MAX_Y;
            LogCat::d("HeightCondition", "Tile [", localX, ", ", localY, "] percent:", percent);
            bool isInRange = percent >= minHeightPercent && percent <= maxHeightPercent;
            if (isInRange) {
                int tileIndex = localX + localY * CHUNK_SIZE;
                result[tileIndex] = true;
                matchedTileCount++;
                LogCat::d("HeightCondition",
                          "Tile (", localX, ", ", localY, ") matched. globalY=", globalY, ", percent=", percent,
                          " (in [", minHeightPercent, ", ", maxHeightPercent, "]), index=", tileIndex);
            }
        }
    }
    LogCat::d("HeightCondition", "Finish height condition matching. Total matched tiles: ", matchedTileCount,
              ", Set bits in result: ", result.count());

    return result;
}
