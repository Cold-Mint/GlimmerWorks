//
// Created by Cold-Mint on 2025/10/9.
//
#include "StringManager.h"

#include "../../log/LogCat.h"


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
    return &keyIt->second;
}

std::string glimmer::StringManager::ListStrings() const {
    std::string result;
    for (const auto &packPair: stringMap_) {
        const auto &packId = packPair.first;
        const auto &keyMap = packPair.second;

        for (const auto &keyPair: keyMap) {
            const auto &key = keyPair.first;
            const auto &res = keyPair.second; // StringResource

            result += packId;
            result += ":";
            result += key;
            result += "=";
            result += res.value;
            result += "\n";
        }
    }

    return result;
}


void glimmer::StringManager::RegisterResource(const StringResource &stringResource) {
    LogCat::i("Registering string resource: packId = ", stringResource.packId,
              ", key = ", stringResource.key, "value = ", stringResource.value);
    stringMap_[stringResource.packId][stringResource.key] = stringResource;
}
