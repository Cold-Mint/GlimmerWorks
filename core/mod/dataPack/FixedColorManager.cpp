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
#include "FixedColorManager.h"

#include "google/protobuf/compiler/csharp/csharp_field_base.h"


void glimmer::FixedColorManager::RegisterCoreRef(std::string_view resourceId, uint8_t r, uint8_t b,
                                                 uint8_t g, uint8_t a) {
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

glimmer::FixedColorResource *glimmer::FixedColorManager::FindFixedColorResource(std::string_view packId,
    std::string_view key) {
    if (const auto packIt = fixedColorMap_.find(packId); packIt != fixedColorMap_.end()) {
        if (const auto keyIt = packIt->second.find(key); keyIt != packIt->second.end()) {
            return keyIt->second.get();
        }
    }
    return nullptr;
}

std::vector<std::string> glimmer::FixedColorManager::GetFixedColorResourceList() const {
    std::vector<std::string> result;
    for (const auto &[packId, keyMap]: fixedColorMap_) {
        for (const auto &[key, resource]: keyMap) {
            result.emplace_back(Resource::GenerateId(packId, key));
        }
    }
    return result;
}

std::string glimmer::FixedColorManager::ListFixedColorResources() const {
    std::ostringstream oss;
    for (const auto &[packId, keyMap]: fixedColorMap_) {
        for (const auto &[key, resource]: keyMap) {
            oss << Resource::GenerateId(packId, key) << "\n";
        }
    }
    return oss.str();
}
