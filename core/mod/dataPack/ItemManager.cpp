//
// Created by coldmint on 2025/12/23.
//

#include "ItemManager.h"

#include "../../log/LogCat.h"

void glimmer::ItemManager::RegisterResource(ItemResource &itemResource) {
    LogCat::i("Registering item resource: packId = ", itemResource.packId,
              ", key = ", itemResource.key, "texture = ", itemResource.texture);
    itemMap_[itemResource.packId][itemResource.key] = itemResource;
}

glimmer::ItemResource *glimmer::ItemManager::Find(const std::string &packId, const std::string &key) {
    LogCat::d("Searching for item resource: packId = ", packId, ", key = ", key);
    const auto packIt = itemMap_.find(packId);
    if (packIt == itemMap_.end()) {
        LogCat::w("Pack not found: ", packId);
        return nullptr;
    }

    auto &keyMap = packIt->second;
    const auto keyIt = keyMap.find(key);
    if (keyIt == keyMap.end()) {
        LogCat::w("Key not found in pack ", packId, ": ", key);
        return nullptr;
    }

    LogCat::i("Found item resource: packId = ", packId, ", key = ", key);
    return &keyIt->second;
}

std::vector<std::string> glimmer::ItemManager::GetItemIDList() {
    std::vector<std::string> result;
    for (const auto &[packId, keyMap]: itemMap_) {
        for (const auto &[key, resource]: keyMap) {
            result.emplace_back(Resource::GenerateId(packId, key));
        }
    }

    return result;
}


std::string glimmer::ItemManager::ListItems() const {
    std::string result;
    for (const auto &packPair: itemMap_) {
        const auto &packId = packPair.first;
        const auto &keyMap = packPair.second;

        for (const auto &keyPair: keyMap) {
            const auto &key = keyPair.first;

            result += Resource::GenerateId(packId, key);
            result += '\n';
        }
    }

    return result;
}
