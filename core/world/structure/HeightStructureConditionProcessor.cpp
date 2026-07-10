/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "HeightStructureConditionProcessor.h"

#include "core/log/LogCat.h"

std::string glimmer::HeightStructureConditionProcessor::GetName() {
    return STRUCTURE_PLACEMENT_CONDITIONS_HEIGHT;
}

std::bitset<CHUNK_AREA> glimmer::HeightStructureConditionProcessor::Match(TerrainResult *terrainResult,
                                                                          const VariableConfig &variableConfig) {

    std::bitset<CHUNK_AREA> result;
    const VariableDefinition *maxHeightPercentVariable = variableConfig.FindVariable("maxHeightPercent");
    float maxHeightPercent = 0.0F;
    if (maxHeightPercentVariable != nullptr) {
        maxHeightPercent = maxHeightPercentVariable->AsFloat();

    } else {

    }
    const VariableDefinition *minHeightPercentVariable = variableConfig.FindVariable("minHeightPercent");
    float minHeightPercent = 0.0F;
    if (minHeightPercentVariable != nullptr) {
        minHeightPercent = minHeightPercentVariable->AsFloat();

    } else {

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
