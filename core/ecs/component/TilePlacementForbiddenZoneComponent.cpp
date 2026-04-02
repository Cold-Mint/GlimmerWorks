//
// Created by coldmint on 2026/4/2.
//

#include "TilePlacementForbiddenZoneComponent.h"

#include "core/Constants.h"

void glimmer::TilePlacementForbiddenZoneComponent::SetWidth(int width) {
    width_ = width;
}

void glimmer::TilePlacementForbiddenZoneComponent::SetOffsetX(float offsetX) {
    offsetX_ = offsetX;
}

void glimmer::TilePlacementForbiddenZoneComponent::SetOffsetY(float offsetY) {
    offsetY_ = offsetY;
}

uint32_t glimmer::TilePlacementForbiddenZoneComponent::GetId() {
    return COMPONENT_ID_TILE_PLACEMENT_FORBIDDEN_ZONE;
}

float glimmer::TilePlacementForbiddenZoneComponent::GetOffsetX() const {
    return offsetX_;
}

float glimmer::TilePlacementForbiddenZoneComponent::GetOffsetY() const {
    return offsetY_;
}

void glimmer::TilePlacementForbiddenZoneComponent::SetHeight(int height) {
    height_ = height;
}

int glimmer::TilePlacementForbiddenZoneComponent::GetWidth() const {
    return width_;
}

int glimmer::TilePlacementForbiddenZoneComponent::GetHeight() const {
    return height_;
}
