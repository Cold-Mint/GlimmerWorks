//
// Created by coldmint on 2026/4/16.
//

#include "FixedColorManager.h"

glimmer::FixedColorManager::FixedColorManager() {
    lightMaskFillColorResource_ = std::make_unique<FixedColorResource>();
    lightMaskFillColorResource_->r = 0;
    lightMaskFillColorResource_->g = 0;
    lightMaskFillColorResource_->b = 0;
    lightMaskFillColorResource_->a = 255;
    lightMaskNoneColorResource_ = std::make_unique<FixedColorResource>();
    lightMaskNoneColorResource_->r = 0;
    lightMaskNoneColorResource_->g = 0;
    lightMaskNoneColorResource_->b = 0;
    lightMaskNoneColorResource_->a = 0;
    lightNoneColorResource_ = std::make_unique<FixedColorResource>();
    lightNoneColorResource_->r = 0;
    lightNoneColorResource_->g = 0;
    lightNoneColorResource_->b = 0;
    lightNoneColorResource_->a = 0;
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
    if (packId == RESOURCE_REF_CORE && key == LIGHT_MASK_FULL_COLOR) {
        return lightMaskFillColorResource_.get();
    }
    if (packId == RESOURCE_REF_CORE && key == LIGHT_MASK_NONE_COLOR) {
        return lightMaskNoneColorResource_.get();
    }
    if (packId == RESOURCE_REF_CORE && key == LIGHT_NONE_COLOR) {
        return lightNoneColorResource_.get();
    }
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
