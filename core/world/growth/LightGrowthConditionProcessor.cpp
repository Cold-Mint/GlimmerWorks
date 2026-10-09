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
#include "LightGrowthConditionProcessor.h"

#include <algorithm>
#include <unordered_set>
#include <utility>

#include "core/mod/Resource.h"
#include "core/world/ChunkManager.h"
#include "core/world/LightBuffer.h"
#include "core/world/WorldContext.h"

glimmer::GrowthConditionProcessorType
glimmer::LightGrowthConditionProcessor::GetGrowthConditionProcessorType() {
    return GrowthConditionProcessorType::Light;
}

bool glimmer::LightGrowthConditionProcessor::Match(const WorldContext *worldContext, const TileVector2D &position,
                                                   const IGrowthConditionResource *growthConditionResource) {
    const auto lightCondition = dynamic_cast<const LightGrowthConditionResource *>(growthConditionResource);
    if (lightCondition == nullptr || worldContext == nullptr) {
        return false;
    }
    const ChunkManager *chunkManager = worldContext->GetChunkManager();
    if (chunkManager == nullptr) {
        return false;
    }
    const LightBuffer *lightBuffer = chunkManager->GetLightingBuffer();
    if (lightBuffer == nullptr) {
        return false;
    }
    uint32_t intensity = 0;
    if (const TileLightData *tileLightData = lightBuffer->GetTileLightData(position); tileLightData != nullptr) {
        const auto *contributions = tileLightData->GetLightContributions();
        if (contributions != nullptr) {
            std::unordered_set<const LightSource *> seenSources;
            for (const auto &entry: *contributions) {
                for (const auto &contribution: entry.second) {
                    if (contribution == nullptr) {
                        continue;
                    }
                    const LightSource *source = contribution->GetLightSource();
                    if (source == nullptr) {
                        continue;
                    }
                    if ((lightCondition->lightSourceMask & std::to_underlying(source->GetDirection())) == 0) {
                        continue;
                    }
                    if (!seenSources.insert(source).second) {
                        continue;
                    }
                    if (const Color *color = contribution->GetLightColor(); color != nullptr) {
                        intensity += color->a;
                    }
                }
            }
        }
    }
    const uint8_t lightLevel = static_cast<uint8_t>(std::min<uint32_t>(intensity, 255));
    return lightLevel >= lightCondition->minLight && lightLevel <= lightCondition->maxLight;
}
