//
// Created by Cold-Mint on 2025/10/28.
//

#include "Transform2DComponent.h"

#include "core/Constants.h"

void glimmer::Transform2DComponent::SetPosition(WorldVector2D newPosition)
{
    position_ = newPosition;
}

void glimmer::Transform2DComponent::SetRotation(const float newRotation)
{
    rotation_ = newRotation;
}

float glimmer::Transform2DComponent::GetRotation() const
{
    return rotation_;
}

void glimmer::Transform2DComponent::Translate(const WorldVector2D deltaPosition)
{
    position_ += deltaPosition;
}

WorldVector2D glimmer::Transform2DComponent::GetPosition() const
{
    return position_;
}

u_int32_t glimmer::Transform2DComponent::GetId() {
    return COMPONENT_ID_TRANSFORM_2D;
}
