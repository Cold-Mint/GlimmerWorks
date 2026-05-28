//
// Created by Cold-Mint on 2026/4/2.
//

#include "TilePlacementForbiddenZoneComponent.h"


void glimmer::TilePlacementForbiddenZoneComponent::SetWidth(const int width) {
    width_ = width;
}

void glimmer::TilePlacementForbiddenZoneComponent::SetHeight(const int height) {
    height_ = height;
}

void glimmer::TilePlacementForbiddenZoneComponent::SetOffsetX(const int offsetX) {
    offsetX_ = offsetX;
}

void glimmer::TilePlacementForbiddenZoneComponent::SetOffsetY(const int offsetY) {
    offsetY_ = offsetY;
}

int glimmer::TilePlacementForbiddenZoneComponent::GetWidth() const {
    return width_;
}

int glimmer::TilePlacementForbiddenZoneComponent::GetHeight() const {
    return height_;
}

int glimmer::TilePlacementForbiddenZoneComponent::GetOffsetX() const {
    return offsetX_;
}

int glimmer::TilePlacementForbiddenZoneComponent::GetOffsetY() const {
    return offsetY_;
}

GameComponentTypeMessage glimmer::TilePlacementForbiddenZoneComponent::GetComponentType() {
    return COMPONENT_TILE_PLACEMENT_FORBIDDEN_ZONE;
}
