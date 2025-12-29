//
// Created by coldmint on 2025/12/23.
//

#include "ComposableItem.h"

std::string glimmer::ComposableItem::GetId() const {
    return id_;
}

std::string glimmer::ComposableItem::GetName() const {
    return name_;
}

std::string glimmer::ComposableItem::GetDescription() const {
    return description_;
}

std::shared_ptr<SDL_Texture> glimmer::ComposableItem::GetIcon() const {
    return icon_;
}

void glimmer::ComposableItem::AddItemAbility(ItemAbility *ability) {
    itemAbilityList_.push_back(ability);
}

void glimmer::ComposableItem::RemoveItemAbility(const ItemAbility *ability) {
    auto it = std::remove_if(
        itemAbilityList_.begin(),
        itemAbilityList_.end(),
        [&](ItemAbility *ptr) {
            return ptr == ability;
        }
    );

    itemAbilityList_.erase(it, itemAbilityList_.end());
}

void glimmer::ComposableItem::OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity *user) {
    for (const auto &mod: itemAbilityList_) {
        mod->OnUse(appContext, worldContext, user);
    }
}

void glimmer::ComposableItem::OnDrop() {
}

size_t glimmer::ComposableItem::GetMaxSlotSize() const {
    return maxSlotSize_;
}

const std::vector<glimmer::ItemAbility *> &glimmer::ComposableItem::GetAbilityList() const {
    return itemAbilityList_;
}
