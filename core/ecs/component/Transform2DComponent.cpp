//
// Created by Cold-Mint on 2025/10/28.
//

#include "Transform2DComponent.h"

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

glimmer::WorldVector2D glimmer::Transform2DComponent::GetPosition() const
{
    return position_;
}
