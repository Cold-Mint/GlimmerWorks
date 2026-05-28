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
