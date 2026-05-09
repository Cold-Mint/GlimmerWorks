//
// Created by Cold-Mint on 2025/12/3.
//

#include "TileManager.h"

#include "../../Constants.h"
#include "core/world/generator/TileLayerType.h"
#include "core/world/generator/TilePhysicsType.h"


glimmer::TileResource *glimmer::TileManager::AddCoreResource(const std::string &resourceId,
                                                             TilePhysicsType physicsType, TileLayerType layerType,
                                                             float hardness, const std::string &nameKey,
                                                             const std::string &textureKey,
                                                             const std::string &lightSourceKey,
                                                             const std::string &lightMaskKey,
                                                             bool isOverwritable, bool canDropLoot,
                                                             std::optional<std::string> descriptionKey) {
    auto tileResource = std::make_unique<TileResource>();
    ResourceRef textureResourceRef;
    textureResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    textureResourceRef.SetResourceKey(textureKey);
    textureResourceRef.SetResourceType(RESOURCE_TYPE_TEXTURES);
    tileResource->texture = textureResourceRef;
    ResourceRef nameResource;
    nameResource.SetSelfPackageId(RESOURCE_REF_CORE);
    nameResource.SetResourceKey(nameKey);
    nameResource.SetResourceType(RESOURCE_TYPE_STRING);
    tileResource->name = nameResource;
    if (descriptionKey.has_value()) {
        ResourceRef descriptionResource;
        descriptionResource.SetSelfPackageId(RESOURCE_REF_CORE);
        descriptionResource.SetResourceKey(descriptionKey.value());
        descriptionResource.SetResourceType(RESOURCE_TYPE_STRING);
        tileResource->description = descriptionResource;
    }
    tileResource->resourceId = resourceId;
    tileResource->packId = RESOURCE_REF_CORE;
    tileResource->physicsType = static_cast<uint8_t>(physicsType);
    tileResource->layerType = static_cast<uint8_t>(layerType);
    tileResource->hardness = hardness;
    tileResource->isOverwritable = isOverwritable;
    tileResource->canDropLoot = canDropLoot;
    ResourceRef lightSourceRef;
    lightSourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    lightSourceRef.SetResourceType(RESOURCE_TYPE_LIGHT_SOURCE);
    lightSourceRef.SetResourceKey(lightSourceKey);
    tileResource->lightSource = lightSourceRef;
    ResourceRef lightMaskRef;
    lightMaskRef.SetSelfPackageId(RESOURCE_REF_CORE);
    lightMaskRef.SetResourceType(RESOURCE_TYPE_LIGHT_SOURCE);
    lightMaskRef.SetResourceKey(lightMaskKey);
    tileResource->lightMask = lightMaskRef;
    return AddResource(std::move(tileResource));
}

void glimmer::TileManager::InitBuiltinTiles() {
    air_ = AddCoreResource(TILE_ID_AIR, TilePhysicsType::None, Ground, -1.0F, STRING_TILE_AIR_NAME, "tiles/air",
                           LIGHT_NONE, LIGHT_MASK_NONE, true, false, STRING_TILE_AIR_DESCRIPTION);
    airWall_ = AddCoreResource(TILE_ID_AIR_WALL, TilePhysicsType::None, BackGround, -1.0F, STRING_TILE_AIR_WALL_NAME,
                               "tiles/air_wall",
                               LIGHT_SKY, LIGHT_MASK_NONE, true, false, STRING_TILE_AIR_WALL_DESCRIPTION);
    AddCoreResource(TILE_ID_BEDROCK, TilePhysicsType::Static, Ground, -1.0F, STRING_TILE_BEDROCK_NAME, "tiles/bedrock",
                    LIGHT_NONE, LIGHT_MASK_HIGH, false, false, STRING_TILE_BEDROCK_DESCRIPTION);
    AddCoreResource(TILE_ID_WATER, TilePhysicsType::None, Ground, -1.0F, STRING_TILE_WATER_NAME, "tiles/water",
                    LIGHT_NONE, LIGHT_MASK_NONE, true, true, std::nullopt);
    AddCoreResource(TILE_ID_ERROR, TilePhysicsType::Static, Ground, 0.1F, STRING_TILE_ERROR_NAME, ERROR_TEXTURE_KEY,
                    LIGHT_NONE, LIGHT_MASK_NONE, false, true, STRING_TILE_ERROR_DESCRIPTION);
    AddCoreResource(TILE_ID_ERROR_WALL, TilePhysicsType::Static, BackGround, 0.1F, STRING_TILE_ERROR_WALL_NAME,
                    ERROR_TEXTURE_KEY,
                    LIGHT_NONE, LIGHT_MASK_NONE, false, true, STRING_TILE_ERROR_WALL_DESCRIPTION);
    AddCoreResource(TILE_ID_ACCESS_DENIED, TilePhysicsType::Static, Ground, 0.1F, STRING_TILE_ACCESS_DENIED_NAME,
                    ACCESS_DENIED_TEXTURE_KEY,
                    LIGHT_NONE, LIGHT_MASK_NONE, false, true, STRING_TILE_ACCESS_DENIED_DESCRIPTION);
    AddCoreResource(TILE_ID_ACCESS_DENIED_WALL, TilePhysicsType::Static, BackGround, 0.1F,
                    STRING_TILE_ACCESS_DENIED_WALL_NAME,
                    ACCESS_DENIED_TEXTURE_KEY,
                    LIGHT_NONE, LIGHT_MASK_NONE, false, true, STRING_TILE_ACCESS_DENIED_WALL_DESCRIPTION);
}

glimmer::TileResource *glimmer::TileManager::AddErrorPlaceHolder(const std::string &packId,
                                                                 const std::string &resourceId,
                                                                 const TileLayerType tileLayer) {
    auto errorPlaceholder = std::make_unique<TileResource>();
    ResourceRef errorResource;
    errorResource.SetSelfPackageId(RESOURCE_REF_CORE);
    errorResource.SetResourceKey(ERROR_TEXTURE_KEY);
    errorResource.SetResourceType(RESOURCE_TYPE_TEXTURES);
    errorPlaceholder->texture = errorResource;
    errorPlaceholder->resourceId = resourceId;
    errorPlaceholder->packId = packId;
    if (tileLayer == Ground) {
        errorPlaceholder->physicsType = static_cast<uint8_t>(TilePhysicsType::Static);
    } else {
        errorPlaceholder->physicsType = static_cast<uint8_t>(TilePhysicsType::None);
    }
    errorPlaceholder->layerType = tileLayer;
    errorPlaceholder->hardness = 0.1F;
    errorPlaceholder->missing = true;
    return AddResource(std::move(errorPlaceholder));
}

glimmer::TileResource *glimmer::TileManager::GenerateAccessDeniedPlaceHolder(const std::string &packId,
                                                                             const std::string &resourceId,
                                                                             const TileLayerType tileLayer) {
    const auto packIt = accessDeniedTileMap_.find(packId);
    if (packIt != accessDeniedTileMap_.end()) {
        auto &keyMap = packIt->second;
        const auto keyIt = keyMap.find(resourceId);
        if (keyIt != keyMap.end()) {
            return keyIt->second.get();
        }
    }
    auto accessDeniedPlaceholder = std::make_unique<TileResource>();
    ResourceRef accessDeniedResource;
    accessDeniedResource.SetSelfPackageId(RESOURCE_REF_CORE);
    accessDeniedResource.SetResourceKey(ACCESS_DENIED_TEXTURE_KEY);
    accessDeniedResource.SetResourceType(RESOURCE_TYPE_TEXTURES);
    accessDeniedPlaceholder->texture = accessDeniedResource;
    accessDeniedPlaceholder->resourceId = resourceId;
    accessDeniedPlaceholder->packId = packId;
    accessDeniedPlaceholder->isOverwritable = false;
    accessDeniedPlaceholder->canDropLoot = true;
    if (tileLayer == Ground) {
        accessDeniedPlaceholder->physicsType = static_cast<uint8_t>(TilePhysicsType::Static);
    } else {
        accessDeniedPlaceholder->physicsType = static_cast<uint8_t>(TilePhysicsType::None);
    }
    accessDeniedPlaceholder->layerType = tileLayer;
    accessDeniedPlaceholder->hardness = 0.1F;
    accessDeniedPlaceholder->missing = true;
    auto &slot = accessDeniedTileMap_[packId][resourceId];
    slot = std::move(accessDeniedPlaceholder);
    return slot.get();
}


glimmer::TileResource *glimmer::TileManager::AddResource(std::unique_ptr<TileResource> tileResource) {
    auto &slot = tileMap_[tileResource->packId][tileResource->resourceId];
    slot = std::move(tileResource);
    return slot.get();
}

glimmer::TileResource *glimmer::TileManager::GetAirResource(const TileLayerType tileLayerType) const {
    if (tileLayerType == Ground) {
        return air_;
    }
    return airWall_;
}

glimmer::ResourceRef glimmer::TileManager::GetAirResourceRef(const TileLayerType tileLayerType) {
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_TYPE_TILE);
    if (tileLayerType == Ground) {
        resourceRef.SetResourceKey(TILE_ID_AIR);
    } else {
        resourceRef.SetResourceKey(TILE_ID_AIR_WALL);
    }
    return resourceRef;
}

glimmer::TileResource *glimmer::TileManager::FindTileRaw(const std::string &packId, const std::string &key) {
    const auto packIt = tileMap_.find(packId);
    if (packIt == tileMap_.end()) {
        return nullptr;
    }

    auto &keyMap = packIt->second;
    const auto keyIt = keyMap.find(key);
    if (keyIt == keyMap.end()) {
        return nullptr;
    }
    return keyIt->second.get();
}

glimmer::TileResource *glimmer::TileManager::FindTileFallback(const std::string &packId, const std::string &key,
                                                              const TileLayerType tileLayer) {
    TileResource *result = FindTileRaw(packId, key);
    if (result == nullptr) {
        return AddErrorPlaceHolder(packId, key, tileLayer);
    }
    return result;
}

std::vector<std::string> glimmer::TileManager::GetTileIDList() {
    std::vector<std::string> result;
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
    return oss.str();
}
