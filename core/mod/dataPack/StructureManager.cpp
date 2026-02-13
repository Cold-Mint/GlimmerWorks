//
// Created by coldmint on 2026/2/13.
//

#include "StructureManager.h"

#include "core/log/LogCat.h"

glimmer::StructureResource *glimmer::StructureManager::AddResource(
    std::unique_ptr<StructureResource> structureResource) {
    LogCat::i("Registering tile resource: packId = ", structureResource->packId,
              ", key = ", structureResource->key);
    auto &slot = structureMap_[structureResource->packId][structureResource->key];
    slot = std::move(structureResource);
    return slot.get();
}

glimmer::StructureResource *glimmer::StructureManager::Find(const std::string &packId, const std::string &key) {
    LogCat::d("Searching for structure resource: packId = ", packId, ", key = ", key);
    const auto packIt = structureMap_.find(packId);
    if (packIt == structureMap_.end()) {
        LogCat::w("Pack not found: ", packId);
        return nullptr;
    }

    auto &keyMap = packIt->second;
    const auto keyIt = keyMap.find(key);
    if (keyIt == keyMap.end()) {
        LogCat::w("Key not found in pack ", packId, ": ", key);
        return nullptr;
    }

    LogCat::i("Found structure resource: packId = ", packId, ", key = ", key);
    return keyIt->second.get();
}

std::vector<std::string> glimmer::StructureManager::GetStructureIDList() {
    std::vector<std::string> result;
    for (const auto &[packId, keyMap]: structureMap_) {
        for (const auto &[key, resource]: keyMap) {
            result.emplace_back(Resource::GenerateId(packId, key));
        }
    }

    return result;
}

std::string glimmer::StructureManager::ListStructures() {
    std::string result;
    for (const auto &packPair: structureMap_) {
        const auto &packId = packPair.first;
        const auto &keyMap = packPair.second;

        for (const auto &keyPair: keyMap) {
            const auto &key = keyPair.first;
            result += Resource::GenerateId(packId, key);
            result += "\n";
        }
    }
    return result;
}
