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
#include "SpacingStructureConditionProcessor.h"

glimmer::StructureConditionProcessorType glimmer::SpacingStructureConditionProcessor::
GetStructureConditionProcessorType() {
    return StructureConditionProcessorType::Spacing;
}

std::bitset<CHUNK_AREA> glimmer::SpacingStructureConditionProcessor::Match(const TerrainResult *terrainResult,
                                                                           const IStructurePlacementConditionsResource *
                                                                           placementConditionsResource) {
    const auto spacingStructureConditions = dynamic_cast<const SpacingStructureConditionsResource *>(
        placementConditionsResource);
    int minDistance = spacingStructureConditions->minDistance;
    if (minDistance <= 0) {
        minDistance = 4;
    }
    std::bitset<CHUNK_AREA> result;
    if (spacingStructureConditions->isVertical) {
        for (int y = 0; y < CHUNK_SIZE; ++y) {
            int globalY = y + terrainResult->GetPosition().y;
            if (globalY % minDistance == 0) {
                for (int x = 0; x < CHUNK_SIZE; ++x) {
                    result.set(y * CHUNK_SIZE + x);
                }
            }
        }
    } else {
        for (int x = 0; x < CHUNK_SIZE; ++x) {
            int globalX = x + terrainResult->GetPosition().x;
            if (globalX % minDistance == 0) {
                for (int y = 0; y < CHUNK_SIZE; ++y) {
                    result.set(y * CHUNK_SIZE + x);
                }
            }
        }
    }
    return result;
}
