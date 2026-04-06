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


glimmer::BiomeResource *glimmer::BiomesManager::FindBestBiome(
    const float humidity,
    const float temperature,
    const float weirdness,
    const float erosion,
    const float elevation) const {
    if (biomeVector_.empty()) {
        return nullptr;
    }

    BiomeResource *bestBiome = nullptr;
    float bestDistance = std::numeric_limits<float>::max();

    for (auto &biome: biomeVector_) {
        // Compute terrain-biome attribute difference
        // 计算地形属性与生物群系属性的差值
        const float dh = biome->humidity - humidity;
        const float dt = biome->temperature - temperature;
        const float dw = biome->weirdness - weirdness;
        const float de = biome->erosion - erosion;
        const float dE = biome->elevation - elevation;

        // Core: Strictness weighted calculation (attribute difference × corresponding strictness)
        // Higher strictness = greater penalty for attribute mismatch, making selection less likely
        // 核心：严格度加权计算（属性差值 × 对应严格度）
        // 严格度越高，属性不匹配的惩罚越大，越难被选中
        const float distance =
                dh * dh * biome->strictnessHumidity +
                dt * dt * biome->strictnessTemperature +
                dw * dw * biome->strictnessWeirdness +
                de * de * biome->strictnessErosion +
                dE * dE * biome->strictnessElevation;

        // Find the biome with the smallest matching distance
        // 匹配距离最小的生物群系
        if (distance < bestDistance) {
            bestDistance = distance;
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
