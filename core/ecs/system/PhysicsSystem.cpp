//
// Created by Cold-Mint on 2025/11/6.
//

#include "PhysicsSystem.h"
#include "../../world/WorldContext.h"
#include "box2d/box2d.h"

void glimmer::PhysicsSystem::OnActivationChanged(bool activeStatus)
{
    if (activeStatus)
    {
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = b2Vec2(0.0f, -9.8f);
        worldId_ = b2CreateWorld(&worldDef);
    }
    else
    {
        b2DestroyWorld(worldId_);
        worldId_ = b2_nullWorldId;
    }
}

void glimmer::PhysicsSystem::Update(float delta)
{
    const float fixedTimeStep = 1.0f / 60.0f;
    const int subStepCount = 4;

    static float accumulator = 0.0f;
    accumulator += delta;

    while (accumulator >= fixedTimeStep)
    {
        b2World_Step(worldId_, fixedTimeStep, subStepCount);
        accumulator -= fixedTimeStep;
    }

    // 同步 Box2D 刚体位置回 ECS Transform（如有）
    for (auto& entity : worldContext_->GetEntitiesWithComponents<RigidBody2DComponent, Transform2DComponent>())
    {
        Transform2DComponent* transform = worldContext_->GetComponent<Transform2DComponent>(entity->GetID());

        RigidBody2DComponent* body = worldContext_->GetComponent<RigidBody2DComponent>(entity->GetID());
        body->CreateBody(worldId_, transform->GetPosition());

        b2Vec2 pos = b2Body_GetPosition(body->GetBodyId());
        // b2Rot rot = b2Body_GetRotation(body->GetBodyId());

        transform->SetPosition({pos.x, pos.y});
        // transform.rotation = b2Rot_GetAngle(rot);
    }
}

std::string glimmer::PhysicsSystem::GetName()
{
    return "glimmer.PhysicsSystem";
}
