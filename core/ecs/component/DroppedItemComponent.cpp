//
// Created by Cold-Mint on 2025/12/20.
//

#include "DroppedItemComponent.h"

#include "core/inventory/AbilityItem.h"
#include "core/mod/ResourceLocator.h"
#include "core/world/WorldContext.h"
#include "src/saves/dropped_item.pb.h"

glimmer::DroppedItemComponent::DroppedItemComponent() = default;

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

uint32_t glimmer::DroppedItemComponent::GetId() {
    return COMPONENT_ID_DROPPED_ITEM;
}

bool glimmer::DroppedItemComponent::IsSerializable() {
    return true;
}

std::string glimmer::DroppedItemComponent::Serialize() {
    DroppedItemMessage droppedItemMessage;
    item_->WriteItemMessage(*droppedItemMessage.mutable_item());
    droppedItemMessage.set_pickupcooldown(pickupCooldown_);
    droppedItemMessage.set_remainingtime(remainingTime_);
    return droppedItemMessage.SerializeAsString();
}

void glimmer::DroppedItemComponent::Deserialize(WorldContext *worldContext, const std::string &data) {
    AppContext *appContext = worldContext->GetAppContext();
    GameComponent::Deserialize(worldContext, data);
    DroppedItemMessage droppedItemMessage;
    if (droppedItemMessage.ParseFromString(data)) {
        auto item = appContext->GetResourceLocator()->FindItem(droppedItemMessage.item());
        if (item != nullptr) {
            item_ = std::move(item);
            item_->ReadItemMessage(appContext, droppedItemMessage.item());
        }
        pickupCooldown_ = droppedItemMessage.pickupcooldown();
        remainingTime_ = droppedItemMessage.remainingtime();
    }
}
