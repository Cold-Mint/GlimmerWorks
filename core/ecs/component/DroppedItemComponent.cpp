//
// Created by coldmint on 2025/12/20.
//

#include "DroppedItemComponent.h"

#include "core/inventory/AbilityItem.h"
#include "core/inventory/ComposableItem.h"
#include "core/mod/ResourceLocator.h"
#include "src/saves/dropped_item.pb.h"

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

u_int32_t glimmer::DroppedItemComponent::GetId() {
    return COMPONENT_ID_DROPPED_ITEM;
}

bool glimmer::DroppedItemComponent::IsSerializable() {
    return true;
}

std::string glimmer::DroppedItemComponent::Serialize() {
    DroppedItemMessage droppedItemMessage;
    auto resourceRef = item_->ToResourceRef();
    if (resourceRef.has_value()) {
        resourceRef->ToMessage(*droppedItemMessage.mutable_item());
    }
    droppedItemMessage.set_pickupcooldown(pickupCooldown_);
    droppedItemMessage.set_remainingtime(remainingTime_);
    return droppedItemMessage.SerializeAsString();
}

void glimmer::DroppedItemComponent::Deserialize(AppContext *appContext, WorldContext *worldContext,const std::string &data) {
    DroppedItemMessage droppedItemMessage;
    droppedItemMessage.ParseFromString(data);
    ResourceRef resourceRef;
    resourceRef.FromMessage(droppedItemMessage.item());
    auto item = appContext->GetResourceLocator()->FindItem(appContext, resourceRef);
    if (item.has_value()) {
        item_ = std::move(item.value());
    }
    pickupCooldown_ = droppedItemMessage.pickupcooldown();
    remainingTime_ = droppedItemMessage.remainingtime();
}
