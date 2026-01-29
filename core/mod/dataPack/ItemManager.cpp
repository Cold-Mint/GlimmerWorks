//
// Created by coldmint on 2025/12/23.
//

#include "ItemManager.h"

#include "../../log/LogCat.h"


glimmer::ComposableItemResource *glimmer::ItemManager::AddComposableResource(
    std::unique_ptr<ComposableItemResource> itemResource) {
    LogCat::i("Registering composable item resource: packId = ", itemResource->packId,
              ", key = ", itemResource->key, "texture = ", itemResource->texture);
    auto &slot =
            composableItemMap_[itemResource->packId][itemResource->key];
    slot = std::move(itemResource);
    return slot.get();
}

std::unique_ptr<glimmer::ComposableItemResource> glimmer::ItemManager::CreatePlaceholderComposableItemResource(
    const std::string &packId, const std::string &key) {
    auto composableItemResource = std::make_unique<ComposableItemResource>();
    composableItemResource->packId = packId;
    composableItemResource->key = key;
    composableItemResource->texture = ERROR_TEXTURE_PATH;
    composableItemResource->defaultAbilityList = {};
    composableItemResource->slotSize = 0;
    composableItemResource->missing = true;
    return composableItemResource;
}

std::unique_ptr<glimmer::AbilityItemResource> glimmer::ItemManager::CreateAbilityItemResource(const std::string &packId,
    const std::string &key) {
    auto abilityItemResource = std::make_unique<AbilityItemResource>();
    abilityItemResource->packId = packId;
    abilityItemResource->key = key;
    abilityItemResource->texture = ERROR_TEXTURE_PATH;
    abilityItemResource->ability = ABILITY_ID_NONE;
    abilityItemResource->missing = true;
    return abilityItemResource;
}


glimmer::AbilityItemResource *glimmer::ItemManager::AddAbilityItemResource(
    std::unique_ptr<AbilityItemResource> itemResource) {
    LogCat::i("Registering ability item resource: packId = ", itemResource->packId,
              ", key = ", itemResource->key, "texture = ", itemResource->texture);
    auto &slot =
            abilityItemMap_[itemResource->packId][itemResource->key];
    slot = std::move(itemResource);
    return slot.get();
}


glimmer::ComposableItemResource *glimmer::ItemManager::FindComposableItemResource(
    const std::string &packId, const std::string &key) {
    LogCat::d("Searching for item resource: packId = ", packId, ", key = ", key);
    const auto packIt = composableItemMap_.find(packId);
    if (packIt == composableItemMap_.end()) {
        LogCat::w("Pack not found: ", packId);
        return AddComposableResource(CreatePlaceholderComposableItemResource(packId, key));
    }

    auto &keyMap = packIt->second;
    const auto keyIt = keyMap.find(key);
    if (keyIt == keyMap.end()) {
        LogCat::w("Key not found in pack ", packId, ": ", key);
        return AddComposableResource(CreatePlaceholderComposableItemResource(packId, key));
    }

    LogCat::i("Found item resource: packId = ", packId, ", key = ", key);
    return keyIt->second.get();
}

glimmer::AbilityItemResource *glimmer::ItemManager::FindAbilityItemResource(const std::string &packId,
                                                                            const std::string &key) {
    LogCat::d("Searching for item resource: packId = ", packId, ", key = ", key);
    const auto packIt = abilityItemMap_.find(packId);
    if (packIt == abilityItemMap_.end()) {
        LogCat::w("Pack not found: ", packId);
        return AddAbilityItemResource(CreateAbilityItemResource(packId, key));
    }

    auto &keyMap = packIt->second;
    const auto keyIt = keyMap.find(key);
    if (keyIt == keyMap.end()) {
        LogCat::w("Key not found in pack ", packId, ": ", key);
        return AddAbilityItemResource(CreateAbilityItemResource(packId, key));
    }

    LogCat::i("Found item resource: packId = ", packId, ", key = ", key);
    return keyIt->second.get();
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
