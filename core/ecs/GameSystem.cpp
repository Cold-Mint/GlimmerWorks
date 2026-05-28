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
#include "GameSystem.h"
#include "../world/WorldContext.h"

bool glimmer::GameSystem::ShouldActivate() {
    if (worldContext_ == nullptr) {
        return false;
    }
    if (requiredComponents.empty()) {
        return false;
    }
    for (const auto &type: requiredComponents) {
        if (!worldContext_->HasComponentType(type)) {
            //A certain dependent component type in the world does not exist and cannot be activated
            // 世界中某个依赖的组件类型不存在，不能激活
            return false;
        }
    }
    return true; // All components exist and can be activated 所有组件都存在，可以激活
}

void glimmer::GameSystem::OnActivationChanged(const bool activeStatus) {
}

glimmer::GameSystem::GameSystem(WorldContext *worldContext) : worldContext_(worldContext) {
}

bool glimmer::GameSystem::CheckActivation() {
    const bool shouldActivate = ShouldActivate();
    if (!active && shouldActivate) {
        active = true;
        LogCat::w("System Activated: ", GetName());
        OnActivationChanged(active);
    } else if (active && !shouldActivate) {
        active = false;
        LogCat::w("System Deactivated: ", GetName());
        OnActivationChanged(active);
    }
    return active;
}

bool glimmer::GameSystem::IsActive() const {
    return active;
}

bool glimmer::GameSystem::SupportsComponentType(const std::type_index &type) const {
    if (requiredComponents.empty()) {
        return false;
    }
    return requiredComponents.contains(type);
}

bool glimmer::GameSystem::CanRunWhilePaused() const {
    return false;
}

bool glimmer::GameSystem::HandleEvent(const SDL_Event &event) {
    return false;
}

bool glimmer::GameSystem::OnBackPressed() {
    return false;
}

void glimmer::GameSystem::Update(float delta) {
}

uint8_t glimmer::GameSystem::GetRenderOrder() {
    return 0;
}

void glimmer::GameSystem::Render(SDL_Renderer *renderer) {
}
