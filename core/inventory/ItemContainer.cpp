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
#include "ItemContainer.h"

#include "AbilityItem.h"
#include "../log/LogCat.h"
#include "../mod/ResourceLocator.h"
#include "core/world/TileInstancePool.h"
#include "core/world/WorldContext.h"


void glimmer::ItemContainer::BindItemEvent(size_t index, std::unique_ptr<Item> &item) const {
    if (item == nullptr) {
        return;
    }
    InvokeOnContentChanged(index, item.get(), ContainerChangeType::ADD);
    item->SetOnAmountZero([&item]() {
        item.reset();
    });
    item->SetOnAmountChanged([this,index, &item](const ContainerChangeType changeType, size_t amount) {
        InvokeOnContentChanged(index, item.get(), changeType);
    });
}

void glimmer::ItemContainer::UnBindItemEvent(size_t index, const std::unique_ptr<Item> &item) const {
    if (item == nullptr) {
        return;
    }
    InvokeOnContentChanged(index, item.get(), ContainerChangeType::REMOVE);
    item->SetOnAmountZero(nullptr);
}

void glimmer::ItemContainer::
InvokeOnContentChanged(size_t index, Item *item, ContainerChangeType containerChange) const {
    if (onContentChanged_.empty()) {
        return;
    }
    for (auto &onChanged: onContentChanged_) {
        (*onChanged)(index, item, containerChange);
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

std::shared_ptr<std::function<void(size_t, glimmer::Item *, glimmer::ContainerChangeType)> >
glimmer::ItemContainer::AddOnContentChanged(
    const std::function<void(size_t, Item *, ContainerChangeType)> &onContentChanged) {
    auto ptr = std::make_shared<std::function<void(size_t, Item *, ContainerChangeType)> >(onContentChanged);
    onContentChanged_.emplace_back(ptr);
    return ptr; //Return the pointer. The user needs to save this pointer for removal purposes.返回指针，用户需要保存这个指针用于移除
}

void glimmer::ItemContainer::RemoveOnContentChanged(
    const std::shared_ptr<std::function<void(size_t, Item *, ContainerChangeType)> > &onContentChanged) {
    auto it = std::remove_if(
        onContentChanged_.begin(),
        onContentChanged_.end(),
        [&](const std::shared_ptr<std::function<void(size_t, Item *, ContainerChangeType)> > &ptr) {
            return ptr == onContentChanged; // Direct comparison of shared_ptr addresses 直接比较shared_ptr地址
        }
    );
    onContentChanged_.erase(it, onContentChanged_.end());
}

std::unique_ptr<glimmer::Item> glimmer::ItemContainer::AddItem(std::unique_ptr<Item> newItem) {
    if (newItem == nullptr) {
        return nullptr;
    }
    for (size_t i = 0; i < items_.size(); ++i) {
        std::unique_ptr<Item> &currentItem = items_[i];
        if (currentItem == nullptr) {
            currentItem = std::move(newItem);
            BindItemEvent(i, currentItem);
            continue;
        }
        const size_t stackSpace = currentItem->GetRemainingStackCount(newItem.get());
        if (stackSpace == 0) {
            continue;
        }
        const size_t stackedAmount = currentItem->AddAmount(newItem->GetAmount());
        if (stackedAmount == 0) {
            continue;
        }
        if (const size_t removeAmount = newItem->RemoveAmount(stackedAmount); removeAmount == 0) {
            LogCat::w("The removal quantity failed.");
            if (currentItem->RemoveAmount(stackedAmount) == 0) {
                LogCat::w("Failed to remove from the item container.");
            }
        }
        if (newItem->GetAmount() == 0) {
            return nullptr;
        }
    }
    return newItem;
}

int glimmer::ItemContainer::FindIndex(const Item *item) {
    if (item == nullptr) {
        return -1;
    }
    for (size_t i = 0; i < items_.size(); ++i) {
        std::unique_ptr<Item> &currentItem = items_[i];
        if (currentItem == nullptr) {
            continue;
        }
        if (currentItem.get() == item) {
            return i;
        }
    }
    return -1;
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
    UnBindItemEvent(index, oldItem);
    items_[index] = std::move(item);
    BindItemEvent(index, items_[index]);
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
    UnBindItemEvent(index, item);
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

void glimmer::ItemContainer::FromMessage(WorldContext *worldContext, const ItemContainerMessage &message) {
    const AppContext *appContext = worldContext->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    const size_t messageSize = message.itemresourceref_size();
    items_.resize(messageSize);
    for (int i = 0; i < messageSize; ++i) {
        const ItemMessage &itemMessage = message.itemresourceref(i);
        auto item = appContext->GetResourceLocator()->FindItem(worldContext, itemMessage);
        if (item != nullptr) {
            item->SetAmount(itemMessage.amount());
            items_[i] = std::move(item);
            BindItemEvent(i, items_[i]);
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
