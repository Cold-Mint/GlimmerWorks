//
// Created by Cold-Mint on 2025/12/17.
//

#include "ItemContainer.h"

#include "AbilityItem.h"
#include "../log/LogCat.h"
#include "../mod/ResourceLocator.h"


void glimmer::ItemContainer::BindItemEvent(std::unique_ptr<Item> &item) const {
    if (item == nullptr) {
        return;
    }
    InvokeOnContentChanged(ContainerChangeType::ADD);
    item->SetOnAmountZero([&item]() {
        item.reset();
    });
    item->SetOnAmountChanged([this](const ContainerChangeType changeType, size_t amount) {
        InvokeOnContentChanged(changeType);
    });
}

void glimmer::ItemContainer::UnBindItemEvent(const std::unique_ptr<Item> &item) const {
    if (item == nullptr) {
        return;
    }
    InvokeOnContentChanged(ContainerChangeType::REMOVE);
    item->SetOnAmountZero(nullptr);
}

void glimmer::ItemContainer::InvokeOnContentChanged(ContainerChangeType containerChange) const {
    LogCat::d("ComposableItem ItemContainer InvokeOnContentChanged" + std::to_string(onContentChanged_.size()));
    if (onContentChanged_.empty()) {
        return;
    }
    for (auto &onChanged: onContentChanged_) {
        (*onChanged)(containerChange);
    }
}

glimmer::ItemContainer::ItemContainer(const size_t capacity) {
    items_ = std::vector<std::unique_ptr<Item> >(capacity);
}

glimmer::ItemContainer::ItemContainer()
    : ItemContainer(0) {
}

glimmer::ItemContainer::~ItemContainer() {
    LogCat::w("ComposableItem ItemContainer::~ItemContainer");
}

std::shared_ptr<std::function<void(glimmer::ContainerChangeType)> > glimmer::ItemContainer::AddOnContentChanged(
    const std::function<void(ContainerChangeType)> &onContentChanged) {
    auto ptr = std::make_shared<std::function<void(ContainerChangeType)> >(onContentChanged);
    onContentChanged_.emplace_back(ptr);
    return ptr; //Return the pointer. The user needs to save this pointer for removal purposes.返回指针，用户需要保存这个指针用于移除
}

void glimmer::ItemContainer::RemoveOnContentChanged(
    const std::shared_ptr<std::function<void(ContainerChangeType)> > &onContentChanged) {
    auto it = std::remove_if(
        onContentChanged_.begin(),
        onContentChanged_.end(),
        [&](const std::shared_ptr<std::function<void(ContainerChangeType)> > &ptr) {
            return ptr == onContentChanged; // Direct comparison of shared_ptr addresses 直接比较shared_ptr地址
        }
    );
    onContentChanged_.erase(it, onContentChanged_.end());
}


std::unique_ptr<glimmer::Item> glimmer::ItemContainer::AddItem(std::unique_ptr<Item> item) {
    for (auto &i: items_) {
        Item *itemPtr = i.get();
        if (itemPtr == nullptr) {
            i = std::move(item);
            BindItemEvent(i);
            return nullptr;
        }
        const size_t stackSpace = itemPtr->GetRemainingStackCount(item.get());
        if (stackSpace == 0) {
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

size_t glimmer::ItemContainer::GetRemainingItemAmountAfterAdd(const Item *item) const {
    if (item == nullptr) {
        return 0;
    }
    size_t remainingAmount = item->GetAmount();
    for (const auto &slot: items_) {
        const Item *itemPtr = slot.get();
        if (remainingAmount == 0) {
            break;
        }
        if (itemPtr == nullptr) {
            //Empty slot
            //空槽位
            remainingAmount = 0;
            break;
        }
        const size_t stackSpace = itemPtr->GetRemainingStackCount(item);
        if (stackSpace == 0) {
            //Not stackable
            //不可堆叠
            continue;
        }
        if (remainingAmount <= stackSpace) {
            //If the quantity to be placed is less than or equal to the available quantity, then remainingAmount = 0
            //如果需要放的数量比能放的数量小或相等。那么remainingAmount = 0
            remainingAmount = 0;
        } else {
            remainingAmount -= stackSpace;
        }
    }
    return remainingAmount;
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

size_t glimmer::ItemContainer::RemoveItemAt(const size_t index, const size_t amount) const {
    if (index >= items_.size() || items_[index] == nullptr) {
        return 0;
    }
    Item *item = items_[index].get();
    return item->RemoveAmount(amount);
}

glimmer::Item *glimmer::ItemContainer::GetItem(const size_t index) const {
    if (index >= items_.size()) {
        return nullptr;
    }
    return items_[index].get();
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

void glimmer::ItemContainer::FromMessage(const AppContext *appContext, const ItemContainerMessage &message) {
    const size_t messageSize = message.itemresourceref_size();
    items_.resize(messageSize);
    for (int i = 0; i < messageSize; ++i) {
        const ItemMessage &itemMessage = message.itemresourceref(i);
        auto item = appContext->GetResourceLocator()->FindItem(itemMessage);
        if (item != nullptr) {
            item->SetAmount(itemMessage.amount());
            items_[i] = std::move(item);
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
        const auto itemMessage = message.add_itemresourceref();
        const auto *item = items_[i].get();
        if (item == nullptr) {
            continue;
        }
        item->WriteItemMessage(*itemMessage);
    }
}

void glimmer::ItemContainer::ResetItems() {
    for (size_t i = 0; i < items_.size(); ++i) {
        items_[i] = nullptr;
    }
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
