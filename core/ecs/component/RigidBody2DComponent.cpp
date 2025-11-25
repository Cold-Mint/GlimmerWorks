//
// Created by Cold-Mint on 2025/11/6.
//

#include "RigidBody2DComponent.h"

#include "../../log/LogCat.h"
#include "../../utils/Box2DUtils.h"
#include "box2d/box2d.h"


void glimmer::RigidBody2DComponent::CreateBody(const b2WorldId worldId, const b2Vec2 position)
{
    if (ready_)
    {
        return;
    }
    b2BodyDef bodyDef_ = b2DefaultBodyDef();
    bodyDef_.type = bodyType_;
    bodyDef_.position = position;
    bodyDef_.enableSleep = false;
    bodyDef_.fixedRotation = fixedRotation_;
    bodyId_ = b2CreateBody(worldId, &bodyDef_);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0F;
    shapeDef.material.friction = 0.3F;
    const b2Polygon shape = b2MakeBox(
        Box2DUtils::ToMeters(width_ * 0.5F),
        Box2DUtils::ToMeters(height_ * 0.5F)
    );

    b2CreatePolygonShape(bodyId_, &shapeDef, &shape);
    ready_ = true;
}

b2BodyId glimmer::RigidBody2DComponent::GetBodyId() const
{
    return bodyId_;
}

void glimmer::RigidBody2DComponent::SetWidth(const float width)
{
    if (ready_)
    {
        LogCat::d("Cannot change width after creation.");
        return;
    }
    width_ = width;
}

void glimmer::RigidBody2DComponent::SetHeight(const float height)
{
    if (ready_)
    {
        LogCat::d("Cannot change height after creation.");
        return;
    }
    height_ = height;
}

float glimmer::RigidBody2DComponent::GetWidth() const
{
    return width_;
}

float glimmer::RigidBody2DComponent::GetHeight() const
{
    return height_;
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

void glimmer::RigidBody2DComponent::SetEnableSleep(bool enable)
{
    if (ready_)
    {
        LogCat::d("Cannot enableSleep after creation.");
        return;
    }
    enableSleep_ = enable;
}

bool glimmer::RigidBody2DComponent::GetEnableSleep() const
{
    return enableSleep_;
}

void glimmer::RigidBody2DComponent::SetFixedRotation(bool fixedRotation)
{
    if (ready_)
    {
        LogCat::d("Cannot change fixedRotation after creation.");
        return;
    }
    fixedRotation_ = fixedRotation;
}

bool glimmer::RigidBody2DComponent::GetFixedRotation() const
{
    return fixedRotation_;
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
