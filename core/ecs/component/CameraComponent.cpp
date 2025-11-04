//
// Created by Cold-Mint on 2025/10/29.
//
#include "CameraComponent.h"

#include <algorithm>

SDL_FRect glimmer::CameraComponent::GetViewportRect(const WorldVector2D cameraPosition) const
{
    const float scaledWidth = size_.x / zoom_;
    const float scaledHeight = size_.y / zoom_;
    return {
        cameraPosition.x - scaledWidth * 0.5f,
        cameraPosition.y - scaledHeight * 0.5f,
        scaledWidth,
        scaledHeight
    };
}


glimmer::CameraVector2D glimmer::CameraComponent::GetViewPortPosition(const WorldVector2D cameraPosition,
                                                                      const WorldVector2D worldPosition) const
{
    const float offsetX = (worldPosition.x - cameraPosition.x) * zoom_;
    const float offsetY = (worldPosition.y - cameraPosition.y) * zoom_;
    return {
        size_.x * 0.5f + offsetX,
        size_.y * 0.5f + offsetY
    };
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
    this->zoom_ = std::clamp(zoom, 0.5F, 4.0F);
}
