//
// Created by Cold-Mint on 2025/11/6.
//

#include "RigidBody2DComponent.h"

#include "box2d/box2d.h"

void glimmer::RigidBody2DComponent::CreateBody(const b2WorldId worldId, const WorldVector2D worldVector2d)
{
    if (init)
    {
        return;
    }
    b2BodyDef bodyDef_ = b2DefaultBodyDef();
    bodyDef_.type = b2_dynamicBody;
    bodyDef_.position = b2Vec2(worldVector2d.x, worldVector2d.y);
    bodyId_ = b2CreateBody(worldId, &bodyDef_);
    b2Polygon dynamicBox = b2MakeBox(1.0f, 1.0f);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.material.friction = 0.3f;
    b2CreatePolygonShape(bodyId_, &shapeDef, &dynamicBox);
    init = true;
}

b2BodyId glimmer::RigidBody2DComponent::GetBodyId() const
{
    return bodyId_;
}
