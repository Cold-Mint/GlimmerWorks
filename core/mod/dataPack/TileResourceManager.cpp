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

#include <utility>

#include "core/Constants.h"
#include "core/world/generator/TileLayerType.h"
#include "core/world/generator/TilePhysicsType.h"

static uint64_t airResourceRefFingerprint_ = 0;
static uint64_t airWallResourceRefFingerprint_ = 0;

glimmer::TileResource *glimmer::TileResourceManager::AddCoreResource(const CoreTileResourceParams& params) {
    auto tileResource = std::make_unique<TileResource>();
    tileResource->texture = CreateCoreRef(params.textureKey, RESOURCE_TEXTURE);
    tileResource->name = CreateCoreRef(params.nameKey, RESOURCE_STRING);
    
    if (params.descriptionKey) {
        tileResource->description = CreateCoreRef(*params.descriptionKey, RESOURCE_STRING);
    }
    
    tileResource->resourceId = params.resourceId;
    tileResource->packId = RESOURCE_REF_CORE;
    tileResource->physicsType = std::to_underlying(params.physicsType);
    tileResource->layerType = std::to_underlying(params.layerType);
    tileResource->unitHardness = params.unitHardness;
    tileResource->isOverwritable = params.isOverwritable;
    tileResource->canDropLoot = params.canDropLoot;
    
    tileResource->lightSource = CreateCoreRef(params.lightSourceKey, RESOURCE_LIGHT_SOURCE);
    tileResource->sideLightMask = CreateCoreRef(params.sideLightMaskKey, RESOURCE_LIGHT_SOURCE);
    tileResource->backLightMask = CreateCoreRef(params.backLightMaskKey, RESOURCE_LIGHT_SOURCE);
    
    return AddResource(std::move(tileResource));
}

glimmer::ResourceRef glimmer::TileResourceManager::CreateCoreRef(const std::string& key, const ResourceTypeMessage type)
{
    ResourceRef ref;
    ref.SetSelfPackageId(RESOURCE_REF_CORE);
    ref.SetResourceKey(key);
    ref.SetResourceType(type);
    return ref;
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
    air_ = AddCoreResource({
        TILE_ID_AIR, TilePhysicsType::None, Ground, -1.0F,
        STRING_TILE_AIR_NAME, "tiles/air",
        LIGHT_NONE, LIGHT_MASK_NONE, LIGHT_MASK_NONE,
        true, false, STRING_TILE_AIR_DESCRIPTION
    });
    airWall_ = AddCoreResource({
        TILE_ID_AIR_WALL, TilePhysicsType::None, BackGround, -1.0F,
        STRING_TILE_AIR_WALL_NAME, "tiles/air_wall",
        LIGHT_SKY, LIGHT_MASK_NONE, LIGHT_MASK_NONE,
        true, false, STRING_TILE_AIR_WALL_DESCRIPTION
    });
    AddCoreResource({
        TILE_ID_BEDROCK, TilePhysicsType::Static, Ground, -1.0F,
        STRING_TILE_BEDROCK_NAME, "tiles/bedrock",
        LIGHT_NONE, LIGHT_MASK_HIGH, LIGHT_MASK_LOW,
        false, false, STRING_TILE_BEDROCK_DESCRIPTION
    });
    AddCoreResource({
        TILE_ID_WATER, TilePhysicsType::None, Ground, -1.0F,
        STRING_TILE_WATER_NAME, "tiles/water",
        LIGHT_NONE, LIGHT_MASK_NONE, LIGHT_MASK_LOW,
        true, true, std::nullopt
    });
    AddCoreResource({
        TILE_ID_ERROR, TilePhysicsType::Static, Ground, 0.1F,
        STRING_TILE_ERROR_NAME, ERROR_TEXTURE_KEY,
        LIGHT_NONE, LIGHT_MASK_MEDIUM, LIGHT_MASK_LOW,
        false, true, STRING_TILE_ERROR_DESCRIPTION
    });
    AddCoreResource({
        TILE_ID_ERROR_WALL, TilePhysicsType::Static, BackGround, 0.1F,
        STRING_TILE_ERROR_WALL_NAME, ERROR_TEXTURE_KEY,
        LIGHT_NONE, LIGHT_MASK_MEDIUM, LIGHT_MASK_LOW,
        false, true, STRING_TILE_ERROR_WALL_DESCRIPTION
    });
    AddCoreResource({
        TILE_ID_ACCESS_DENIED, TilePhysicsType::Static, Ground, 0.1F,
        STRING_TILE_ACCESS_DENIED_NAME, ACCESS_DENIED_TEXTURE_KEY,
        LIGHT_NONE, LIGHT_MASK_MEDIUM, LIGHT_MASK_LOW,
        false, true, STRING_TILE_ACCESS_DENIED_DESCRIPTION
    });
    AddCoreResource({
        TILE_ID_ACCESS_DENIED_WALL, TilePhysicsType::Static, BackGround, 0.1F,
        STRING_TILE_ACCESS_DENIED_WALL_NAME, ACCESS_DENIED_TEXTURE_KEY,
        LIGHT_NONE, LIGHT_MASK_MEDIUM, LIGHT_MASK_LOW,
        false, true, STRING_TILE_ACCESS_DENIED_WALL_DESCRIPTION
    });
}

glimmer::TileResource *glimmer::TileResourceManager::AddErrorPlaceHolder(std::string_view packId,
                                                                         std::string_view resourceId,
                                                                         const TileLayerType tileLayer) {
    auto errorPlaceholder = std::make_unique<TileResource>();
    errorPlaceholder->texture = CreateCoreRef(ERROR_TEXTURE_KEY, RESOURCE_TEXTURE);
    errorPlaceholder->resourceId = resourceId;
    errorPlaceholder->packId = packId;
    errorPlaceholder->physicsType = std::to_underlying(
        tileLayer == Ground ? TilePhysicsType::Static : TilePhysicsType::None);
    errorPlaceholder->layerType = tileLayer;
    errorPlaceholder->unitHardness = 0.1F;
    errorPlaceholder->missing = true;
    return AddResource(std::move(errorPlaceholder));
}

glimmer::TileResource *glimmer::TileResourceManager::GenerateAccessDeniedPlaceHolder(std::string_view packId,
    std::string_view resourceId,
    const TileLayerType tileLayer) {
    if (const auto packIt = accessDeniedTileMap_.find(packId); packIt != accessDeniedTileMap_.end()) {
        if (const auto keyIt = packIt->second.find(resourceId); keyIt != packIt->second.end()) {
            return keyIt->second.get();
        }
    }
    auto accessDeniedPlaceholder = std::make_unique<TileResource>();
    accessDeniedPlaceholder->texture = CreateCoreRef(ACCESS_DENIED_TEXTURE_KEY, RESOURCE_TEXTURE);
    accessDeniedPlaceholder->resourceId = resourceId;
    accessDeniedPlaceholder->packId = packId;
    accessDeniedPlaceholder->isOverwritable = false;
    accessDeniedPlaceholder->canDropLoot = true;
    accessDeniedPlaceholder->physicsType = std::to_underlying(
        tileLayer == Ground ? TilePhysicsType::Static : TilePhysicsType::None);
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

glimmer::TileResource *glimmer::TileResourceManager::FindTileRaw(std::string_view packId, std::string_view key) {
    if (const auto packIt = tileMap_.find(packId); packIt != tileMap_.end()) {
        if (const auto keyIt = packIt->second.find(key); keyIt != packIt->second.end()) {
            return keyIt->second.get();
        }
    }
    return nullptr;
}

glimmer::TileResource *glimmer::TileResourceManager::FindTileFallback(std::string_view packId, std::string_view key,
                                                                      const TileLayerType tileLayer) {
    if (TileResource *result = FindTileRaw(packId, key); result != nullptr) {
        return result;
    }
    return AddErrorPlaceHolder(std::string(packId), std::string(key), tileLayer);
}

std::vector<std::string> glimmer::TileResourceManager::GetTileIDList() const {
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
    for (const auto &[packId, keyMap]: tileMap_) {
        for (const auto &[key, resource]: keyMap) {
            oss << Resource::GenerateId(packId, key) << "\n";
        }
    }
    return oss.str();
}
