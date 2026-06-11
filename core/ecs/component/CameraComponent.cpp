/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "CameraComponent.h"

#include "core/math/CoordinateTransformer.h"

bool glimmer::CameraComponent::IsPointInViewport(const WorldVector2D& cameraPosition,
                                                 const WorldVector2D& worldPos) const
{
    const auto viewportRect = CoordinateTransformer::GetViewportRect(cameraPosition, size_, zoom_);
    const SDL_FPoint point = SDL_FPoint(worldPos.x, worldPos.y);
    return SDL_PointInRectFloat(&point, &viewportRect);
}

bool glimmer::CameraComponent::IsRectInViewport(const WorldVector2D& cameraPosition, const SDL_FRect* rect) const
{
    const auto viewportRect = CoordinateTransformer::GetViewportRect(cameraPosition, size_, zoom_);
    return SDL_HasRectIntersectionFloat(&viewportRect, rect);
}

void glimmer::CameraComponent::SetSize(const ScreenVector2D& size)
{
    this->size_ = size;
}


glimmer::ScreenVector2D glimmer::CameraComponent::GetSize() const
{
    return size_;
}


float glimmer::CameraComponent::GetZoom() const
{
    return zoom_;
}

void glimmer::CameraComponent::SetZoom(const float zoom)
{
    this->zoom_ = std::clamp(zoom, 0.2F, 4.0F);
}

GameComponentTypeMessage glimmer::CameraComponent::GetComponentTypeStatic()
{
    return COMPONENT_CAMERA;
}

GameComponentTypeMessage glimmer::CameraComponent::GetComponentType()
{
    return GetComponentTypeStatic();
}
