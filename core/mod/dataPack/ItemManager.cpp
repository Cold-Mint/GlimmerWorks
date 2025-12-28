//
// Created by coldmint on 2025/12/23.
//

#include "ItemManager.h"

#include "../../log/LogCat.h"

void glimmer::ItemManager::RegisterComposableResource(ComposableItemResource &itemResource) {
    LogCat::i("Registering composable item resource: packId = ", itemResource.packId,
              ", key = ", itemResource.key, "texture = ", itemResource.texture);
    composableItemMap_[itemResource.packId][itemResource.key] = itemResource;
}

void glimmer::ItemManager::RegisterAbilityItemResource(AbilityItemResource &itemResource) {
    LogCat::i("Registering ability item resource: packId = ", itemResource.packId,
              ", key = ", itemResource.key, "texture = ", itemResource.texture);
    abilityItemMap_[itemResource.packId][itemResource.key] = itemResource;
}

glimmer::ComposableItemResource *glimmer::ItemManager::FindComposableItemResource(
    const std::string &packId, const std::string &key) {
    LogCat::d("Searching for item resource: packId = ", packId, ", key = ", key);
    const auto packIt = composableItemMap_.find(packId);
    if (packIt == composableItemMap_.end()) {
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

glimmer::AbilityItemResource *glimmer::ItemManager::FindAbilityItemResource(const std::string &packId,
                                                                            const std::string &key) {
    LogCat::d("Searching for item resource: packId = ", packId, ", key = ", key);
    const auto packIt = abilityItemMap_.find(packId);
    if (packIt == abilityItemMap_.end()) {
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

std::vector<std::string> glimmer::ItemManager::GetComposableItemIDList() {
    std::vector<std::string> result;
    for (const auto &[packId, keyMap]: composableItemMap_) {
        for (const auto &[key, resource]: keyMap) {
            result.emplace_back(Resource::GenerateId(packId, key));
        }
    }
    return result;
}

std::vector<std::string> glimmer::ItemManager::GetAbilityItemIDList() {
    std::vector<std::string> result;
    for (const auto &[packId, keyMap]: abilityItemMap_) {
        for (const auto &[key, resource]: keyMap) {
            result.emplace_back(Resource::GenerateId(packId, key));
        }
    }
    return result;
}

std::string glimmer::ItemManager::ListComposableItems() const {
    std::string result;
    for (const auto &packPair: composableItemMap_) {
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

std::string glimmer::ItemManager::ListAbilityItems() const {
    std::string result;
    for (const auto &packPair: abilityItemMap_) {
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
