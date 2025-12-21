//
// Created by Cold-Mint on 2025/12/3.
//

#include "TileManager.h"

#include "../../Constants.h"
#include "../../log/LogCat.h"
#include "../../world/TileLayerType.h"
#include "../../world/TilePhysicsType.h"


void glimmer::TileManager::InitBuiltinTiles() {
    air = std::make_unique<TileResource>();
    air->texture = "tiles/air.png";
    air->key = TILE_ID_AIR;
    air->physicsType = static_cast<uint8_t>(TilePhysicsType::None);
    air->layerType = static_cast<uint8_t>(TileLayerType::Main);
    air->breakable = false;
    air->hardness = 0;
    water = std::make_unique<TileResource>();
    water->texture = "tiles/water.png";
    water->key = TILE_ID_WATER;
    water->physicsType = static_cast<uint8_t>(TilePhysicsType::None);
    water->layerType = static_cast<uint8_t>(TileLayerType::Main);
    water->breakable = false;
    water->hardness = 0;
    bedrock = std::make_unique<TileResource>();
    bedrock->texture = "tiles/bedrock.png";
    bedrock->key = TILE_ID_BEDROCK;
    bedrock->physicsType = static_cast<uint8_t>(TilePhysicsType::Static);
    bedrock->layerType = static_cast<uint8_t>(TileLayerType::Main);
    bedrock->breakable = false;
    bedrock->hardness = 0;
}

glimmer::TileResource *glimmer::TileManager::GetAir() const {
    return air.get();
}

glimmer::TileResource *glimmer::TileManager::GetWater() const {
    return water.get();
}

glimmer::TileResource *glimmer::TileManager::GetBedrock() const {
    return bedrock.get();
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
