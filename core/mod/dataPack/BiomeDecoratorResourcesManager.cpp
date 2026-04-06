//
// Created by coldmint on 2026/4/6.
//

#include "BiomeDecoratorResourcesManager.h"

#include "core/log/LogCat.h"


glimmer::IBiomeDecoratorResource * glimmer::BiomeDecoratorResourcesManager::Register(
    std::unique_ptr<IBiomeDecoratorResource> biomeDecoratorResource) {
    auto &slot =
        biomeDecoratorMap_[biomeDecoratorResource->packId][biomeDecoratorResource->resourceId];
    slot = std::move(biomeDecoratorResource);
    return slot.get();
}

glimmer::IBiomeDecoratorResource * glimmer::BiomeDecoratorResourcesManager::FindBiomeDecorator(
    const std::string &packId, const std::string &resourceId) {
    const auto packIt = biomeDecoratorMap_.find(packId);
    if (packIt == biomeDecoratorMap_.end()) {
        return nullptr;
    }

    auto &keyMap = packIt->second;
    const auto keyIt = keyMap.find(resourceId);
    if (keyIt == keyMap.end()) {
        return nullptr;
    }
    return keyIt->second.get();
}

std::string glimmer::BiomeDecoratorResourcesManager::ListBiomeDecorators() const {
    std::ostringstream oss;
    for (const auto &packPair: biomeDecoratorMap_) {
        const auto &packId = packPair.first;
        const auto &keyMap = packPair.second;
        for (const auto &keyPair: keyMap) {
            const auto &key = keyPair.first;
            oss << Resource::GenerateId(packId, key) << "\n";
        }
    }
    return oss.str();
}
