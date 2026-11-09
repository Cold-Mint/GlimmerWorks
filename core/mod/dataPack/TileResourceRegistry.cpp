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
#include "TileResourceRegistry.h"

#include <utility>

#include "core/config/Constants.h"
#include "core/world/generator/TileLayerType.h"


glimmer::TileResourceRegistry::TileResourceRegistry() {
    auto air = std::make_unique<TileResource>();
    air->packId = Constants::RESOURCE_REF_CORE.data();
    air->resourceId = Constants::TILE_ID_AIR.data();
    air->texture = GetCoreResourceRef("tiles/air", RESOURCE_TEXTURE);
    air->description = GetCoreResourceRef(Constants::STRING_TILE_AIR_DESCRIPTION, RESOURCE_STRING);
    air->name = GetCoreResourceRef(Constants::STRING_TILE_AIR_NAME, RESOURCE_STRING);
    air->lightSource = GetCoreResourceRef(Constants::LIGHT_NONE, RESOURCE_LIGHT_SOURCE);
    air->sideLightMask = GetCoreResourceRef(Constants::LIGHT_MASK_NONE, RESOURCE_LIGHT_MASK);
    air->backLightMask = GetCoreResourceRef(Constants::LIGHT_MASK_NONE, RESOURCE_LIGHT_MASK);
    air->tags.emplace_back(StringUtils::MakeTag(Constants::TAG_AIR));
    air->collisionMask = Constants::COLLISION_MASK_NONE;
    air->layer = std::to_underlying(TileLayerType::Ground);
    air->unitHardness = -1.0F;
    air->isOverwritable = true;
    air->canDropLoot = false;
    air->solid = false;
    air->partial = true;
    Register(std::move(air));

    auto airDecoration = std::make_unique<TileResource>();
    airDecoration->packId = Constants::RESOURCE_REF_CORE.data();
    airDecoration->resourceId = Constants::TILE_ID_AIR_DECORATION.data();
    airDecoration->texture = GetCoreResourceRef("tiles/air_decoration", RESOURCE_TEXTURE);
    airDecoration->description = GetCoreResourceRef(Constants::STRING_TILE_AIR_DECORATION_DESCRIPTION, RESOURCE_STRING);
    airDecoration->name = GetCoreResourceRef(Constants::STRING_TILE_AIR_DECORATION_NAME, RESOURCE_STRING);
    airDecoration->lightSource = GetCoreResourceRef(Constants::LIGHT_NONE, RESOURCE_LIGHT_SOURCE);
    airDecoration->sideLightMask = GetCoreResourceRef(Constants::LIGHT_MASK_NONE, RESOURCE_LIGHT_MASK);
    airDecoration->backLightMask = GetCoreResourceRef(Constants::LIGHT_MASK_NONE, RESOURCE_LIGHT_MASK);
    airDecoration->tags.emplace_back(StringUtils::MakeTag(Constants::TAG_AIR));
    airDecoration->collisionMask = Constants::COLLISION_MASK_NONE;
    airDecoration->layer = std::to_underlying(TileLayerType::BackgroundDecoration);
    airDecoration->unitHardness = -1.0F;
    airDecoration->isOverwritable = true;
    airDecoration->canDropLoot = false;
    airDecoration->solid = false;
    airDecoration->partial = true;
    Register(std::move(airDecoration));

    auto airWall = std::make_unique<TileResource>();
    airWall->packId = Constants::RESOURCE_REF_CORE.data();
    airWall->resourceId = Constants::TILE_ID_AIR_WALL.data();
    airWall->texture = GetCoreResourceRef("tiles/air_wall", RESOURCE_TEXTURE);
    airWall->description = GetCoreResourceRef(Constants::STRING_TILE_AIR_WALL_DESCRIPTION, RESOURCE_STRING);
    airWall->name = GetCoreResourceRef(Constants::STRING_TILE_AIR_WALL_NAME, RESOURCE_STRING);
    airWall->lightSource = GetCoreResourceRef(Constants::LIGHT_NONE, RESOURCE_LIGHT_SOURCE);
    airWall->sideLightMask = GetCoreResourceRef(Constants::LIGHT_MASK_NONE, RESOURCE_LIGHT_MASK);
    airWall->backLightMask = GetCoreResourceRef(Constants::LIGHT_MASK_NONE, RESOURCE_LIGHT_MASK);
    airWall->tags.emplace_back(StringUtils::MakeTag(Constants::TAG_AIR));
    airWall->collisionMask = Constants::COLLISION_MASK_NONE;
    airWall->layer = std::to_underlying(TileLayerType::BackGround);
    airWall->unitHardness = -1.0F;
    airWall->isOverwritable = true;
    airWall->canDropLoot = false;
    airWall->solid = false;
    airWall->partial = true;
    Register(std::move(airWall));

    auto voidTile = std::make_unique<TileResource>();
    voidTile->packId = Constants::RESOURCE_REF_CORE.data();
    voidTile->resourceId = Constants::TILE_ID_VOID.data();
    voidTile->texture = GetCoreResourceRef("tiles/void", RESOURCE_TEXTURE);
    voidTile->description = GetCoreResourceRef(Constants::STRING_TILE_VOID_DESCRIPTION, RESOURCE_STRING);
    voidTile->name = GetCoreResourceRef(Constants::STRING_TILE_VOID_NAME, RESOURCE_STRING);
    voidTile->lightSource = GetCoreResourceRef(Constants::LIGHT_NONE, RESOURCE_LIGHT_SOURCE);
    voidTile->sideLightMask = GetCoreResourceRef(Constants::LIGHT_MASK_NONE, RESOURCE_LIGHT_MASK);
    voidTile->backLightMask = GetCoreResourceRef(Constants::LIGHT_MASK_LOW, RESOURCE_LIGHT_MASK);
    voidTile->tags.emplace_back(StringUtils::MakeTag(Constants::TAG_VOID));
    voidTile->collisionMask = Constants::COLLISION_MASK_NONE;
    voidTile->layer = std::to_underlying(TileLayerType::Ground);
    voidTile->unitHardness = -1.0F;
    voidTile->isOverwritable = false;
    voidTile->canDropLoot = false;
    voidTile->solid = false;
    voidTile->partial = true;
    Register(std::move(voidTile));

    auto voidWall = std::make_unique<TileResource>();
    voidWall->packId = Constants::RESOURCE_REF_CORE.data();
    voidWall->resourceId = Constants::TILE_ID_VOID_WALL.data();
    voidWall->texture = GetCoreResourceRef("tiles/void_wall", RESOURCE_TEXTURE);
    voidWall->description = GetCoreResourceRef(Constants::STRING_TILE_VOID_WALL_DESCRIPTION, RESOURCE_STRING);
    voidWall->name = GetCoreResourceRef(Constants::STRING_TILE_VOID_WALL_NAME, RESOURCE_STRING);
    voidWall->lightSource = GetCoreResourceRef(Constants::LIGHT_NONE, RESOURCE_LIGHT_SOURCE);
    voidWall->sideLightMask = GetCoreResourceRef(Constants::LIGHT_MASK_NONE, RESOURCE_LIGHT_MASK);
    voidWall->backLightMask = GetCoreResourceRef(Constants::LIGHT_MASK_LOW, RESOURCE_LIGHT_MASK);
    voidWall->tags.emplace_back(StringUtils::MakeTag(Constants::TAG_VOID));
    voidWall->collisionMask = Constants::COLLISION_MASK_NONE;
    voidWall->layer = std::to_underlying(TileLayerType::BackGround);
    voidWall->unitHardness = -1.0F;
    voidWall->isOverwritable = true;
    voidWall->canDropLoot = false;
    voidWall->solid = false;
    voidWall->partial = true;
    Register(std::move(voidWall));

    auto voidDecoration = std::make_unique<TileResource>();
    voidDecoration->packId = Constants::RESOURCE_REF_CORE.data();
    voidDecoration->resourceId = Constants::TILE_ID_VOID_DECORATION.data();
    voidDecoration->texture = GetCoreResourceRef("tiles/void_decoration", RESOURCE_TEXTURE);
    voidDecoration->description = GetCoreResourceRef(Constants::STRING_TILE_VOID_DECORATION_DESCRIPTION,
                                                     RESOURCE_STRING);
    voidDecoration->name = GetCoreResourceRef(Constants::STRING_TILE_VOID_DECORATION_NAME, RESOURCE_STRING);
    voidDecoration->lightSource = GetCoreResourceRef(Constants::LIGHT_NONE, RESOURCE_LIGHT_SOURCE);
    voidDecoration->sideLightMask = GetCoreResourceRef(Constants::LIGHT_MASK_NONE, RESOURCE_LIGHT_MASK);
    voidDecoration->backLightMask = GetCoreResourceRef(Constants::LIGHT_MASK_LOW, RESOURCE_LIGHT_MASK);
    voidDecoration->tags.emplace_back(StringUtils::MakeTag(Constants::TAG_VOID));
    voidDecoration->collisionMask = Constants::COLLISION_MASK_NONE;
    voidDecoration->layer = std::to_underlying(TileLayerType::BackgroundDecoration);
    voidDecoration->unitHardness = -1.0F;
    voidDecoration->isOverwritable = true;
    voidDecoration->canDropLoot = false;
    voidDecoration->solid = false;
    voidDecoration->partial = true;
    Register(std::move(voidDecoration));

    auto water = std::make_unique<TileResource>();
    water->packId = Constants::RESOURCE_REF_CORE.data();
    water->resourceId = Constants::TILE_ID_WATER.data();
    water->texture = GetCoreResourceRef("tiles/water", RESOURCE_TEXTURE);
    water->name = GetCoreResourceRef(Constants::STRING_TILE_WATER_NAME, RESOURCE_STRING);
    water->lightSource = GetCoreResourceRef(Constants::LIGHT_NONE, RESOURCE_LIGHT_SOURCE);
    water->sideLightMask = GetCoreResourceRef(Constants::LIGHT_MASK_NONE, RESOURCE_LIGHT_MASK);
    water->backLightMask = GetCoreResourceRef(Constants::LIGHT_MASK_LOW, RESOURCE_LIGHT_MASK);
    water->tags.emplace_back(StringUtils::MakeTag(Constants::TAG_WATER));
    water->tags.emplace_back(StringUtils::MakeTag(Constants::TAG_FLUID));
    water->collisionMask = Constants::COLLISION_MASK_NONE;
    water->layer = std::to_underlying(TileLayerType::Ground);
    water->unitHardness = -1.0F;
    water->isOverwritable = true;
    water->canDropLoot = false;
    water->solid = false;
    water->partial = true;
    Register(std::move(water));

    auto bedrock = std::make_unique<TileResource>();
    bedrock->packId = Constants::RESOURCE_REF_CORE.data();
    bedrock->resourceId = Constants::TILE_ID_BEDROCK.data();
    bedrock->texture = GetCoreResourceRef("tiles/bedrock", RESOURCE_TEXTURE);
    bedrock->description = GetCoreResourceRef(Constants::STRING_TILE_BEDROCK_DESCRIPTION, RESOURCE_STRING);
    bedrock->name = GetCoreResourceRef(Constants::STRING_TILE_BEDROCK_NAME, RESOURCE_STRING);
    bedrock->lightSource = GetCoreResourceRef(Constants::LIGHT_NONE, RESOURCE_LIGHT_SOURCE);
    bedrock->sideLightMask = GetCoreResourceRef(Constants::LIGHT_MASK_FULL, RESOURCE_LIGHT_MASK);
    bedrock->backLightMask = GetCoreResourceRef(Constants::LIGHT_MASK_FULL, RESOURCE_LIGHT_MASK);
    bedrock->tags.emplace_back(StringUtils::MakeTag(Constants::TAG_BEDROCK));
    bedrock->collisionMask = Constants::COLLISION_MASK_SOLID;
    bedrock->layer = std::to_underlying(TileLayerType::Ground);
    bedrock->unitHardness = -1.0F;
    bedrock->isOverwritable = false;
    bedrock->canDropLoot = false;
    bedrock->solid = false;
    bedrock->partial = true;
    Register(std::move(bedrock));
}

std::unique_ptr<glimmer::TileResource> glimmer::TileResourceRegistry::GeneratePlaceholderResource(
    const std::string_view packId, const std::string_view key) {
    auto tile = std::make_unique<TileResource>();
    tile->missing = true;
    tile->packId = packId;
    tile->resourceId = key;
    return tile;
}

void glimmer::TileResourceRegistry::WriteAirResourceRef(const TileLayerType tileLayerType, ResourceRef &resourceRef) {
    resourceRef.SetSelfPackageId(Constants::RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_TILE);
    if (tileLayerType == TileLayerType::Ground) {
        resourceRef.SetResourceKey(Constants::TILE_ID_AIR);
    } else if (tileLayerType == TileLayerType::BackgroundDecoration) {
        resourceRef.SetResourceKey(Constants::TILE_ID_AIR_DECORATION);
    } else {
        resourceRef.SetResourceKey(Constants::TILE_ID_AIR_WALL);
    }
}
