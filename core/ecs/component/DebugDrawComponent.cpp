//
// Created by Cold-Mint on 2025/10/29.
//

#include "DebugDrawComponent.h"

#include "core/Constants.h"

SDL_Color glimmer::DebugDrawComponent::GetColor() const {
    return color_;
}

void glimmer::DebugDrawComponent::SetColor(const SDL_Color &color) {
    color_ = color;
}

glimmer::Vector2D glimmer::DebugDrawComponent::GetSize() const {
    return size_;
}

void glimmer::DebugDrawComponent::SetSize(const Vector2D &size) {
    size_ = size;
}

u_int32_t glimmer::DebugDrawComponent::GetId() {
    return COMPONENT_ID_DEBUG_DRAW;
}
