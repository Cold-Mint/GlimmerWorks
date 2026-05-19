//
// Created by coldmint on 2026/4/16.
//

#include "LightMaskManager.h"


glimmer::LightMaskResource *glimmer::LightMaskManager::RegisterCoreLightMaskResource(const std::string &resourceId,
    const std::string &colorKey) {
    std::unique_ptr<LightMaskResource> result = std::make_unique<LightMaskResource>();
    result->packId = RESOURCE_REF_CORE;
    result->resourceId = resourceId;
    result->missing = false;
    ResourceRef colorRef;
    colorRef.SetSelfPackageId(RESOURCE_REF_CORE);
    colorRef.SetResourceKey(colorKey);
    colorRef.SetResourceType(FixedColor);
    result->lightMaskColor = colorRef;
    return Register(std::move(result));
}

glimmer::LightMaskManager::LightMaskManager() {
    RegisterCoreLightMaskResource(LIGHT_MASK_FULL, LIGHT_MASK_FULL_COLOR);
    RegisterCoreLightMaskResource(LIGHT_MASK_HIGH, LIGHT_MASK_HIGH_COLOR);
    RegisterCoreLightMaskResource(LIGHT_MASK_MEDIUM, LIGHT_MASK_MEDIUM_COLOR);
    RegisterCoreLightMaskResource(LIGHT_MASK_LOW, LIGHT_MASK_LOW_COLOR);
    RegisterCoreLightMaskResource(LIGHT_MASK_NONE, LIGHT_MASK_NONE_COLOR);
}

glimmer::LightMaskResource *glimmer::LightMaskManager::Register(std::unique_ptr<LightMaskResource> lightMaskResource) {
    auto &slot =
            lightMaskMap_[lightMaskResource->packId][lightMaskResource->resourceId];
    slot = std::move(lightMaskResource);
    return slot.get();
}

glimmer::LightMaskResource *glimmer::LightMaskManager::FindLightMaskResource(const std::string &packId,
                                                                             const std::string &key) {
    const auto packIt = lightMaskMap_.find(packId);
    if (packIt == lightMaskMap_.end()) {
        return nullptr;
    }

    auto &keyMap = packIt->second;
    const auto keyIt = keyMap.find(key);
    if (keyIt == keyMap.end()) {
        return nullptr;
    }
    return keyIt->second.get();
}

std::vector<std::string> glimmer::LightMaskManager::GetLightMaskResourceList() const {
    std::vector<std::string> result;
    for (const auto &packPair: lightMaskMap_) {
        const auto &packId = packPair.first;
        const auto &keyMap = packPair.second;

        for (const auto &keyPair: keyMap) {
            const auto &key = keyPair;
            result.emplace_back(Resource::GenerateId(packId, key.first));
        }
    }
    return result;
}

std::string glimmer::LightMaskManager::ListLightMaskResource() const {
    std::ostringstream oss;
    for (const auto &packPair: lightMaskMap_) {
        const auto &packId = packPair.first;
        const auto &keyMap = packPair.second;
        for (const auto &keyPair: keyMap) {
            const auto &key = keyPair.first;
            oss << Resource::GenerateId(packId, key) << "\n";
        }
    }
    return oss.str();
}
