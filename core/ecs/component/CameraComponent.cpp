//
// Created by Cold-Mint on 2025/10/29.
//
#include "CameraComponent.h"

SDL_FRect glimmer::CameraComponent::GetViewportRect(const Vector2D cameraPosition) const
{
    return {cameraPosition.x - size_.x / 2, cameraPosition.y - size_.y / 2, size_.x, size_.y};
}

glimmer::Vector2D glimmer::CameraComponent::GetViewPortPosition(const Vector2D cameraPosition,
                                                                const Vector2D worldPosition) const
{
    return {worldPosition.x - cameraPosition.x - size_.x / 2, worldPosition.y - cameraPosition.y - size_.y / 2};
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
