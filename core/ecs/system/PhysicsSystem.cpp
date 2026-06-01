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
#include "PhysicsSystem.h"

#include "../../utils/Box2DUtils.h"
#include "../../world/WorldContext.h"
#include "box2d/box2d.h"
#include "core/ecs/component/RigidBody2DComponent.h"
#include "core/ecs/component/Transform2DComponent.h"


glimmer::PhysicsSystem::PhysicsSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent(COMPONENT_RIGID_BODY_2D);
}

void glimmer::PhysicsSystem::Update(const float delta) {
    const b2WorldId worldId_ = worldContext_->GetWorldId();
    accumulator_ += delta;
    while (accumulator_ >= FIXED_TIME_STEP) {
        b2World_Step(worldId_, FIXED_TIME_STEP, 4);
        accumulator_ -= FIXED_TIME_STEP;
    }
    for (const auto entities = worldContext_->GetEntityIDWithComponents<RigidBody2DComponent, Transform2DComponent>();
         auto &entity: entities) {
        const RigidBody2DComponent *body = worldContext_->GetComponent<RigidBody2DComponent>(entity);
        if (!body->IsReady() || !body->IsEnabled()) {
            continue;
        }
        auto *transform = worldContext_->GetComponent<Transform2DComponent>(entity);
        const b2Vec2 position = b2Body_GetPosition(body->GetBodyId());
        transform->SetPosition(Box2DUtils::ToPixels(position));
        transform->SetRotation(b2Rot_GetAngle(b2Body_GetRotation(body->GetBodyId())));
    }
}

std::string glimmer::PhysicsSystem::GetName() {
    return "glimmer.PhysicsSystem";
}
