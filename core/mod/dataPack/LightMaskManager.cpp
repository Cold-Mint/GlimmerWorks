/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
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
    colorRef.SetResourceType(RESOURCE_FIXED_COLOR);
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
