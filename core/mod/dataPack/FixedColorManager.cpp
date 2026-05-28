//
// Created by Cold-Mint on 2026/4/16.
//

#include "FixedColorManager.h"


void glimmer::FixedColorManager::RegisterCoreRef(const std::string &resourceId, const uint8_t r, const uint8_t b,
                                                 const uint8_t g,
                                                 const uint8_t a) {
    auto fixedColorResource = std::make_unique<FixedColorResource>();
    fixedColorResource->resourceId = resourceId;
    fixedColorResource->packId = RESOURCE_REF_CORE;
    fixedColorResource->missing = false;
    fixedColorResource->r = r;
    fixedColorResource->g = g;
    fixedColorResource->b = b;
    fixedColorResource->a = a;
    Register(std::move(fixedColorResource));
}

glimmer::FixedColorManager::FixedColorManager() {
    RegisterCoreRef(LIGHT_MASK_FULL_COLOR, 0, 0, 0, 255);
    RegisterCoreRef(LIGHT_MASK_NONE_COLOR, 0, 0, 0, 0);
    RegisterCoreRef(LIGHT_MASK_LOW_COLOR, 0, 0, 0, 64);
    RegisterCoreRef(LIGHT_MASK_MEDIUM_COLOR, 0, 0, 0, 128);
    RegisterCoreRef(LIGHT_MASK_HIGH_COLOR, 0, 0, 0, 192);
    RegisterCoreRef(LIGHT_NONE_COLOR, 0, 0, 0, 0);
    RegisterCoreRef(LIGHT_SKY_COLOR, 255, 255, 255, 24);
}

glimmer::FixedColorResource *glimmer::FixedColorManager::Register(
    std::unique_ptr<FixedColorResource> fixedColorResource) {
    auto &slot =
            fixedColorMap_[fixedColorResource->packId][fixedColorResource->resourceId];
    slot = std::move(fixedColorResource);
    return slot.get();
}

glimmer::FixedColorResource *glimmer::FixedColorManager::FindFixedColorResource(const std::string &packId,
    const std::string &key) {
    const auto packIt = fixedColorMap_.find(packId);
    if (packIt == fixedColorMap_.end()) {
        return nullptr;
    }

    auto &keyMap = packIt->second;
    const auto keyIt = keyMap.find(key);
    if (keyIt == keyMap.end()) {
        return nullptr;
    }
    return keyIt->second.get();
}

std::vector<std::string> glimmer::FixedColorManager::GetFixedColorResourceList() const {
    std::vector<std::string> result;
    for (const auto &packPair: fixedColorMap_) {
        const auto &packId = packPair.first;
        const auto &keyMap = packPair.second;

        for (const auto &keyPair: keyMap) {
            const auto &key = keyPair;
            result.emplace_back(Resource::GenerateId(packId, key.first));
        }
    }
    return result;
}

std::string glimmer::FixedColorManager::ListFixedColorResources() const {
    std::ostringstream oss;
    for (const auto &packPair: fixedColorMap_) {
        const auto &packId = packPair.first;
        const auto &keyMap = packPair.second;
        for (const auto &keyPair: keyMap) {
            const auto &key = keyPair.first;
            oss << Resource::GenerateId(packId, key) << "\n";
        }
    }
    return oss.str();
}
