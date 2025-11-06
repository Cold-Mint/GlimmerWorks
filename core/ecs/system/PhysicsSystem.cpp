//
// Created by Cold-Mint on 2025/11/6.
//

#include "PhysicsSystem.h"
#include "../../world/WorldContext.h"
#include "box2d/box2d.h"


void glimmer::PhysicsSystem::Update(const float delta)
{
    const b2WorldId worldId_ = worldContext_->GetWorldId();
    accumulator_ += delta;
    //Simulate 4 steps in 0.016 seconds
    //0.016s模拟4步
    while (accumulator_ >= fixedTimeStep_)
    {
        b2World_Step(worldId_, fixedTimeStep_, 4);
        accumulator_ -= fixedTimeStep_;
    }
    for (const auto entities = worldContext_->GetEntitiesWithComponents<RigidBody2DComponent, Transform2DComponent>();
         auto& entity : entities)
    {
        const RigidBody2DComponent* body = worldContext_->GetComponent<RigidBody2DComponent>(entity->GetID());
        if (!body->IsReady())
        {
            continue;
        }
        Transform2DComponent* transform = worldContext_->GetComponent<Transform2DComponent>(entity->GetID());
        b2Vec2 position = b2Body_GetPosition(body->GetBodyId());
        transform->SetPosition({position.x, position.y});
        transform->SetRotation(b2Rot_GetAngle(b2Body_GetRotation(body->GetBodyId())));
    }
}

std::string glimmer::PhysicsSystem::GetName()
{
    return "glimmer.PhysicsSystem";
}
