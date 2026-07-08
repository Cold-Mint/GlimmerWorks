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
#include "MobManager.h"

glimmer::MobResource *glimmer::MobManager::Register(std::unique_ptr<MobResource> mobResource) {
    auto &slot =
            mobMap_[mobResource->packId][mobResource->resourceId];
    slot = std::move(mobResource);
    if (slot.get()->isPlayer) {
        playerMobsResource_.push_back(slot.get());
    }
    return slot.get();
}

glimmer::MobResource *glimmer::MobManager::FindMobResource(std::string_view packId, std::string_view key) {
    if (const auto packIt = mobMap_.find(packId); packIt != mobMap_.end()) {
        if (const auto keyIt = packIt->second.find(key); keyIt != packIt->second.end()) {
            return keyIt->second.get();
        }
    }
    return nullptr;
}

std::span<const glimmer::MobResource* const> glimmer::MobManager::GetPlayerResourceList() const {
    return playerMobsResource_;
}


std::vector<std::string> glimmer::MobManager::GetMobList() const {
    std::vector<std::string> result;
    for (const auto &[packId, keyMap]: mobMap_) {
        for (const auto &[key, resource]: keyMap) {
            result.emplace_back(Resource::GenerateId(packId, key));
        }
    }
    return result;
}

std::string glimmer::MobManager::ListMobs() const {
    std::ostringstream oss;
    for (const auto &[packId, keyMap]: mobMap_) {
        for (const auto &[key, resource]: keyMap) {
            oss << Resource::GenerateId(packId, key) << "\n";
        }
    }
    return oss.str();
}
