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
#include "BiomesManager.h"

#include "../../log/LogCat.h"

glimmer::BiomeResource *glimmer::BiomesManager::AddResource(std::unique_ptr<BiomeResource> biomeResource) {
    LogCat::i("Registering biome resource: packId = ", biomeResource->packId,
              ", resourceId = ", biomeResource->resourceId);
    auto &slot = biomeMap_[biomeResource->packId][biomeResource->resourceId];
    slot = std::move(biomeResource);
    biomeVector_.push_back(slot.get());
    return slot.get();
}

glimmer::BiomeResource *glimmer::BiomesManager::Find(const std::string &packId, const std::string &resourceId) {
    LogCat::d("Searching for biome resource: packId = ", packId, ", resourceId = ", resourceId);
    const auto packIt = biomeMap_.find(packId);
    if (packIt == biomeMap_.end()) {
        LogCat::w("Pack not found: ", packId);
        return nullptr;
    }

    auto &keyMap = packIt->second;
    const auto keyIt = keyMap.find(resourceId);
    if (keyIt == keyMap.end()) {
        LogCat::w("Key not found in pack ", packId, ": ", resourceId);
        return nullptr;
    }

    LogCat::i("Found biome resource: packId = ", packId, ", resourceId = ", resourceId);
    return keyIt->second.get();
}

float glimmer::BiomesManager::CalculateBiomeScoreDelta(const float targetValue, const float actualValue,
                                                       const float strictness) {
    const float diff = targetValue - actualValue;
    return diff * diff * strictness;
}

const std::vector<glimmer::BiomeResource *> &glimmer::BiomesManager::GetBiomeVector() {
    return biomeVector_;
}

glimmer::BiomeResource *glimmer::BiomesManager::FindBestBiome(const float humidity, const float temperature,
                                                              const float weirdness,
                                                              const float erosion, const float elevation,
                                                              const float surfaceProximity) const {
    if (biomeVector_.empty()) {
        return nullptr;
    }

    BiomeResource *bestBiome = nullptr;
    float bestDistance = std::numeric_limits<float>::max();

    for (auto &biome: biomeVector_) {
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

std::vector<std::string> glimmer::BiomesManager::GetBiomeList() const {
    std::vector<std::string> result;
    for (const auto &packPair: biomeMap_) {
        const auto &packId = packPair.first;
        const auto &keyMap = packPair.second;

        for (const auto &keyPair: keyMap) {
            const auto &key = keyPair;
            result.emplace_back(Resource::GenerateId(packId, key.first));
        }
    }
    return result;
}

std::string glimmer::BiomesManager::ListBiomes() const {
    std::ostringstream oss;
    for (const auto &packPair: biomeMap_) {
        const auto &packId = packPair.first;
        const auto &keyMap = packPair.second;
        for (const auto &keyPair: keyMap) {
            const auto &key = keyPair.first;
            oss << Resource::GenerateId(packId, key) << '\n';
        }
    }
    return oss.str();
}
