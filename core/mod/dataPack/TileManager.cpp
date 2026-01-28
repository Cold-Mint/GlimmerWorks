//
// Created by Cold-Mint on 2025/12/3.
//

#include "TileManager.h"

#include "../../Constants.h"
#include "../../log/LogCat.h"
#include "../../world/TileLayerType.h"
#include "../../world/TilePhysicsType.h"


void glimmer::TileManager::InitBuiltinTiles() {
    air_ = std::make_unique<TileResource>();
    air_->texture = "tiles/air.png";
    air_->key = TILE_ID_AIR;
    air_->packId = RESOURCE_REF_CORE;
    air_->physicsType = static_cast<uint8_t>(TilePhysicsType::None);
    air_->layerType = static_cast<uint8_t>(TileLayerType::Main);
    air_->breakable = false;
    air_->hardness = 0;
    air_->errorPlaceholder = false;
    water_ = std::make_unique<TileResource>();
    water_->texture = "tiles/water.png";
    water_->key = TILE_ID_WATER;
    water_->packId = RESOURCE_REF_CORE;
    water_->physicsType = static_cast<uint8_t>(TilePhysicsType::None);
    water_->layerType = static_cast<uint8_t>(TileLayerType::Main);
    water_->breakable = false;
    water_->hardness = 0;
    water_->errorPlaceholder = false;
    bedrock_ = std::make_unique<TileResource>();
    bedrock_->texture = "tiles/bedrock.png";
    bedrock_->key = TILE_ID_BEDROCK;
    bedrock_->packId = RESOURCE_REF_CORE;
    bedrock_->physicsType = static_cast<uint8_t>(TilePhysicsType::Static);
    bedrock_->layerType = static_cast<uint8_t>(TileLayerType::Main);
    bedrock_->breakable = false;
    bedrock_->hardness = 0;
    bedrock_->errorPlaceholder = false;
    error_ = std::make_unique<TileResource>();
    error_->texture = ERROR_TEXTURE_PATH;
    error_->key = TILE_ID_ERROR;
    error_->packId = RESOURCE_REF_CORE;
    error_->physicsType = static_cast<uint8_t>(TilePhysicsType::Static);
    error_->layerType = static_cast<uint8_t>(TileLayerType::Main);
    error_->breakable = true;
    error_->hardness = 0.1F;
    error_->errorPlaceholder = false;
}

glimmer::TileResource *glimmer::TileManager::GetAir() const {
    return air_.get();
}

glimmer::TileResource *glimmer::TileManager::GetWater() const {
    return water_.get();
}

glimmer::TileResource *glimmer::TileManager::GetBedrock() const {
    return bedrock_.get();
}

glimmer::TileResource *glimmer::TileManager::GetError() const {
    return error_.get();
}

glimmer::TileResource *glimmer::TileManager::AddResource(std::unique_ptr<TileResource> tileResource) {
    LogCat::i("Registering tile resource: packId = ", tileResource->packId,
              ", key = ", tileResource->key, "texture = ", tileResource->texture);
    auto &slot = tileMap_[tileResource->packId][tileResource->key];
    slot = std::move(tileResource);
    return slot.get();
}

std::unique_ptr<glimmer::TileResource> glimmer::TileManager::GenerateErrorPlaceHolder(const std::string &packId,
    const std::string &key) {
    auto errorPlaceholder = std::make_unique<TileResource>();
    errorPlaceholder->texture = ERROR_TEXTURE_PATH;
    errorPlaceholder->key = key;
    errorPlaceholder->packId = packId;
    errorPlaceholder->physicsType = static_cast<uint8_t>(TilePhysicsType::Static);
    errorPlaceholder->layerType = static_cast<uint8_t>(TileLayerType::Main);
    errorPlaceholder->breakable = true;
    errorPlaceholder->hardness = 0.1F;
    errorPlaceholder->errorPlaceholder = true;
    return errorPlaceholder;
}


glimmer::TileResource *glimmer::TileManager::Find(const std::string &packId, const std::string &key) {
    LogCat::d("Searching for tile resource: packId = ", packId, ", key = ", key);
    const auto packIt = tileMap_.find(packId);
    if (packIt == tileMap_.end()) {
        LogCat::w("Pack not found: ", packId);
        return AddResource(GenerateErrorPlaceHolder(packId, key));
    }

    auto &keyMap = packIt->second;
    const auto keyIt = keyMap.find(key);
    if (keyIt == keyMap.end()) {
        LogCat::w("Key not found in pack ", packId, ": ", key);
        return AddResource(GenerateErrorPlaceHolder(packId, key));
    }

    LogCat::i("Found tile resource: packId = ", packId, ", key = ", key);
    return keyIt->second.get();
}

std::vector<std::string> glimmer::TileManager::GetTileIDList() {
    std::vector<std::string> result;
    result.emplace_back(Resource::GenerateId(air_->packId, air_->key));
    result.emplace_back(Resource::GenerateId(water_->packId, water_->key));
    result.emplace_back(Resource::GenerateId(bedrock_->packId, bedrock_->key));
    for (const auto &[packId, keyMap]: tileMap_) {
        for (const auto &[key, resource]: keyMap) {
            result.emplace_back(Resource::GenerateId(packId, key));
        }
    }

    return result;
}

std::string glimmer::TileManager::ListTiles() const {
    std::string result;
    for (const auto &packPair: tileMap_) {
        const auto &packId = packPair.first;
        const auto &keyMap = packPair.second;

        for (const auto &keyPair: keyMap) {
            const auto &key = keyPair.first;
            const auto &res = keyPair.second;
            result += Resource::GenerateId(packId, key);
            result += " texture=";
            result += res->texture;
            result += "\n";
        }
    }

    result += Resource::GenerateId(*error_);
    result += " texture=";
    result += error_->texture;
    result += "\n";
    result += Resource::GenerateId(*air_);
    result += " texture=";
    result += air_->texture;
    result += "\n";
    result += Resource::GenerateId(*water_);
    result += " texture=";
    result += water_->texture;
    result += "\n";
    result += Resource::GenerateId(*bedrock_);
    result += " texture=";
    result += bedrock_->texture;
    result += "\n";
    return result;
}
