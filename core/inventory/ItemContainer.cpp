//
// Created by Cold-Mint on 2025/12/17.
//

#include "ItemContainer.h"

#include "AbilityItem.h"
#include "ComposableItem.h"
#include "../log/LogCat.h"
#include "../mod/ResourceLocator.h"


void glimmer::ItemContainer::BindItemEvent(std::unique_ptr<Item> &item) {
    if (item == nullptr) {
        return;
    }
    item->SetOnAmountZero([&item]() {
        item.reset();
    });
}

void glimmer::ItemContainer::UnBindItemEvent(const std::unique_ptr<Item> &item) {
    if (item == nullptr) {
        return;
    }
    item->SetOnAmountZero(nullptr);
}

std::unique_ptr<glimmer::Item> glimmer::ItemContainer::AddItem(std::unique_ptr<Item> item) {
    for (auto &i: items_) {
        Item *itemPtr = i.get();
        if (itemPtr == nullptr) {
            i = std::move(item);
            BindItemEvent(i);
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

std::unique_ptr<glimmer::Item> glimmer::ItemContainer::ReplaceItem(const size_t index, std::unique_ptr<Item> item) {
    if (index >= items_.size()) {
        return nullptr;
    }
    std::unique_ptr<Item> oldItem = std::move(items_[index]);
    UnBindItemEvent(oldItem);
    items_[index] = std::move(item);
    BindItemEvent(items_[index]);
    return oldItem;
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
    std::unique_ptr<Item> item = std::move(items_[index]);
    UnBindItemEvent(item);
    return item;
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
    if (otherContainer == nullptr) {
        return false;
    }
    if (index >= items_.size() || otherIndex >= otherContainer->items_.size()) {
        return false;
    }
    if (this == otherContainer) {
        if (index == otherIndex) {
            return true;
        }
        //Exchange items in the same container.
        //在相同容器内交换物品。
        std::swap(items_[index], items_[otherIndex]);
        return true;
    }
    auto itemThis = ReplaceItem(index, otherContainer->TakeAllItem(otherIndex));
    (void) otherContainer->ReplaceItem(otherIndex, std::move(itemThis));
    return true;
}

void glimmer::ItemContainer::FromMessage(AppContext *appContext, const ItemContainerMessage &message) {
    const size_t messageSize = message.itemresourceref_size();
    items_.resize(messageSize);
    for (size_t i = 0; i < messageSize; ++i) {
        ResourceRefMessage resourceRefMessage = message.itemresourceref(i);
        ResourceRef resourceRef;
        resourceRef.FromMessage(resourceRefMessage);
        auto item = appContext->GetResourceLocator()->FindItem(appContext, resourceRef);
        if (item.has_value()) {
            items_[i] = std::move(item.value());
            BindItemEvent(items_[i]);
        }
    }
}

void glimmer::ItemContainer::ToMessage(ItemContainerMessage &message) const {
    message.mutable_itemresourceref()->Reserve(
        items_.size()
    );
    for (size_t i = 0; i < items_.size(); ++i) {
        //Even if the slot is empty, we create an empty object and put it into the serializer.
        //即使槽位为空，我们创建空对象，放到系列化器内。
        const auto refMessage = message.add_itemresourceref();
        auto *item = items_[i].get();
        if (item == nullptr) {
            continue;
        }
        auto resourceRef = item->ToResourceRef();
        if (resourceRef.has_value()) {
            resourceRef->ToMessage(*refMessage);
        }
    }
}

void glimmer::ItemContainer::Clear() {
    items_.clear();
}

std::unique_ptr<glimmer::ItemContainer> glimmer::ItemContainer::Clone() const {
    const size_t size = items_.size();
    auto newContainer = std::make_unique<ItemContainer>(size);
    for (size_t i = 0; i < size; ++i) {
        if (items_[i] != nullptr) {
            newContainer->items_[i] = items_[i]->Clone();
        }
    }
    return newContainer;
}
