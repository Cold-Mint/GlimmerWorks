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

void glimmer::DroppedItemComponent::SetItem(std::unique_ptr<Item> item) {
    item_ = std::move(item);
}

std::unique_ptr<glimmer::Item> glimmer::DroppedItemComponent::ExtractItem() {
    return std::move(item_);
}

glimmer::Item *glimmer::DroppedItemComponent::GetItem() const {
    return item_.get();
}

void glimmer::DroppedItemComponent::SetPickupCooldown(const float cooldown) {
    pickupCooldown_ = cooldown;
}

float glimmer::DroppedItemComponent::GetPickupCooldown() const {
    return pickupCooldown_;
}

bool glimmer::DroppedItemComponent::CanBePickedUp() const {
    return pickupCooldown_ <= 0.0F;
}
