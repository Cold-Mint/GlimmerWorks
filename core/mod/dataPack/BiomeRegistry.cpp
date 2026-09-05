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
#include "BiomeRegistry.h"


float glimmer::BiomeRegistry::CalculateBiomeScoreDelta(const float targetValue, const float actualValue,
                                                       const float strictness) {
    const float diff = targetValue - actualValue;
    return diff * diff * strictness;
}

std::span<glimmer::BiomeResource *> glimmer::BiomeRegistry::GetBiomeVector() {
    return biomeVector_;
}

void glimmer::BiomeRegistry::OnRegister(BiomeResource *resource) {
    biomeVector_.emplace_back(resource);
}

bool glimmer::BiomeRegistry::BelongsToDimension(const BiomeResource *biome, const std::string &dimensionId) {
    if (biome == nullptr) {
        return false;
    }
    if (biome->dimensions.empty()) {
        return true;
    }
    for (const auto &dimension: biome->dimensions) {
        if (Resource::GenerateId(dimension.GetPackageId(), dimension.GetResourceKey()) == dimensionId) {
            return true;
        }
    }
    return false;
}

glimmer::BiomeResource *glimmer::BiomeRegistry::FindBestBiome(const std::string &dimensionId,
                                                              const float humidity, const float temperature,
                                                              const float weirdness,
                                                              const float erosion, const float elevation,
                                                              const float surfaceProximity) const {
    if (biomeVector_.empty()) {
        return nullptr;
    }

    BiomeResource *bestBiome = nullptr;
    float bestDistance = std::numeric_limits<float>::max();

    for (auto &biome: biomeVector_) {
        if (!BelongsToDimension(biome, dimensionId)) {
            continue;
        }
        const float scoreHumidity = CalculateBiomeScoreDelta(biome->humidity, humidity, biome->strictnessHumidity);
        const float scoreTemperature = CalculateBiomeScoreDelta(biome->temperature, temperature,
                                                                biome->strictnessTemperature);
        const float scoreWeirdness = CalculateBiomeScoreDelta(biome->weirdness, weirdness, biome->strictnessWeirdness);
        const float scoreErosion = CalculateBiomeScoreDelta(biome->erosion, erosion, biome->strictnessErosion);
        const float scoreElevation = CalculateBiomeScoreDelta(biome->elevation, elevation, biome->strictnessElevation);
        const float scoreSurfaceProximity = CalculateBiomeScoreDelta(biome->surfaceProximity, surfaceProximity,
                                                                     biome->strictnessSurfaceProximity);
        const float totalDistance = scoreHumidity + scoreTemperature + scoreWeirdness + scoreErosion + scoreElevation +
                                    scoreSurfaceProximity;
        if (totalDistance < bestDistance) {
            bestDistance = totalDistance;
            bestBiome = biome;
        }
    }
    return bestBiome;
}
