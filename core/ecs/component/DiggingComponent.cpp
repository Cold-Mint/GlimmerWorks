//
// Created by coldmint on 2025/12/29.
//

#include "DiggingComponent.h"

#include "../../log/LogCat.h"

bool glimmer::DiggingComponent::IsEnable() const {
    return enable_;
}

void glimmer::DiggingComponent::SetEnable(const bool enable) {
    enable_ = enable;
}

glimmer::Vector2D glimmer::DiggingComponent::GetPosition() const {
    LogCat::d("get diggingComponent ",position_.x," ",position_.y);
    return position_;
}

void glimmer::DiggingComponent::SetPosition(const Vector2D &position) {
    LogCat::d("set diggingComponent ",position.x," ",position.y);
    position_ = position;
}

float glimmer::DiggingComponent::GetProgress() const {
    return progress_;
}

void glimmer::DiggingComponent::SetProgress(const float progress) {
    progress_ = progress;
}
