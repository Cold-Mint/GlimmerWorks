//
// Created by Cold-Mint on 2025/11/6.
//

#include "RigidBody2DComponent.h"

#include "../../log/LogCat.h"
#include "box2d/box2d.h"


void glimmer::RigidBody2DComponent::CreateBody(const b2WorldId worldId, const WorldVector2D worldVector2d, const b2Polygon& bodyPolygon)
{
    if (ready_)
    {
        return;
    }
    b2BodyDef bodyDef_ = b2DefaultBodyDef();
    bodyDef_.type = bodyType_;
    bodyDef_.position = b2Vec2(worldVector2d.x, worldVector2d.y);
    bodyId_ = b2CreateBody(worldId, &bodyDef_);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.material.friction = 0.3f;
    b2CreatePolygonShape(bodyId_, &shapeDef, &bodyPolygon);
    ready_ = true;
}

b2BodyId glimmer::RigidBody2DComponent::GetBodyId() const
{
    return bodyId_;
}

bool glimmer::RigidBody2DComponent::IsReady() const
{
    return ready_;
}

void glimmer::RigidBody2DComponent::SetBodyType(const b2BodyType bodyType)
{
    if (ready_)
    {
        LogCat::d("Cannot change body type after creation.");
        return;
    }
    bodyType_ = bodyType;
}

bool glimmer::RigidBody2DComponent::IsDynamicBody() const
{
    return bodyType_ == b2_dynamicBody;
}

bool glimmer::RigidBody2DComponent::IsKinematicBody() const
{
    return bodyType_ == b2_kinematicBody;
}

bool glimmer::RigidBody2DComponent::IsStaticBody() const
{
    return bodyType_ == b2_staticBody;
}
