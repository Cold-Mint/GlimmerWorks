//
// Created by coldmint on 2026/2/26.
//

#include "AreaMarkerComponent.h"

#include "core/Constants.h"

uint32_t glimmer::AreaMarkerComponent::GetId() {
    return COMPONENT_ID_AREA_MARKER;
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

const SDL_Color &glimmer::AreaMarkerComponent::GetAreaMarkerBorderColor() const {
    return areaMarkerBorder_;
}

const SDL_Color &glimmer::AreaMarkerComponent::GetAreaMarkerFullColor() const {
    return areaMarkerFull_;
}

void glimmer::AreaMarkerComponent::SetAreaMarkerBorderColor(const SDL_Color &color) {
    areaMarkerBorder_ = color;
}

void glimmer::AreaMarkerComponent::SetAreaMarkerFullColor(const SDL_Color &color) {
    areaMarkerFull_ = color;
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

const TileVector2D &glimmer::AreaMarkerComponent::GetStartPoint() const {
    return startPoint_;
}

const TileVector2D &glimmer::AreaMarkerComponent::GetEndPoint() const {
    return endPoint_;
}

bool glimmer::AreaMarkerComponent::IsSerializable() {
    return false;
}
