//
// Created by Cold-Mint on 2025/10/29.
//
#include "CameraComponent.h"

SDL_FRect glimmer::CameraComponent::GetViewportRect(const WorldVector2D cameraPosition) const
{
    return {cameraPosition.x - size_.x / 2, cameraPosition.y - size_.y / 2, size_.x, size_.y};
}

glimmer::CameraVector2D glimmer::CameraComponent::GetViewPortPosition(const WorldVector2D cameraPosition,
                                                                      const WorldVector2D worldPosition) const
{
    return {worldPosition.x - cameraPosition.x - size_.x / 2, worldPosition.y - cameraPosition.y - size_.y / 2};
}

bool glimmer::CameraComponent::IsPointInViewport(const WorldVector2D cameraPosition, const WorldVector2D worldPos) const
{
    const auto viewportRect = GetViewportRect(cameraPosition);
    return worldPos.x >= viewportRect.x && worldPos.x <= viewportRect.x + viewportRect.w && worldPos.y >= viewportRect.y
        && worldPos.y <= viewportRect.y + viewportRect.h;
}


void glimmer::CameraComponent::SetSpeed(const float speed)
{
    this->speed_ = speed;
}

void glimmer::CameraComponent::SetSize(const Vector2D size)
{
    this->size_ = size;
}

glimmer::Vector2D glimmer::CameraComponent::GetSize() const
{
    return size_;
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
