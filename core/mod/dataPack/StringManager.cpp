//
// Created by Cold-Mint on 2025/10/9.
//
#include "StringManager.h"

#include "../../log/LogCat.h"


Glimmer::StringResource *Glimmer::StringManager::find(const std::string &packId, const std::string &key) {
    LogCat::d("Searching for string resource: packId = ", packId, ", key = ", key);
    const auto packIt = stringMap.find(packId);
    if (packIt == stringMap.end()) {
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
    return &keyIt->second;
}


void Glimmer::StringManager::registerResource(const StringResource &stringResource) {
    LogCat::i("Registering string resource: packId = ", stringResource.packId,
              ", key = ", stringResource.key);

    stringMap[stringResource.packId][stringResource.key] = stringResource;

    LogCat::d("Current number of resources in pack ", stringResource.packId, ": ",
              stringMap[stringResource.packId].size());
}
