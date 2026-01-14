//
// Created by Cold-Mint on 2025/12/17.
//

#include "ItemContainer.h"

#include "AbilityItem.h"
#include "ComposableItem.h"
#include "TileItem.h"
#include "../log/LogCat.h"
#include "../mod/ResourceLocator.h"


std::unique_ptr<glimmer::Item> glimmer::ItemContainer::AddItem(std::unique_ptr<Item> item) {
    for (auto &i: items_) {
        Item *itemPtr = i.get();
        if (itemPtr == nullptr) {
            i = std::move(item);
            i->SetOnAmountZero([&i]() {
                i.reset();
            });
            return nullptr;
        }
        if (bool canStackMore = itemPtr->CanStackMore(item.get()); !canStackMore) {
            continue;
        }
        const size_t stackedAmount = itemPtr->AddAmount(item->GetAmount());
        if (stackedAmount == 0) {
            continue;
        }
        if (const size_t removeAmount = item->RemoveAmount(stackedAmount); removeAmount == 0) {
            LogCat::w("The removal quantity failed.");
            if (itemPtr->RemoveAmount(stackedAmount) == 0) {
                LogCat::w("Failed to remove from the item container.");
            }
        }
        if (item->GetAmount() == 0) {
            return nullptr;
        }
    }
    return item;
}

size_t glimmer::ItemContainer::RemoveItem(const std::string &id, const size_t amount) const {
    int unallocatedCount = static_cast<int>(amount);
    for (auto &i: items_) {
        Item *itemPtr = i.get();
        if (itemPtr == nullptr) {
            continue;
        }
        if (itemPtr->GetId() == id) {
            const size_t actualAmount = itemPtr->RemoveAmount(unallocatedCount);
            if (actualAmount == 0) {
                LogCat::w("Failed to remove item from item container.");
                continue;
            }
            unallocatedCount -= static_cast<int>(actualAmount);
            if (unallocatedCount == 0) {
                break;
            }
        }
    }
    return amount - unallocatedCount;
}


size_t glimmer::ItemContainer::GetUsedCapacity() const {
    size_t count = 0;
    for (const auto &i: items_) {
        if (i != nullptr) {
            count++;
        }
    }
    return count;
}

size_t glimmer::ItemContainer::GetCapacity() const {
    return items_.size();
}

std::unique_ptr<glimmer::Item> glimmer::ItemContainer::TakeAllItem(const size_t index) {
    if (index >= items_.size()) {
        return nullptr;
    }
    return std::move(items_[index]);
}

std::unique_ptr<glimmer::Item> glimmer::ItemContainer::TakeItem(const size_t index, const size_t amount) const {
    if (index >= items_.size()) {
        return nullptr;
    }
    Item *item = items_[index].get();
    if (item == nullptr) {
        return nullptr;
    }
    std::unique_ptr<Item> newItem = item->Clone();
    const size_t removedAmount = item->RemoveAmount(amount);
    if (removedAmount == 0) {
        return nullptr;
    }
    newItem->SetAmount(removedAmount);
    return newItem;
}

glimmer::Item *glimmer::ItemContainer::GetItem(const size_t index) const {
    if (index >= items_.size()) {
        return nullptr;
    }
    return items_[index].get();
}

size_t glimmer::ItemContainer::RemoveItemAt(const size_t index, const size_t amount) const {
    if (index >= items_.size() || items_[index] == nullptr) {
        return 0;
    }
    Item *item = items_[index].get();
    return item->RemoveAmount(amount);
}

bool glimmer::ItemContainer::SwapItem(size_t index, ItemContainer *otherContainer, size_t otherIndex) {
    if (!otherContainer) return false;
    if (index >= items_.size() || otherIndex >= otherContainer->items_.size()) return false;

    // Swap unique_ptrs
    // If same container
    if (this == otherContainer) {
        if (index == otherIndex) return true;
        std::swap(items_[index], items_[otherIndex]);
        return true;
    }

    // Different containers
    auto itemThis = std::move(items_[index]);
    auto itemOther = std::move(otherContainer->items_[otherIndex]);

    items_[index] = std::move(itemOther);
    otherContainer->items_[otherIndex] = std::move(itemThis);

    return true;
}

void glimmer::ItemContainer::FromMessage(AppContext *appContext, const ItemContainerMessage &message) {
    for (size_t i = 0; i < items_.size(); ++i) {
        ResourceRefMessage resourceRefMessage = message.itemresourceref(i);
        ResourceRef resourceRef;
        resourceRef.FromMessage(resourceRefMessage);
        auto item = appContext->GetResourceLocator()->FindItem(appContext, resourceRef);
        if (item.has_value()) {
            items_[i] = std::move(item.value());
        }
        LogCat::e("Resource Pointers cannot be converted into items.");
    }
}

void glimmer::ItemContainer::ToMessage(ItemContainerMessage &message) const {
    message.mutable_itemresourceref()->Reserve(
        items_.size()
    );
    for (size_t i = 0; i < items_.size(); ++i) {
        if (items_[i] != nullptr) {
            const auto refMessage = message.add_itemresourceref();
            auto *item = items_[i].get();
            if (item == nullptr) {
                continue;
            }
            auto resourceRef = item->ToResourceRef();
            if (!resourceRef.has_value()) {
                continue;
            }
            resourceRef->ToMessage(*refMessage);
        }
    }
}

std::unique_ptr<glimmer::ItemContainer> glimmer::ItemContainer::Clone() const {
    auto newContainer = std::make_unique<ItemContainer>(capacity_);
    for (size_t i = 0; i < items_.size(); ++i) {
        if (items_[i] != nullptr) {
            newContainer->items_[i] = items_[i]->Clone();
        }
    }
    return newContainer;
}
