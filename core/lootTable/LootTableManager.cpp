//
// Created by coldmint on 2026/1/30.
//

#include "LootTableManager.h"

#include "core/log/LogCat.h"

glimmer::LootResource *glimmer::LootTableManager::AddResource(std::unique_ptr<LootResource> lootResource) {
    LogCat::i("Registering lootTable resource: packId = ", lootResource->packId,
              ", key = ", lootResource->key);
    auto &slot = lootMap_[lootResource->packId][lootResource->key];
    slot = std::move(lootResource);
    return slot.get();
}

glimmer::LootResource * glimmer::LootTableManager::Find(const std::string &packId, const std::string &key) {
    LogCat::d("Searching for loot resource: packId = ", packId, ", key = ", key);
    const auto packIt = lootMap_.find(packId);
    if (packIt == lootMap_.end()) {
        LogCat::w("Pack not found: ", packId);
        return nullptr;
    }

    auto &keyMap = packIt->second;
    const auto keyIt = keyMap.find(key);
    if (keyIt == keyMap.end()) {
        LogCat::w("Key not found in pack ", packId, ": ", key);
        return nullptr;
    }

    LogCat::i("Found loot resource: packId = ", packId, ", key = ", key);
    return keyIt->second.get();
}

std::vector<std::string> glimmer::LootTableManager::GetLootTableList() {
    std::vector<std::string> result;
    for (const auto &[packId, keyMap]: lootMap_) {
        for (const auto &[key, resource]: keyMap) {
            result.emplace_back(Resource::GenerateId(packId, key));
        }
    }
    return result;
}

std::string glimmer::LootTableManager::ListLootTables() const {
    std::string result;
    for (const auto &packPair: lootMap_) {
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
