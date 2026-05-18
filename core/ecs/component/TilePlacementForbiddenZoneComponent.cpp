//
// Created by coldmint on 2026/4/2.
//

#include "TilePlacementForbiddenZoneComponent.h"

#include "core/Constants.h"

void glimmer::TilePlacementForbiddenZoneComponent::SetWidth(const float width) {
    width_ = width;
}

void glimmer::TilePlacementForbiddenZoneComponent::SetOffsetX(const float offsetX) {
    offset_.x = offsetX;
}

void glimmer::TilePlacementForbiddenZoneComponent::SetOffsetY(const float offsetY) {
    offset_.y = offsetY;
}

uint32_t glimmer::TilePlacementForbiddenZoneComponent::GetId() {
    return COMPONENT_ID_TILE_PLACEMENT_FORBIDDEN_ZONE;
}

float glimmer::TilePlacementForbiddenZoneComponent::GetOffsetX() const {
    return offset_.x;
}

float glimmer::TilePlacementForbiddenZoneComponent::GetOffsetY() const {
    return offset_.y;
}

void glimmer::TilePlacementForbiddenZoneComponent::SetHeight(const float height) {
    height_ = height;
}

float glimmer::TilePlacementForbiddenZoneComponent::GetWidth() const {
    return width_;
}

float glimmer::TilePlacementForbiddenZoneComponent::GetHeight() const {
    return height_;
}
