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
#include "BiomeGrowthConditionProcessor.h"

#include "core/context/AppContext.h"
#include "core/context/ModContext.h"
#include "core/math/TileVector2D.h"
#include "core/mod/Resource.h"
#include "core/mod/dataPack/BiomeRegistry.h"
#include "core/world/WorldContext.h"
#include "core/world/generator/ChunkGenerator.h"
#include "core/world/generator/TerrainMath.h"

glimmer::GrowthConditionProcessorType
glimmer::BiomeGrowthConditionProcessor::GetGrowthConditionProcessorType() {
    return GrowthConditionProcessorType::Biome;
}

bool glimmer::BiomeGrowthConditionProcessor::Match(const WorldContext *worldContext, const TileVector2D &position,
                                                   const IGrowthConditionResource *growthConditionResource) {
    const auto biomeCondition = dynamic_cast<const BiomeGrowthConditionResource *>(growthConditionResource);
    if (biomeCondition == nullptr || worldContext == nullptr) {
        return false;
    }
    const std::unordered_set<std::string> &targetBiomeIds = biomeCondition->GetCachedBiomeIds();
    if (targetBiomeIds.empty()) {
        return false;
    }
    const AppContext *appContext = worldContext->GetAppContext();
    if (appContext == nullptr) {
        return false;
    }
    const ModContext *modContext = appContext->GetModContext();
    if (modContext == nullptr) {
        return false;
    }
    const BiomeRegistry *biomeRegistry = modContext->GetBiomeRegistry();
    if (biomeRegistry == nullptr) {
        return false;
    }
    ChunkGenerator *chunkGenerator = worldContext->GetChunkGenerator();
    if (chunkGenerator == nullptr) {
        return false;
    }
    const float elevation = TerrainMath::GetElevation(position.y);
    const BiomeResource *biomeResource = biomeRegistry->FindBestBiome(
        chunkGenerator->GetDimensionId(),
        chunkGenerator->GetHumidity(position),
        chunkGenerator->GetTemperature(position, elevation),
        chunkGenerator->GetWeirdness(position),
        chunkGenerator->GetErosion(position),
        elevation,
        TerrainMath::GetSurfaceProximity(chunkGenerator->GetFirstTileTerrainY(position.x), position.y));
    if (biomeResource == nullptr) {
        return false;
    }
    return targetBiomeIds.contains(Resource::GenerateId(*biomeResource));
}
