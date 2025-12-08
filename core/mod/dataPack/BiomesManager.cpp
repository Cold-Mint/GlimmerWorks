//
// Created by coldmint on 2025/12/8.
//

#include "BiomesManager.h"

#include "../../log/LogCat.h"
#include "../../math/Vector2DI.h"

void glimmer::BiomesManager::RegisterResource(BiomeResource &biomeResource) {
    LogCat::i("Registering biome resource: packId = ", biomeResource.packId,
              ", key = ", biomeResource.key);
    biomeMap_[biomeResource.packId][biomeResource.key] = biomeResource;
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


glimmer::TileResource * glimmer::BiomesManager::GetTileResource(TileVector2D tileVector2d, float height, float humidity,
    float temperature) {

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
