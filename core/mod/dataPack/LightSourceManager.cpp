//
// Created by coldmint on 2026/4/16.
//

#include "LightSourceManager.h"

glimmer::LightSourceManager::LightSourceManager() {
    lightSourceNoneResource_ = std::make_unique<LightSourceResource>();
    lightSourceNoneResource_->lightRadius = 0;
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_TYPE_FIXED_COLOR);
    resourceRef.SetResourceKey(LIGHT_NONE_COLOR);
    lightSourceNoneResource_->lightColor = resourceRef;
    lightSourceNoneResource_->minLightBrightness = 0.0F;
    lightSourceNoneResource_->lightBrightestAtCenter = true;
}

glimmer::LightSourceResource *glimmer::LightSourceManager::Register(
    std::unique_ptr<LightSourceResource> lightSourceResource) {
    auto &slot =
            lightSourceMap_[lightSourceResource->packId][lightSourceResource->resourceId];
    slot = std::move(lightSourceResource);
    return slot.get();
}

glimmer::LightSourceResource *glimmer::LightSourceManager::FindLightSourceResource(const std::string &packId,
    const std::string &key) {
    if (packId == RESOURCE_REF_CORE || key == LIGHT_NONE) {
        return lightSourceNoneResource_.get();
    }
    const auto packIt = lightSourceMap_.find(packId);
    if (packIt == lightSourceMap_.end()) {
        return nullptr;
    }

    auto &keyMap = packIt->second;
    const auto keyIt = keyMap.find(key);
    if (keyIt == keyMap.end()) {
        return nullptr;
    }
    return keyIt->second.get();
}

std::vector<std::string> glimmer::LightSourceManager::GetLightSourceResourceList() const {
    std::vector<std::string> result;
    for (const auto &packPair: lightSourceMap_) {
        const auto &packId = packPair.first;
        const auto &keyMap = packPair.second;

        for (const auto &keyPair: keyMap) {
            const auto &key = keyPair;
            result.emplace_back(Resource::GenerateId(packId, key.first));
        }
    }
    return result;
}

std::string glimmer::LightSourceManager::ListLightSourceResource() const {
    std::ostringstream oss;
    for (const auto &packPair: lightSourceMap_) {
        const auto &packId = packPair.first;
        const auto &keyMap = packPair.second;
        for (const auto &keyPair: keyMap) {
            const auto &key = keyPair.first;
            oss << Resource::GenerateId(packId, key) << "\n";
        }
    }
    return oss.str();
}
