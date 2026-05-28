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
#include "StringManager.h"

#include "../../log/LogCat.h"
#include "core/LangsResources.h"


glimmer::StringManager::StringManager() {
    AddCoreResource(DEV_DISPLAY_NAME_KEY_COLD_MINT, DEV_NAME_COLO_MINT);
}

void glimmer::StringManager::LoadLangsString(const LangsResources *langsResources) {
    AddCoreResource(STRING_TILE_AIR_NAME, langsResources->tileNameAir);
    AddCoreResource(STRING_TILE_AIR_WALL_NAME, langsResources->tileNameAirWall);
    AddCoreResource(STRING_TILE_ERROR_NAME, langsResources->tileNameError);
    AddCoreResource(STRING_TILE_WATER_NAME, langsResources->tileNameWater);
    AddCoreResource(STRING_TILE_ERROR_WALL_NAME, langsResources->tileNameErrorWall);
    AddCoreResource(STRING_TILE_ACCESS_DENIED_NAME, langsResources->tileNameAccessDenied);
    AddCoreResource(STRING_TILE_ACCESS_DENIED_WALL_NAME, langsResources->tileNameAccessDeniedWall);
    AddCoreResource(STRING_TILE_BEDROCK_NAME, langsResources->tileNameBedrock);
    AddCoreResource(STRING_TILE_AIR_DESCRIPTION, langsResources->tileDescriptionAir);
    AddCoreResource(STRING_TILE_AIR_WALL_DESCRIPTION, langsResources->tileDescriptionAirWall);
    AddCoreResource(STRING_TILE_ERROR_DESCRIPTION, langsResources->tileDescriptionError);
    AddCoreResource(STRING_TILE_ERROR_WALL_DESCRIPTION, langsResources->tileDescriptionErrorWall);
    AddCoreResource(STRING_TILE_ACCESS_DENIED_DESCRIPTION,
                    langsResources->tileDescriptionAccessDenied);
    AddCoreResource(STRING_TILE_ACCESS_DENIED_WALL_DESCRIPTION,
                    langsResources->tileDescriptionAccessDeniedWall);
    AddCoreResource(STRING_TILE_BEDROCK_DESCRIPTION, langsResources->tileDescriptionBedrock);
}

glimmer::StringResource *glimmer::StringManager::AddCoreResource(const std::string &resourceId,
                                                                 const std::string &value) {
    auto stringResource = std::make_unique<StringResource>();
    stringResource->missing = false;
    stringResource->value = value;
    stringResource->resourceId = resourceId;
    stringResource->packId = RESOURCE_REF_CORE;
    return AddResource(std::move(stringResource));
}


glimmer::StringResource *glimmer::StringManager::AddResource(std::unique_ptr<StringResource> stringResource) {
    auto &slot =
            stringMap_[stringResource->packId][stringResource->resourceId];
    slot = std::move(stringResource);
    return slot.get();
}

glimmer::StringResource *glimmer::StringManager::Find(const std::string &packId, const std::string &key) {
    LogCat::d("Searching for string resource: packId = ", packId, ", key = ", key);
    const auto packIt = stringMap_.find(packId);
    if (packIt == stringMap_.end()) {
        LogCat::w("Pack not found: ", packId);
        return nullptr;
    }

    auto &keyMap = packIt->second;
    const auto keyIt = keyMap.find(key);
    if (keyIt == keyMap.end()) {
        LogCat::w("Key not found in pack ", packId, ": ", key);
        return nullptr;
    }
    return keyIt->second.get();
}

std::string glimmer::StringManager::ListStrings() const {
    std::ostringstream oss;
    for (const auto &packPair: stringMap_) {
        const auto &packId = packPair.first;
        const auto &keyMap = packPair.second;
        for (const auto &keyPair: keyMap) {
            const auto &key = keyPair.first;
            const auto &res = keyPair.second;
            oss << Resource::GenerateId(packId, key)
                    << " ="
                    << res->value
                    << "\n";
        }
    }
    return oss.str();
}
