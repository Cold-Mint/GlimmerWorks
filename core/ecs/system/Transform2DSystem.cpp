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
#include "Transform2DSystem.h"

#include <algorithm>
#include <chrono>

#include "core/config/Constants.h"
#include "core/world/WorldContext.h"

glimmer::Transform2DSystem::Transform2DSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    WatchComponent(COMPONENT_TRANSFORM_2D);
    Init();
}

void glimmer::Transform2DSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) {
    EntityManager *entityManager = GetEntityManager();
    transform2DComponent_.clear();
    auto transform2DEntities = entityManager->GetEntityIDWithComponents({COMPONENT_TRANSFORM_2D});
    for (auto &entity: transform2DEntities) {
        auto transform2DComponent = entityManager->GetComponent<Transform2DComponent>(entity);
        if (transform2DComponent == nullptr) {
            continue;
        }
        transform2DComponent_.emplace_back(transform2DComponent);
    }
}

uint8_t glimmer::Transform2DSystem::GetExecutionOrder() {
    return 0;
}

void glimmer::Transform2DSystem::OnFrameStart() {
    using Clock = std::chrono::steady_clock;
    const Clock::time_point now = Clock::now();
    const int64_t nowCount = now.time_since_epoch().count();
    const int64_t lastTickCount = lastTickTimestamp_.load(std::memory_order_acquire);
    const auto elapsed = Clock::duration(nowCount - lastTickCount);
    const float elapsedSeconds = std::chrono::duration<float>(elapsed).count();
    const float alpha = std::clamp(elapsedSeconds / FIXED_TIME_STEP, 0.0F, 1.0F);
    for (const auto transform2d: transform2DComponent_) {
        transform2d->Commit(alpha);
    }
}

void glimmer::Transform2DSystem::OnTick(uint64_t tick) {
    lastTickTimestamp_.store(std::chrono::steady_clock::now().time_since_epoch().count(),
                             std::memory_order_release);
}

glimmer::GameSystemType glimmer::Transform2DSystem::GetGameSystemType() const {
    return GameSystemType::Transform2DSystem;
}
