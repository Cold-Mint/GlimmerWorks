//
// Created by Cold-Mint on 2025/12/8.
//

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

    if (bestBiome != nullptr) {
        LogCat::d("return bestBiome = ", Resource::GenerateId(bestBiome->packId, bestBiome->resourceId));
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
