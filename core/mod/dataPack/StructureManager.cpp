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
    structureVector_.push_back(slot.get());
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

const std::vector<glimmer::StructureResource *> &glimmer::StructureManager::GetAll() const {
    return structureVector_;
}

std::vector<std::string> glimmer::StructureManager::GetStructureIDList() const {
    std::vector<std::string> result;
    for (const auto structureVector: structureVector_) {
        result.emplace_back(Resource::GenerateId(structureVector->packId, structureVector->key));
    }
    return result;
}

std::string glimmer::StructureManager::ListStructures() const {
    std::string result;
    for (auto structureVector: structureVector_) {
        result += Resource::GenerateId(structureVector->packId, structureVector->key);
        result += "\n";
    }
    return result;
}
