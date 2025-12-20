//
// Created by coldmint on 2025/12/20.
//

#include "DroppedItemComponent.h"

float glimmer::DroppedItemComponent::GetRemainingTime() const {
    return remainingTime_;
}

void glimmer::DroppedItemComponent::SetRemainingTime(const float remainingTime) {
    remainingTime_ = remainingTime;
}

bool glimmer::DroppedItemComponent::IsExpired() const {
    return remainingTime_ <= 0.0F;
}

glimmer::Item * glimmer::DroppedItemComponent::GetItem() const {
    return item_.get();
}
