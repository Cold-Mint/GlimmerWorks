//
// Created by Cold-Mint on 2025/11/6.
//

#include "PhysicsSystem.h"

#include "../../utils/Box2DUtils.h"
#include "../../world/WorldContext.h"
#include "box2d/box2d.h"
#include "core/ecs/component/RigidBody2DComponent.h"
#include "core/ecs/component/Transform2DComponent.h"


glimmer::PhysicsSystem::PhysicsSystem(WorldContext *worldContext) : GameSystem(worldContext) {
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
