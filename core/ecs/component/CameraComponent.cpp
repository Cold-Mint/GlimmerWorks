//
// Created by Cold-Mint on 2025/10/29.
//
#include "CameraComponent.h"

SDL_FRect glimmer::CameraComponent::GetViewport(const Vector2D worldPosition) const
{
    return {worldPosition.x, worldPosition.y, size_.x, size_.y};
}

void glimmer::CameraComponent::SetSpeed(const float speed)
{
    this->speed_ = speed;
}

float glimmer::CameraComponent::GetSpeed() const
{
    return speed_;
}

float glimmer::CameraComponent::GetZoom() const
{
    return zoom_;
}

void glimmer::CameraComponent::SetZoom(const float zoom)
{
    this->zoom_ = zoom;
}
