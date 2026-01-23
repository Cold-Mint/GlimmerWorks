//
// Created by coldmint on 2025/12/23.
//

#include "ComposableItem.h"

#include "AbilityItem.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/world/WorldContext.h"

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
    itemContainer_->SwapItem(index, otherContainer, otherIndex);
}

std::unique_ptr<glimmer::Item> glimmer::ComposableItem::ReplaceItem(const size_t index,
                                                                    std::unique_ptr<Item> item) const {
    return itemContainer_->ReplaceItem(index, std::move(item));
}

size_t glimmer::ComposableItem::RemoveItemAbility(const std::string &id, const size_t amount) const {
    return itemContainer_->RemoveItem(id, amount);
}

void glimmer::ComposableItem::OnUse(AppContext *appContext, WorldContext *worldContext, const GameEntity::ID user) {
    const size_t max = itemContainer_->GetCapacity();
    for (size_t index = 0; index < max; index++) {
        Item *item = itemContainer_->GetItem(index);
        if (item == nullptr) {
            continue;
        }
        auto *abilityItem = dynamic_cast<AbilityItem *>(item);
        if (abilityItem == nullptr) {
            //Throwing items that are not items of ability.
            //扔出不为能力物品的物品。
            worldContext->CreateDroppedItemEntity(
                itemContainer_->TakeAllItem(index),
                worldContext->GetCameraTransform2D()->GetPosition(),
                2
            );
            continue;
        }
        ItemAbility *itemAbility = abilityItem->GetItemAbility();
        if (itemAbility == nullptr) {
            //Throwing items that are not items of ability.
            //扔出不为能力物品的物品。
            worldContext->CreateDroppedItemEntity(
                itemContainer_->TakeAllItem(index),
                worldContext->GetCameraTransform2D()->GetPosition(),
                2
            );
            continue;
        }
        itemAbility->OnUse(appContext, worldContext, user);
    }
}

glimmer::ItemContainer *glimmer::ComposableItem::GetItemContainer() const {
    return itemContainer_.get();
}

std::unique_ptr<glimmer::Item> glimmer::ComposableItem::Clone() const {
    auto newItem = std::make_unique<ComposableItem>(id_, name_, description_, icon_, maxSlotSize_);
    newItem->itemContainer_ = itemContainer_->Clone();
    return newItem;
}

std::optional<glimmer::ResourceRef> glimmer::ComposableItem::ActualToResourceRef() {
    std::optional<ResourceRef> resourceRef = Resource::ParseFromId(id_, RESOURCE_TYPE_COMPOSABLE_ITEM);
    size_t size = itemContainer_->GetCapacity();
    for (int i = 0; i < size; i++) {
        Item *item = itemContainer_->GetItem(i);
        if (item == nullptr) {
            continue;
        }
        auto ref = item->ToResourceRef();
        if (ref.has_value()) {
            ResourceRefArg refArg;
            refArg.SetName("item_" + std::to_string(i));
            refArg.SetDataFromResourceRef(ref.value());
            resourceRef->AddArg(refArg);
        }
    }
    return resourceRef;
}
