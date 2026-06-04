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
#include "AreaMarkerComponent.h"


GameComponentTypeMessage glimmer::AreaMarkerComponent::GetComponentTypeStatic()
{
    return COMPONENT_AREA_MARKER;
}

GameComponentTypeMessage glimmer::AreaMarkerComponent::GetComponentType() {
    return GetComponentTypeStatic();
}

float glimmer::AreaMarkerComponent::GetRemainingTime() const {
    return remainingTime_;
}

void glimmer::AreaMarkerComponent::SetRemainingTime(float remainingTime) {
    if (remainingTime < MAX_REMAINING_TIME) {
        remainingTime_ = remainingTime;
    } else {
        remainingTime_ = MAX_REMAINING_TIME;
    }
}

bool glimmer::AreaMarkerComponent::IsExpired() const {
    return remainingTime_ <= 0.0F;
}

bool glimmer::AreaMarkerComponent::CanDraw() const {
    return !first_;
}

void glimmer::AreaMarkerComponent::Reset() {
    first_ = true;
    remainingTime_ = 0.0F;
    startPoint_.x = 0.0F;
    startPoint_.y = 0.0F;
    endPoint_.x = 0.0F;
    endPoint_.y = 0.0F;
}

void glimmer::AreaMarkerComponent::SetPoint(TileVector2D point) {
    if (first_) {
        startPoint_ = point;
        first_ = false;
    }
    remainingTime_ = MAX_REMAINING_TIME;
    endPoint_ = point;
}

const glimmer::TileVector2D &glimmer::AreaMarkerComponent::GetStartPoint() const {
    return startPoint_;
}

const glimmer::TileVector2D &glimmer::AreaMarkerComponent::GetEndPoint() const {
    return endPoint_;
}
