//
// Created by coldmint on 2026/4/16.
//

#include "LightMaskManager.h"


glimmer::LightMaskManager::LightMaskManager() {
    lightMaskFillResource_ = std::make_unique<LightMaskResource>();
    lightMaskFillResource_->packId = RESOURCE_REF_CORE;
    lightMaskFillResource_->resourceId = LIGHT_MASK_FULL;
    lightMaskFillResource_->missing = false;
    ResourceRef lightMaskFillColorRef;
    lightMaskFillColorRef.SetSelfPackageId(RESOURCE_REF_CORE);
    lightMaskFillColorRef.SetResourceKey(LIGHT_MASK_FULL_COLOR);
    lightMaskFillColorRef.SetResourceType(RESOURCE_TYPE_FIXED_COLOR);
    lightMaskFillResource_->lightMaskColor = lightMaskFillColorRef;

    lightMaskNoneResource_ = std::make_unique<LightMaskResource>();
    lightMaskNoneResource_->packId = RESOURCE_REF_CORE;
    lightMaskNoneResource_->resourceId = LIGHT_MASK_NONE;
    lightMaskNoneResource_->missing = false;
    ResourceRef lightMaskNoneColorRef;
    lightMaskNoneColorRef.SetSelfPackageId(RESOURCE_REF_CORE);
    lightMaskNoneColorRef.SetResourceKey(LIGHT_MASK_NONE_COLOR);
    lightMaskNoneColorRef.SetResourceType(RESOURCE_TYPE_FIXED_COLOR);
    lightMaskNoneResource_->lightMaskColor = lightMaskNoneColorRef;
}

glimmer::LightMaskResource *glimmer::LightMaskManager::Register(std::unique_ptr<LightMaskResource> lightMaskResource) {
    auto &slot =
            lightMaskMap_[lightMaskResource->packId][lightMaskResource->resourceId];
    slot = std::move(lightMaskResource);
    return slot.get();
}

glimmer::LightMaskResource *glimmer::LightMaskManager::FindLightMaskResource(const std::string &packId,
                                                                             const std::string &key) {
    if (packId == RESOURCE_REF_CORE && key == LIGHT_MASK_FULL) {
        return lightMaskFillResource_.get();
    }
    if (packId == RESOURCE_REF_CORE && key == LIGHT_MASK_NONE) {
        return lightMaskNoneResource_.get();
    }
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
