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

#include "core/config/Constants.h"
#include "core/world/generator/TileLayerType.h"


glimmer::TileResource *glimmer::TileResourceManager::AddCoreResource(const CoreTileResourceParams &params) {
    auto tileResource = std::make_unique<TileResource>();
    tileResource->texture = CreateCoreRef(params.textureKey, RESOURCE_TEXTURE);
    tileResource->name = CreateCoreRef(params.nameKey, RESOURCE_STRING);

    if (params.descriptionKey) {
        tileResource->description = CreateCoreRef(*params.descriptionKey, RESOURCE_STRING);
    }
    tileResource->resourceId = params.resourceId;
    tileResource->packId = RESOURCE_REF_CORE;
    tileResource->collisionMask = params.collisionMask;
    tileResource->layer = std::to_underlying(params.layer);
    tileResource->unitHardness = params.unitHardness;
    tileResource->isOverwritable = params.isOverwritable;
    tileResource->canDropLoot = params.canDropLoot;
    tileResource->autoDerivedTags = params.autoDerivedTags;
    tileResource->solid = params.solid;
    tileResource->lightSource = CreateCoreRef(params.lightSourceKey, RESOURCE_LIGHT_SOURCE);
    tileResource->sideLightMask = CreateCoreRef(params.sideLightMaskKey, RESOURCE_LIGHT_MASK);
    tileResource->backLightMask = CreateCoreRef(params.backLightMaskKey, RESOURCE_LIGHT_MASK);
    tileResource->tags = params.tags;
    for (auto &tag: tileResource->tags) {
        tag.MakeCachedTag();
    }
    return AddResource(std::move(tileResource));
}

glimmer::ResourceRef
glimmer::TileResourceManager::CreateCoreRef(const std::string &key, const ResourceTypeMessage type) {
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
    air_ = AddCoreResource(CoreTileResourceParams{
        .descriptionKey = STRING_TILE_AIR_DESCRIPTION, .resourceId = TILE_ID_AIR,
        .nameKey = STRING_TILE_AIR_NAME, .textureKey = "tiles/air",
        .lightSourceKey = LIGHT_NONE, .sideLightMaskKey = LIGHT_MASK_NONE, .backLightMaskKey = LIGHT_MASK_NONE,
        .tags = {StringUtils::MakeTag(TAG_AIR)}, .unitHardness = -1.0F, .collisionMask = COLLISION_MASK_NONE,
        .layer = TileLayerType::Ground, .isOverwritable = true, .canDropLoot = false, .solid = false,

    });
    airWall_ = AddCoreResource(CoreTileResourceParams{
        .descriptionKey = STRING_TILE_AIR_WALL_DESCRIPTION, .resourceId = TILE_ID_AIR_WALL,
        .nameKey = STRING_TILE_AIR_WALL_NAME, .textureKey = "tiles/air_wall",
        .lightSourceKey = LIGHT_NONE, .sideLightMaskKey = LIGHT_MASK_NONE, .backLightMaskKey = LIGHT_MASK_NONE,
        .tags = {StringUtils::MakeTag(TAG_AIR)}, .unitHardness = -1.0F, .collisionMask = COLLISION_MASK_NONE,
        .layer = TileLayerType::BackGround, .isOverwritable = true, .canDropLoot = false, .solid = false
    });
    AddCoreResource(CoreTileResourceParams{
        .descriptionKey = STRING_TILE_VOID_DESCRIPTION, .resourceId = TILE_ID_VOID,
        .nameKey = STRING_TILE_VOID_NAME, .textureKey = "tiles/void",
        .lightSourceKey = LIGHT_NONE, .sideLightMaskKey = LIGHT_MASK_NONE, .backLightMaskKey = LIGHT_MASK_NONE,
        .tags = {StringUtils::MakeTag(TAG_VOID)}, .unitHardness = -1.0F, .collisionMask = COLLISION_MASK_NONE,
        .layer = TileLayerType::Ground, .isOverwritable = false, .canDropLoot = false,
    });
    AddCoreResource(CoreTileResourceParams{
        .descriptionKey = STRING_TILE_BEDROCK_DESCRIPTION, .resourceId = TILE_ID_BEDROCK,
        .nameKey = STRING_TILE_BEDROCK_NAME, .textureKey = "tiles/bedrock",
        .lightSourceKey = LIGHT_NONE, .sideLightMaskKey = LIGHT_MASK_FULL, .backLightMaskKey = LIGHT_MASK_FULL,
        .tags = {StringUtils::MakeTag(TAG_BEDROCK)}, .unitHardness = -1.0F, .collisionMask = COLLISION_MASK_SOLID,
        .layer = TileLayerType::Ground, .isOverwritable = false, .canDropLoot = false, .solid = true
    });
    AddCoreResource(CoreTileResourceParams{
        .descriptionKey = STRING_TILE_VOID_WALL_DESCRIPTION, .resourceId = TILE_ID_VOID_WALL,
        .nameKey = STRING_TILE_VOID_WALL_NAME, .textureKey = "tiles/void_wall",
        .lightSourceKey = LIGHT_NONE, .sideLightMaskKey = LIGHT_MASK_NONE, .backLightMaskKey = LIGHT_MASK_LOW,
        .tags = {StringUtils::MakeTag(TAG_VOID)}, .unitHardness = -1.0F, .collisionMask = COLLISION_MASK_NONE,
        .layer = TileLayerType::BackGround, .isOverwritable = false, .canDropLoot = false

    });
    AddCoreResource(CoreTileResourceParams{
        .descriptionKey = std::nullopt, .resourceId = TILE_ID_WATER,
        .nameKey = STRING_TILE_WATER_NAME, .textureKey = "tiles/water",
        .lightSourceKey = LIGHT_NONE, .sideLightMaskKey = LIGHT_MASK_NONE, .backLightMaskKey = LIGHT_MASK_LOW,
        .tags = {StringUtils::MakeTag(TAG_WATER), StringUtils::MakeTag(TAG_FLUID)}, .unitHardness = -1.0F,
        .collisionMask = COLLISION_MASK_NONE,
        .layer = TileLayerType::Ground, .isOverwritable = true, .canDropLoot = false,

    });
    AddCoreResource(CoreTileResourceParams{
        .descriptionKey = STRING_TILE_ERROR_DESCRIPTION, .resourceId = TILE_ID_ERROR,
        .nameKey = STRING_TILE_ERROR_NAME, .textureKey = ERROR_TEXTURE_KEY,
        .lightSourceKey = LIGHT_NONE, .sideLightMaskKey = LIGHT_MASK_MEDIUM, .backLightMaskKey = LIGHT_MASK_LOW,
        .tags = {StringUtils::MakeTag(TAG_ERROR), StringUtils::MakeTag(TAG_PLACEHOLDER)}, .unitHardness = 0.1F,
        .collisionMask = COLLISION_MASK_SOLID,
        .layer = TileLayerType::Ground, .canDropLoot = true, .solid = true

    });
    AddCoreResource(CoreTileResourceParams{
        .descriptionKey = STRING_TILE_ERROR_WALL_DESCRIPTION, .resourceId = TILE_ID_ERROR_WALL,
        .nameKey = STRING_TILE_ERROR_WALL_NAME, .textureKey = ERROR_TEXTURE_KEY,
        .lightSourceKey = LIGHT_NONE, .sideLightMaskKey = LIGHT_MASK_MEDIUM, .backLightMaskKey = LIGHT_MASK_LOW,
        .tags = {StringUtils::MakeTag(TAG_ERROR), StringUtils::MakeTag(TAG_PLACEHOLDER)}, .unitHardness = 0.1F,
        .collisionMask = COLLISION_MASK_NONE,
        .layer = TileLayerType::BackGround, .isOverwritable = false, .canDropLoot = true,

    });
    AddCoreResource(CoreTileResourceParams{
        .descriptionKey = STRING_TILE_ACCESS_DENIED_DESCRIPTION, .resourceId = TILE_ID_ACCESS_DENIED,
        .nameKey = STRING_TILE_ACCESS_DENIED_NAME, .textureKey = ACCESS_DENIED_TEXTURE_KEY,
        .lightSourceKey = LIGHT_NONE, .sideLightMaskKey = LIGHT_MASK_MEDIUM, .backLightMaskKey = LIGHT_MASK_LOW,
        .tags = {StringUtils::MakeTag(TAG_ACCESS_DENIED), StringUtils::MakeTag(TAG_PLACEHOLDER)}, .unitHardness = 0.1F,
        .collisionMask = COLLISION_MASK_SOLID,
        .layer = TileLayerType::Ground, .canDropLoot = true, .solid = true
    });
    AddCoreResource(CoreTileResourceParams{
        .descriptionKey = STRING_TILE_ACCESS_DENIED_WALL_DESCRIPTION, .resourceId = TILE_ID_ACCESS_DENIED_WALL,
        .nameKey = STRING_TILE_ACCESS_DENIED_WALL_NAME, .textureKey = ACCESS_DENIED_TEXTURE_KEY,
        .lightSourceKey = LIGHT_NONE, .sideLightMaskKey = LIGHT_MASK_MEDIUM, .backLightMaskKey = LIGHT_MASK_LOW,
        .tags = {StringUtils::MakeTag(TAG_ACCESS_DENIED), StringUtils::MakeTag(TAG_PLACEHOLDER)}, .unitHardness = 0.1F,
        .collisionMask = COLLISION_MASK_NONE,
        .layer = TileLayerType::BackGround, .canDropLoot = true
    });
}

glimmer::TileResource *glimmer::TileResourceManager::AddErrorPlaceHolder(std::string_view packId,
                                                                         std::string_view resourceId,
                                                                         const TileLayerType tileLayer) {
    auto errorPlaceholder = std::make_unique<TileResource>();
    errorPlaceholder->texture = CreateCoreRef(ERROR_TEXTURE_KEY, RESOURCE_TEXTURE);
    errorPlaceholder->resourceId = resourceId;
    errorPlaceholder->packId = packId;
    errorPlaceholder->collisionMask = tileLayer == TileLayerType::Ground ? COLLISION_MASK_SOLID : COLLISION_MASK_NONE;
    errorPlaceholder->layer = std::to_underlying(tileLayer);
    errorPlaceholder->unitHardness = 0.1F;
    errorPlaceholder->missing = true;
    errorPlaceholder->solid = true;
    errorPlaceholder->tags = {StringUtils::MakeTag(TAG_ERROR), StringUtils::MakeTag(TAG_PLACEHOLDER)};
    for (auto &tag: errorPlaceholder->tags) {
        tag.MakeCachedTag();
    }
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
    accessDeniedPlaceholder->collisionMask = tileLayer == TileLayerType::Ground
                                                 ? COLLISION_MASK_SOLID
                                                 : COLLISION_MASK_NONE;
    accessDeniedPlaceholder->layer = std::to_underlying(tileLayer);
    accessDeniedPlaceholder->unitHardness = 0.1F;
    accessDeniedPlaceholder->missing = true;
    accessDeniedPlaceholder->solid = true;
    accessDeniedPlaceholder->tags = {StringUtils::MakeTag(TAG_ACCESS_DENIED), StringUtils::MakeTag(TAG_PLACEHOLDER)};
    for (auto &tag: accessDeniedPlaceholder->tags) {
        tag.MakeCachedTag();
    }
    auto &slot = accessDeniedTileMap_[packId][resourceId];
    slot = std::move(accessDeniedPlaceholder);
    return slot.get();
}


glimmer::TileResource *glimmer::TileResourceManager::AddResource(std::unique_ptr<TileResource> tileResource) {
    auto &slot = tileMap_[tileResource->packId][tileResource->resourceId];
    slot = std::move(tileResource);
    return slot.get();
}

uint64_t glimmer::TileResourceManager::GetAirResourceRefFingerprint(const TileLayerType tileLayerType) {
    if (tileLayerType == TileLayerType::Ground) {
        return airResourceRefFingerprint_;
    }
    return airWallResourceRefFingerprint_;
}

void glimmer::TileResourceManager::WriteAirResourceRef(const TileLayerType tileLayerType, ResourceRef &resourceRef) {
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_TILE);
    if (tileLayerType == TileLayerType::Ground) {
        resourceRef.SetResourceKey(TILE_ID_AIR);
    } else {
        resourceRef.SetResourceKey(TILE_ID_AIR_WALL);
    }
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
    return AddErrorPlaceHolder(packId, key, tileLayer);
}

const std::vector<std::string> &glimmer::TileResourceManager::GetTileIDList() {
    tileIdList_.clear();
    for (const auto &[packId, keyMap]: tileMap_) {
        for (const auto &[key, resource]: keyMap) {
            tileIdList_.emplace_back(Resource::GenerateId(packId, key));
        }
    }
    return tileIdList_;
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
