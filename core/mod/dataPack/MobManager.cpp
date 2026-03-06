//
// Created by coldmint on 2026/3/3.
//

#include "MobManager.h"

glimmer::MobResource *glimmer::MobManager::AddResource(std::unique_ptr<MobResource> mobResource) {
    auto &slot =
            mobMap_[mobResource->packId][mobResource->resourceId];
    slot = std::move(mobResource);
    if (slot.get()->isPlayer) {
        playerMobsResource_.push_back(slot.get());
    }
    return slot.get();
}

glimmer::MobResource *glimmer::MobManager::FindMobResource(const std::string &packId, const std::string &key) {
    const auto packIt = mobMap_.find(packId);
    if (packIt == mobMap_.end()) {
        return nullptr;
    }

    auto &keyMap = packIt->second;
    const auto keyIt = keyMap.find(key);
    if (keyIt == keyMap.end()) {
        return nullptr;
    }
    return keyIt->second.get();
}

const std::vector<glimmer::MobResource *> &glimmer::MobManager::GetPlayerResourceList() const {
    return playerMobsResource_;
}


std::vector<std::string> glimmer::MobManager::GetMobList() const {
    std::vector<std::string> result;
    for (const auto &packPair: mobMap_) {
        const auto &packId = packPair.first;
        const auto &keyMap = packPair.second;

        for (const auto &keyPair: keyMap) {
            const auto &key = keyPair;
            result.emplace_back(Resource::GenerateId(packId, key.first));
        }
    }
    return result;
}

std::string glimmer::MobManager::ListMobs() const {
    std::string result;
    for (const auto &packPair: mobMap_) {
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
