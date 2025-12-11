//
// Created by Cold-Mint on 2025/12/3.
//

#include "TileManager.h"

#include "../../Constants.h"
#include "../../log/LogCat.h"


void glimmer::TileManager::InitBuiltinTiles() {
    air = std::make_unique<TileResource>();
    air->texture = "tiles/air.png";
    air->key = TILE_ID_AIR;
    air->physicsType = 0;
}

glimmer::TileResource *glimmer::TileManager::GetAir() const {
    return air.get();
}

void glimmer::TileManager::RegisterResource(TileResource &tileResource) {
    LogCat::i("Registering tile resource: packId = ", tileResource.packId,
              ", key = ", tileResource.key, "texture = ", tileResource.texture);
    tileMap_[tileResource.packId][tileResource.key] = tileResource;
}

glimmer::TileResource *glimmer::TileManager::Find(const std::string &packId, const std::string &key) {
    LogCat::d("Searching for tile resource: packId = ", packId, ", key = ", key);
    const auto packIt = tileMap_.find(packId);
    if (packIt == tileMap_.end()) {
        LogCat::w("Pack not found: ", packId);
        return nullptr;
    }

    auto &keyMap = packIt->second;
    const auto keyIt = keyMap.find(key);
    if (keyIt == keyMap.end()) {
        LogCat::w("Key not found in pack ", packId, ": ", key);
        return nullptr;
    }

    LogCat::i("Found tile resource: packId = ", packId, ", key = ", key);
    return &keyIt->second;
}

std::string glimmer::TileManager::ListTiles() const {
    std::string result;
    for (const auto &packPair: tileMap_) {
        const auto &packId = packPair.first;
        const auto &keyMap = packPair.second;

        for (const auto &keyPair: keyMap) {
            const auto &key = keyPair.first;
            const auto &res = keyPair.second; // StringResource

            result += packId;
            result += ":";
            result += key;
            result += " texture=";
            result += res.texture;
            result += "\n";
        }
    }

    return result;
}
