//
// Created by Cold-Mint on 2025/10/9.
//
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
    LogCat::i("Registering string resource: packId = ", stringResource->packId,
              ", id = ", stringResource->resourceId, "value = ", stringResource->value);
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

    LogCat::i("Found string resource: packId = ", packId, ", key = ", key);
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
