//
// Created by Cold-Mint on 2025/12/3.
//

#include "TileManager.h"

#include "../../Constants.h"
#include "../../log/LogCat.h"
#include "core/world/generator/TileLayerType.h"
#include "core/world/generator/TilePhysicsType.h"


void glimmer::TileManager::InitBuiltinTiles() {
    air_ = std::make_unique<TileResource>();
    ResourceRef airResource;
    airResource.SetSelfPackageId(RESOURCE_REF_CORE);
    airResource.SetResourceKey("tiles/air");
    airResource.SetResourceType(RESOURCE_TYPE_TEXTURES);
    air_->texture = airResource;
    air_->resourceId = TILE_ID_AIR;
    air_->packId = RESOURCE_REF_CORE;
    air_->physicsType = static_cast<uint8_t>(TilePhysicsType::None);
    air_->layerType = static_cast<uint8_t>(Ground);
    air_->hardness = -1.0F;
    air_->allowCrossLayerPlacement = true;
    airResourceRef_ = std::make_unique<ResourceRef>();
    airResourceRef_->SetResourceType(RESOURCE_TYPE_TILE);
    airResourceRef_->SetSelfPackageId(RESOURCE_REF_CORE);
    airResourceRef_->SetResourceKey(TILE_ID_AIR);
    water_ = std::make_unique<TileResource>();
    ResourceRef waterResource;
    waterResource.SetSelfPackageId(RESOURCE_REF_CORE);
    waterResource.SetResourceKey("tiles/water");
    waterResource.SetResourceType(RESOURCE_TYPE_TEXTURES);
    water_->texture = waterResource;
    water_->resourceId = TILE_ID_WATER;
    water_->packId = RESOURCE_REF_CORE;
    water_->physicsType = static_cast<uint8_t>(TilePhysicsType::None);
    water_->layerType = static_cast<uint8_t>(Ground);
    water_->hardness = -1.0F;
    bedrock_ = std::make_unique<TileResource>();
    ResourceRef bedrockResource;
    bedrockResource.SetSelfPackageId(RESOURCE_REF_CORE);
    bedrockResource.SetResourceKey("tiles/bedrock");
    bedrockResource.SetResourceType(RESOURCE_TYPE_TEXTURES);
    bedrock_->texture = bedrockResource;
    bedrock_->resourceId = TILE_ID_BEDROCK;
    bedrock_->packId = RESOURCE_REF_CORE;
    bedrock_->physicsType = static_cast<uint8_t>(TilePhysicsType::Static);
    bedrock_->layerType = static_cast<uint8_t>(Ground);
    bedrock_->hardness = -1.0F;
    error_ = std::make_unique<TileResource>();
    ResourceRef errorResource;
    errorResource.SetSelfPackageId(RESOURCE_REF_CORE);
    errorResource.SetResourceKey(ERROR_TEXTURE_KEY);
    errorResource.SetResourceType(RESOURCE_TYPE_TEXTURES);
    error_->texture = errorResource;
    error_->resourceId = TILE_ID_ERROR;
    error_->packId = RESOURCE_REF_CORE;
    error_->physicsType = static_cast<uint8_t>(TilePhysicsType::Static);
    error_->layerType = static_cast<uint8_t>(Ground);
    error_->hardness = 0.1F;
    error_->allowCrossLayerPlacement = true;
}

glimmer::TileResource *glimmer::TileManager::GetAir() const {
    return air_.get();
}

const glimmer::ResourceRef *glimmer::TileManager::GetAirResourceRef() const {
    return airResourceRef_.get();
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
              ", resourceId = ", tileResource->resourceId);
    auto &slot = tileMap_[tileResource->packId][tileResource->resourceId];
    slot = std::move(tileResource);
    return slot.get();
}

std::unique_ptr<glimmer::TileResource> glimmer::TileManager::GenerateErrorPlaceHolder(const std::string &packId,
    const std::string &resourceId) {
    auto errorPlaceholder = std::make_unique<TileResource>();
    ResourceRef errorResource;
    errorResource.SetSelfPackageId(RESOURCE_REF_CORE);
    errorResource.SetResourceKey(ERROR_TEXTURE_KEY);
    errorResource.SetResourceType(RESOURCE_TYPE_TEXTURES);
    errorPlaceholder->texture = errorResource;
    errorPlaceholder->resourceId = resourceId;
    errorPlaceholder->packId = packId;
    errorPlaceholder->physicsType = static_cast<uint8_t>(TilePhysicsType::Static);
    errorPlaceholder->layerType = static_cast<uint8_t>(Ground);
    errorPlaceholder->hardness = 0.1F;
    errorPlaceholder->missing = true;
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
    result.emplace_back(Resource::GenerateId(water_->packId, water_->resourceId));
    result.emplace_back(Resource::GenerateId(bedrock_->packId, bedrock_->resourceId));
    for (const auto &[packId, keyMap]: tileMap_) {
        for (const auto &[key, resource]: keyMap) {
            result.emplace_back(Resource::GenerateId(packId, key));
        }
    }

    return result;
}

std::string glimmer::TileManager::ListTiles() const {
    std::ostringstream oss;
    for (const auto &packPair: tileMap_) {
        const auto &packId = packPair.first;
        const auto &keyMap = packPair.second;

        for (const auto &keyPair: keyMap) {
            const auto &key = keyPair.first;
            oss << Resource::GenerateId(packId, key) << "\n";
        }
    }
    if (water_ != nullptr) {
        oss << Resource::GenerateId(*water_) << "\n";
    }
    if (bedrock_ != nullptr) {
        oss << Resource::GenerateId(*bedrock_) << "\n";
    }
    return oss.str();
}
