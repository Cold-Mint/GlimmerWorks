/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "DroppedItemComponent.h"

#include "core/inventory/AbilityItem.h"
#include "core/mod/ResourceLocator.h"
#include "core/world/WorldContext.h"
#include "src/saves/dropped_item.pb.h"

glimmer::DroppedItemComponent::DroppedItemComponent() = default;

uint64_t glimmer::DroppedItemComponent::GetRemainingTicks() const {
    return remainingTicks_.load(std::memory_order_relaxed);
}

void glimmer::DroppedItemComponent::SetRemainingTicks(const uint64_t remainingTicks) {
    remainingTicks_.store(remainingTicks, std::memory_order_relaxed);
}

bool glimmer::DroppedItemComponent::IsExpired() const {
    return GetRemainingTicks() == 0;
}

bool glimmer::DroppedItemComponent::IsDespawnScheduled() const {
    return despawnScheduled_;
}

void glimmer::DroppedItemComponent::SetDespawnScheduled(const bool scheduled) {
    despawnScheduled_ = scheduled;
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

void glimmer::DroppedItemComponent::SetPickupCooldownTicks(const uint64_t cooldownTicks) {
    pickupCooldownTicks_.store(cooldownTicks, std::memory_order_relaxed);
}

uint64_t glimmer::DroppedItemComponent::GetPickupCooldownTicks() const {
    return pickupCooldownTicks_.load(std::memory_order_relaxed);
}

bool glimmer::DroppedItemComponent::CanBePickedUp() const {
    return GetPickupCooldownTicks() == 0;
}

GameComponentTypeMessage glimmer::DroppedItemComponent::GetComponentTypeStatic() {
    return COMPONENT_DROPPED_ITEM;
}

GameComponentTypeMessage glimmer::DroppedItemComponent::GetComponentType() {
    return GetComponentTypeStatic();
}

std::optional<std::string> glimmer::DroppedItemComponent::Serialize() {
    DroppedItemMessage droppedItemMessage;
    if (item_ != nullptr) {
        item_->WriteItemMessage(*droppedItemMessage.mutable_item());
    }
    droppedItemMessage.set_pickupcooldown(static_cast<float>(GetPickupCooldownTicks()));
    droppedItemMessage.set_remainingtime(static_cast<float>(GetRemainingTicks()));
    return droppedItemMessage.SerializeAsString();
}


void glimmer::DroppedItemComponent::Deserialize(WorldContext *worldContext, const std::string &data) {
    const AppContext *appContext = worldContext->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    GameComponent::Deserialize(worldContext, data);
    DroppedItemMessage droppedItemMessage;
    if (droppedItemMessage.ParseFromString(data)) {
        auto item = appContext->GetResourceLocator()->FindItem(worldContext, droppedItemMessage.item());
        if (item != nullptr) {
            item_ = std::move(item);
            item_->ReadItemMessage(worldContext, droppedItemMessage.item());
        }
        SetPickupCooldownTicks(static_cast<uint64_t>(droppedItemMessage.pickupcooldown()));
        SetRemainingTicks(static_cast<uint64_t>(droppedItemMessage.remainingtime()));
    }
}
