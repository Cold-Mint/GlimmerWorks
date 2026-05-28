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
#include "TileResourceManager.h"

#include "../../Constants.h"
#include "core/world/generator/TileLayerType.h"
#include "core/world/generator/TilePhysicsType.h"

static uint64_t airResourceRefFingerprint_ = 0;
static uint64_t airWallResourceRefFingerprint_ = 0;

glimmer::TileResource *glimmer::TileResourceManager::AddCoreResource(const std::string &resourceId,
                                                                     TilePhysicsType physicsType,
                                                                     TileLayerType layerType,
                                                                     float unitHardness, const std::string &nameKey,
                                                                     const std::string &textureKey,
                                                                     const std::string &lightSourceKey,
                                                                     const std::string &sideLightMaskKey,
                                                                     const std::string &backLightMaskKey,
                                                                     bool isOverwritable,
                                                                     bool canDropLoot,
                                                                     std::optional<std::string> descriptionKey) {
    auto tileResource = std::make_unique<TileResource>();
    ResourceRef textureResourceRef;
    textureResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    textureResourceRef.SetResourceKey(textureKey);
    textureResourceRef.SetResourceType(RESOURCE_TEXTURE);
    tileResource->texture = textureResourceRef;
    ResourceRef nameResource;
    nameResource.SetSelfPackageId(RESOURCE_REF_CORE);
    nameResource.SetResourceKey(nameKey);
    nameResource.SetResourceType(RESOURCE_STRING);
    tileResource->name = nameResource;
    if (descriptionKey.has_value()) {
        ResourceRef descriptionResource;
        descriptionResource.SetSelfPackageId(RESOURCE_REF_CORE);
        descriptionResource.SetResourceKey(descriptionKey.value());
        descriptionResource.SetResourceType(RESOURCE_STRING);
        tileResource->description = descriptionResource;
    }
    tileResource->resourceId = resourceId;
    tileResource->packId = RESOURCE_REF_CORE;
    tileResource->physicsType = static_cast<uint8_t>(physicsType);
    tileResource->layerType = static_cast<uint8_t>(layerType);
    tileResource->unitHardness = unitHardness;
    tileResource->isOverwritable = isOverwritable;
    tileResource->canDropLoot = canDropLoot;
    ResourceRef lightSourceRef;
    lightSourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    lightSourceRef.SetResourceType(RESOURCE_LIGHT_SOURCE);
    lightSourceRef.SetResourceKey(lightSourceKey);
    tileResource->lightSource = lightSourceRef;
    ResourceRef sideLightMaskRef;
    sideLightMaskRef.SetSelfPackageId(RESOURCE_REF_CORE);
    sideLightMaskRef.SetResourceType(RESOURCE_LIGHT_SOURCE);
    sideLightMaskRef.SetResourceKey(sideLightMaskKey);
    tileResource->sideLightMask = sideLightMaskRef;
    ResourceRef backLightMaskRef;
    backLightMaskRef.SetSelfPackageId(RESOURCE_REF_CORE);
    backLightMaskRef.SetResourceType(RESOURCE_LIGHT_SOURCE);
    backLightMaskRef.SetResourceKey(backLightMaskKey);
    tileResource->backLightMask = backLightMaskRef;
    return AddResource(std::move(tileResource));
}

glimmer::TileResourceManager::TileResourceManager() {
    ResourceRef airResourceRef;
    airResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    airResourceRef.SetResourceType(RESOURCE_TILE);
    airResourceRef.SetResourceKey(TILE_ID_AIR);

    airResourceRefFingerprint_ = airResourceRef.GetFingerprint();
    airResourceRef.SetResourceKey(TILE_ID_AIR_WALL);
    airWallResourceRefFingerprint_ = airResourceRef.GetFingerprint();
}

void glimmer::TileResourceManager::InitBuiltinTiles() {
    air_ = AddCoreResource(TILE_ID_AIR, TilePhysicsType::None, Ground, -1.0F, STRING_TILE_AIR_NAME, "tiles/air",
                           LIGHT_NONE, LIGHT_MASK_NONE, LIGHT_MASK_NONE, true, false, STRING_TILE_AIR_DESCRIPTION);
    airWall_ = AddCoreResource(TILE_ID_AIR_WALL, TilePhysicsType::None, BackGround, -1.0F, STRING_TILE_AIR_WALL_NAME,
                               "tiles/air_wall",
                               LIGHT_SKY, LIGHT_MASK_NONE, LIGHT_MASK_NONE, true, false,
                               STRING_TILE_AIR_WALL_DESCRIPTION);
    AddCoreResource(TILE_ID_BEDROCK, TilePhysicsType::Static, Ground, -1.0F, STRING_TILE_BEDROCK_NAME, "tiles/bedrock",
                    LIGHT_NONE, LIGHT_MASK_HIGH, LIGHT_MASK_LOW, false, false, STRING_TILE_BEDROCK_DESCRIPTION);
    AddCoreResource(TILE_ID_WATER, TilePhysicsType::None, Ground, -1.0F, STRING_TILE_WATER_NAME, "tiles/water",
                    LIGHT_NONE, LIGHT_MASK_NONE, LIGHT_MASK_LOW, true, true, std::nullopt);
    AddCoreResource(TILE_ID_ERROR, TilePhysicsType::Static, Ground, 0.1F, STRING_TILE_ERROR_NAME, ERROR_TEXTURE_KEY,
                    LIGHT_NONE, LIGHT_MASK_MEDIUM, LIGHT_MASK_LOW, false, true, STRING_TILE_ERROR_DESCRIPTION);
    AddCoreResource(TILE_ID_ERROR_WALL, TilePhysicsType::Static, BackGround, 0.1F, STRING_TILE_ERROR_WALL_NAME,
                    ERROR_TEXTURE_KEY,
                    LIGHT_NONE, LIGHT_MASK_MEDIUM, LIGHT_MASK_LOW, false, true, STRING_TILE_ERROR_WALL_DESCRIPTION);
    AddCoreResource(TILE_ID_ACCESS_DENIED, TilePhysicsType::Static, Ground, 0.1F, STRING_TILE_ACCESS_DENIED_NAME,
                    ACCESS_DENIED_TEXTURE_KEY,
                    LIGHT_NONE, LIGHT_MASK_MEDIUM, LIGHT_MASK_LOW, false, true, STRING_TILE_ACCESS_DENIED_DESCRIPTION);
    AddCoreResource(TILE_ID_ACCESS_DENIED_WALL, TilePhysicsType::Static, BackGround, 0.1F,
                    STRING_TILE_ACCESS_DENIED_WALL_NAME,
                    ACCESS_DENIED_TEXTURE_KEY,
                    LIGHT_NONE, LIGHT_MASK_MEDIUM, LIGHT_MASK_LOW, false, true,
                    STRING_TILE_ACCESS_DENIED_WALL_DESCRIPTION);
}

glimmer::TileResource *glimmer::TileResourceManager::AddErrorPlaceHolder(const std::string &packId,
                                                                         const std::string &resourceId,
                                                                         const TileLayerType tileLayer) {
    auto errorPlaceholder = std::make_unique<TileResource>();
    ResourceRef errorResource;
    errorResource.SetSelfPackageId(RESOURCE_REF_CORE);
    errorResource.SetResourceKey(ERROR_TEXTURE_KEY);
    errorResource.SetResourceType(RESOURCE_TEXTURE);
    errorPlaceholder->texture = errorResource;
    errorPlaceholder->resourceId = resourceId;
    errorPlaceholder->packId = packId;
    if (tileLayer == Ground) {
        errorPlaceholder->physicsType = static_cast<uint8_t>(TilePhysicsType::Static);
    } else {
        errorPlaceholder->physicsType = static_cast<uint8_t>(TilePhysicsType::None);
    }
    errorPlaceholder->layerType = tileLayer;
    errorPlaceholder->unitHardness = 0.1F;
    errorPlaceholder->missing = true;
    return AddResource(std::move(errorPlaceholder));
}

glimmer::TileResource *glimmer::TileResourceManager::GenerateAccessDeniedPlaceHolder(const std::string &packId,
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
    accessDeniedResource.SetResourceType(RESOURCE_TEXTURE);
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
    accessDeniedPlaceholder->unitHardness = 0.1F;
    accessDeniedPlaceholder->missing = true;
    auto &slot = accessDeniedTileMap_[packId][resourceId];
    slot = std::move(accessDeniedPlaceholder);
    return slot.get();
}


glimmer::TileResource *glimmer::TileResourceManager::AddResource(std::unique_ptr<TileResource> tileResource) {
    auto &slot = tileMap_[tileResource->packId][tileResource->resourceId];
    slot = std::move(tileResource);
    return slot.get();
}

glimmer::TileResource *glimmer::TileResourceManager::GetAirResource(const TileLayerType tileLayerType) const {
    if (tileLayerType == Ground) {
        return air_;
    }
    return airWall_;
}

uint64_t glimmer::TileResourceManager::GetAirResourceRefFingerprint(const TileLayerType tileLayerType) {
    if (tileLayerType == Ground) {
        return airResourceRefFingerprint_;
    }
    return airWallResourceRefFingerprint_;
}

glimmer::ResourceRef glimmer::TileResourceManager::GetAirResourceRef(TileLayerType tileLayerType) {
    ResourceRef airResourceRef;
    airResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    airResourceRef.SetResourceType(RESOURCE_TILE);
    if (tileLayerType == Ground) {
        airResourceRef.SetResourceKey(TILE_ID_AIR);
    } else {
        airResourceRef.SetResourceKey(TILE_ID_AIR_WALL);
    }
    return airResourceRef;
}

glimmer::TileResource *glimmer::TileResourceManager::FindTileRaw(const std::string &packId, const std::string &key) {
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

glimmer::TileResource *glimmer::TileResourceManager::FindTileFallback(const std::string &packId, const std::string &key,
                                                                      const TileLayerType tileLayer) {
    TileResource *result = FindTileRaw(packId, key);
    if (result == nullptr) {
        return AddErrorPlaceHolder(packId, key, tileLayer);
    }
    return result;
}

std::vector<std::string> glimmer::TileResourceManager::GetTileIDList() {
    std::vector<std::string> result;
    for (const auto &[packId, keyMap]: tileMap_) {
        for (const auto &[key, resource]: keyMap) {
            result.emplace_back(Resource::GenerateId(packId, key));
        }
    }

    return result;
}

std::string glimmer::TileResourceManager::ListTiles() const {
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
