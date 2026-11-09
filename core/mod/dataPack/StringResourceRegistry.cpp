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
#include "StringResourceRegistry.h"

#include "core/utils/LangsResources.h"


void glimmer::StringResourceRegistry::LoadLangsString(const LangsResources *langsResources) {
    RegisterCoreResource(Constants::STRING_TILE_AIR_NAME, langsResources->tileNameAir);
    RegisterCoreResource(Constants::STRING_TILE_AIR_WALL_NAME, langsResources->tileNameAirWall);
    RegisterCoreResource(Constants::STRING_TILE_ERROR_NAME, langsResources->tileNameError);
    RegisterCoreResource(Constants::STRING_TILE_WATER_NAME, langsResources->tileNameWater);
    RegisterCoreResource(Constants::STRING_TILE_ERROR_WALL_NAME, langsResources->tileNameErrorWall);
    RegisterCoreResource(Constants::STRING_TILE_ACCESS_DENIED_NAME, langsResources->tileNameAccessDenied);
    RegisterCoreResource(Constants::STRING_TILE_ACCESS_DENIED_WALL_NAME, langsResources->tileNameAccessDeniedWall);
    RegisterCoreResource(Constants::STRING_TILE_BEDROCK_NAME, langsResources->tileNameBedrock);
    RegisterCoreResource(Constants::STRING_TILE_VOID_NAME, langsResources->tileNameVoid);
    RegisterCoreResource(Constants::STRING_TILE_VOID_DESCRIPTION, langsResources->tileDescriptionVoid);
    RegisterCoreResource(Constants::STRING_TILE_VOID_WALL_NAME, langsResources->tileNameVoidWall);
    RegisterCoreResource(Constants::STRING_TILE_VOID_WALL_DESCRIPTION, langsResources->tileDescriptionVoidWall);
    RegisterCoreResource(Constants::STRING_TILE_AIR_DESCRIPTION, langsResources->tileDescriptionAir);
    RegisterCoreResource(Constants::STRING_TILE_AIR_WALL_DESCRIPTION, langsResources->tileDescriptionAirWall);
    RegisterCoreResource(Constants::STRING_TILE_ERROR_DESCRIPTION, langsResources->tileDescriptionError);
    RegisterCoreResource(Constants::STRING_TILE_ERROR_WALL_DESCRIPTION, langsResources->tileDescriptionErrorWall);
    RegisterCoreResource(Constants::STRING_TILE_ACCESS_DENIED_DESCRIPTION,
                    langsResources->tileDescriptionAccessDenied);
    RegisterCoreResource(Constants::STRING_TILE_ACCESS_DENIED_WALL_DESCRIPTION,
                    langsResources->tileDescriptionAccessDeniedWall);
    RegisterCoreResource(Constants::STRING_TILE_BEDROCK_DESCRIPTION, langsResources->tileDescriptionBedrock);
}

void glimmer::StringResourceRegistry::RegisterCoreResource(const std::string_view resourceId, const std::string_view value) {
    auto stringResource = std::make_unique<StringResource>();
    stringResource->missing = false;
    stringResource->value = value;
    stringResource->resourceId = resourceId;
    stringResource->packId = Constants::RESOURCE_REF_CORE;
    Register(std::move(stringResource));
}

void glimmer::StringResourceRegistry::SetTagTranslate(const uint64_t tag, std::string_view value) {
    tagTranslateMap_[tag] = value;
}

std::optional<std::string> glimmer::StringResourceRegistry::GetTagTranslate(const uint64_t tag) {
    auto translateIt = tagTranslateMap_.find(tag);
    if (translateIt == tagTranslateMap_.end()) {
        return std::nullopt;
    }
    return translateIt->second;
}
