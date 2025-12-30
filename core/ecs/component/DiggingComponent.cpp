//
// Created by coldmint on 2025/12/29.
//

#include "DiggingComponent.h"

bool glimmer::DiggingComponent::IsEnable() const {
    return enable_;
}

void glimmer::DiggingComponent::SetEnable(const bool enable) {
    enable_ = enable;
}

glimmer::Vector2D glimmer::DiggingComponent::GetPosition() const {
    return position_;
}

void glimmer::DiggingComponent::SetPosition(const Vector2D &position) {
    position_ = position;
}

float glimmer::DiggingComponent::GetProgress() const {
    return progress_;
}

void glimmer::DiggingComponent::SetLayerType(TileLayerType tileLayerType) {
    layerType_ = tileLayerType;
}

glimmer::TileLayerType glimmer::DiggingComponent::GetLayerType() const {
    return layerType_;
}

void glimmer::DiggingComponent::SetProgress(const float progress) {
    progress_ = progress;
}

void glimmer::DiggingComponent::AddProgress(float progress) {
    progress_ += progress;
}

void glimmer::DiggingComponent::SetEfficiency(float efficiency) {
    efficiency_ = efficiency;
}

float glimmer::DiggingComponent::GetEfficiency() const {
    return efficiency_;
}

void glimmer::DiggingComponent::MarkActive() {
    activeSignal_ = true;
}

bool glimmer::DiggingComponent::CheckAndResetActive() {
    const bool active = activeSignal_;
    activeSignal_ = false;
    return active;
}
