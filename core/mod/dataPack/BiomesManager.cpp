//
// Created by Cold-Mint on 2025/12/8.
//

#include "BiomesManager.h"

#include "../../log/LogCat.h"
#include "../../math/Vector2DI.h"

void glimmer::BiomesManager::RegisterResource(BiomeResource &biomeResource) {
    LogCat::i("Registering biome resource: packId = ", biomeResource.packId,
              ", key = ", biomeResource.key);
    biomeMap_[biomeResource.packId][biomeResource.key] = biomeResource;
    biomeVector_.push_back(biomeResource);
}

glimmer::BiomeResource *glimmer::BiomesManager::Find(const std::string &packId, const std::string &key) {
    LogCat::d("Searching for biome resource: packId = ", packId, ", key = ", key);
    const auto packIt = biomeMap_.find(packId);
    if (packIt == biomeMap_.end()) {
        LogCat::w("Pack not found: ", packId);
        return nullptr;
    }

    auto &keyMap = packIt->second;
    const auto keyIt = keyMap.find(key);
    if (keyIt == keyMap.end()) {
        LogCat::w("Key not found in pack ", packId, ": ", key);
        return nullptr;
    }

    LogCat::i("Found biome resource: packId = ", packId, ", key = ", key);
    return &keyIt->second;
}


glimmer::BiomeResource *glimmer::BiomesManager::FindBestBiome(
    const float humidity,
    const float temperature,
    const float weirdness,
    const float erosion,
    const float elevation) {
    if (biomeVector_.empty()) {
        return nullptr;
    }

    BiomeResource *bestBiome = nullptr;
    float bestDistance = std::numeric_limits<float>::max();

    for (auto &biome: biomeVector_) {
        const float dh = biome.humidity - humidity;
        const float dt = biome.temperature - temperature;
        const float dw = biome.weirdness - weirdness;
        const float de = biome.erosion - erosion;
        const float dE = biome.elevation - elevation;

        const float distance = dh * dh + dt * dt + dw * dw + de * de + dE * dE;
        if (distance < bestDistance) {
            bestDistance = distance;
            bestBiome = &biome;
        }
    }

    return bestBiome;
}


std::string glimmer::BiomesManager::ListTiles() const {
    std::string result;
    for (const auto &packPair: biomeMap_) {
        const auto &packId = packPair.first;
        const auto &keyMap = packPair.second;

        for (const auto &keyPair: keyMap) {
            const auto &key = keyPair.first;
            result += packId;
            result += ":";
            result += key;
            result += "\n";
        }
    }

    return result;
}
