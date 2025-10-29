//
// Created by Cold-Mint on 2025/10/28.
//

#include "WorldPositionComponent.h"

void glimmer::WorldPositionComponent::SetPosition(Vector2D newPosition)
{
    position_ = newPosition;
}

void glimmer::WorldPositionComponent::Translate(const Vector2D deltaPosition)
{
    position_ += deltaPosition;
}

glimmer::Vector2D glimmer::WorldPositionComponent::GetPosition() const
{
    return position_;
}
