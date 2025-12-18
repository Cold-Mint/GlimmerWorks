//
// Created by coldmint on 2025/12/18.
//

#include "HotBarComonent.h"


glimmer::Vector2D glimmer::HotBarComponent::GetPosition() const {
    return position_;
}

void glimmer::HotBarComponent::SetPosition(const Vector2D position) {
    position_ = position;
}
