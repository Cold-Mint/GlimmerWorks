//
// Created by coldmint on 2025/12/23.
//

#include "ComposableItem.h"

#include "AbilityItem.h"

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

void glimmer::ComposableItem::SwapItem(size_t index, ItemContainer *otherContainer, size_t otherIndex) const {
    itemContainer->SwapItem(index, otherContainer, otherIndex);
}

size_t glimmer::ComposableItem::RemoveItemAbility(const std::string &id, size_t amount) const {
    return itemContainer->RemoveItem(id, amount);
}


void glimmer::ComposableItem::OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity *user) {
    const size_t max = itemContainer->GetCapacity();
    for (size_t index = 0; index < max; index++) {
        Item *item = itemContainer->GetItem(index);
        if (item == nullptr) {
            continue;
        }
        auto *abilityItem = dynamic_cast<AbilityItem *>(item);
        if (abilityItem == nullptr) {
            LogCat::e("The combinable items include items of non-abilityItem type.");
            continue;
        }
        ItemAbility *itemAbility = abilityItem->GetItemAbility();
        if (itemAbility == nullptr) {
            LogCat::e("The combinable items include space capacity.");
            continue;
        }
        itemAbility->OnUse(appContext, worldContext, user);
    }
}

std::unique_ptr<glimmer::Item> glimmer::ComposableItem::Clone() const {
    auto newItem = std::make_unique<ComposableItem>(id_, name_, description_, icon_, maxSlotSize_);
    newItem->itemContainer = itemContainer->Clone();
    return newItem;
}

size_t glimmer::ComposableItem::GetMaxSlotSize() const {
    return maxSlotSize_;
}

std::vector<glimmer::AbilityItem *> glimmer::ComposableItem::GetAbilityList() const {
    std::vector<AbilityItem *> items;
    items.reserve(itemContainer->GetCapacity());

    const size_t max = itemContainer->GetCapacity();
    for (size_t index = 0; index < max; index++) {
        Item *item = itemContainer->GetItem(index);
        if (item == nullptr) {
            continue;
        }
        auto *abilityItem = dynamic_cast<AbilityItem *>(item);
        if (abilityItem != nullptr) {
            items.push_back(abilityItem);
        }
    }
    return items;
}
