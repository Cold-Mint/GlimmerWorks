//
// Created by Cold-Mint on 2025/10/28.
//

#include "WorldPositionComponent.h"

void glimmer::WorldPositionComponent::SetPosition(WorldVector2D newPosition)
{
    position_ = newPosition;
}

void glimmer::WorldPositionComponent::Translate(const WorldVector2D deltaPosition)
{
    position_ += deltaPosition;
}

glimmer::WorldVector2D glimmer::WorldPositionComponent::GetPosition() const
{
    return position_;
}
