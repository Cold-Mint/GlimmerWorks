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
#include "core/log/LogCat.h"
#include "core/mod/ResourceLocator.h"
#include "core/world/WorldContext.h"


void glimmer::ItemContainer::BindItemEvent(uint8_t index, std::unique_ptr<Item> &item) {
    if (item == nullptr) {
        return;
    }
    LogCat::d("container_bind_item_event", "Bind item event at slot[{}], itemId={}", static_cast<int>(index),
              item->GetId());
    InvokeOnContentChanged(index, item.get(), ContainerChangeType::STACK_CREATE);
    if (ItemStackModule *itemStackModule = item->GetMutableStackModule(); itemStackModule != nullptr) {
        itemStackModule->SetOnAmountChanged(
            [this,index, &item](const ContainerChangeType changeType, const uint8_t amount) {
                InvokeOnContentChanged(index, item.get(), changeType);
                if (amount == 0) {
                    UnBindItemEvent(item.get());
                    InvokeOnContentChanged(index, item.get(), ContainerChangeType::STACK_AMOUNT_EXHAUSTED);
                    LogCat::d("container_item_stack_exhausted",
                              "Item stack exhausted at slot[{}], removing item", static_cast<int>(index));
                    item.reset();
                }
            });
    }
    if (ItemDurabilityModule *itemDurabilityModule = item->GetMutableDurabilityModule(); itemDurabilityModule !=
        nullptr) {
        itemDurabilityModule->SetOnUsedDurabilityChanged(
            [this,index, &item](ContainerChangeType changeType, uint32_t maxDurability,
                                uint32_t usedDurability) {
                InvokeOnContentChanged(index, item.get(), changeType);
                if (usedDurability >= maxDurability) {
                    UnBindItemEvent(item.get());
                    InvokeOnContentChanged(index, item.get(), ContainerChangeType::STACK_DURABILITY_EXHAUSTED);
                    LogCat::d("container_item_durability_exhausted",
                              "Item durability exhausted at slot[{}], removing item", static_cast<int>(index));
                    item.reset();
                }
            });
    }
    if (ItemLockModule *itemLockModule = item->GetMutableLockModule(); itemLockModule != nullptr) {
        itemLockModule->SetOnLockStatusChanged([this](bool) {
            needRefreshTag_ = true;
        });
    }
}

void glimmer::ItemContainer::UnBindItemEvent(Item *item) {
    if (item == nullptr) {
        return;
    }
    LogCat::d("container_unbind_item_event", "Unbind item event, itemId={}", item->GetId());
    if (ItemStackModule *itemStackModule = item->GetMutableStackModule(); itemStackModule != nullptr) {
        itemStackModule->SetOnAmountChanged(nullptr);
    }
    if (ItemDurabilityModule *itemDurabilityModule = item->GetMutableDurabilityModule(); itemDurabilityModule !=
        nullptr) {
        itemDurabilityModule->SetOnUsedDurabilityChanged(nullptr);
    }
    if (ItemLockModule *itemLockModule = item->GetMutableLockModule(); itemLockModule != nullptr) {
        itemLockModule->SetOnLockStatusChanged(nullptr);
    }
}

void glimmer::ItemContainer::InvokeOnContentChanged(uint8_t index, Item *item, ContainerChangeType containerChange) {
    needRefreshTag_ = true;
    if (onContentChanged_.empty()) {
        return;
    }
    for (const auto &onChanged: onContentChanged_) {
        (*onChanged)(index, item, containerChange);
    }
}

void glimmer::ItemContainer::InvokeOnSelectIndexChanged(uint8_t index) const {
    if (onSelectIndexChanged_.empty()) {
        return;
    }
    for (const auto &onChanged: onSelectIndexChanged_) {
        (*onChanged)(index);
    }
}

void glimmer::ItemContainer::CacheItemTag(Item *item) {
    if (item == nullptr) {
        return;
    }
    const ItemLockModule *itemLockModule = item->GetLockModule();
    if (itemLockModule == nullptr) {
        return;
    }
    if (itemLockModule->IsLocked()) {
        return;
    }
    const ItemStackModule *itemStackModule = item->GetStackModule();
    if (itemStackModule == nullptr) {
        return;
    }
    const uint8_t amount = itemStackModule->GetAmount();
    if (amount == 0) {
        return;
    }
    const ItemTagModule *itemTagModule = item->GetMutableTagModule();
    if (itemTagModule == nullptr) {
        return;
    }
    const std::vector<uint64_t> &tags = itemTagModule->GetTags();
    for (auto &tagId: tags) {
        const ItemTagResource *itemTagResourcePtr = itemTagModule->GetItemTagResource(tagId);
        if (itemTagResourcePtr == nullptr) {
            continue;
        }
        auto tagIterator = tagToValue_.find(tagId);
        if (tagIterator == tagToValue_.end()) {
            ItemTagResource itemTagResource;
            itemTagResource.name = itemTagResourcePtr->name;
            itemTagResource.value = itemTagResourcePtr->value * amount;
            itemTagResource.MakeCachedTag();
            tagToValue_[tagId] = itemTagResource;
        } else {
            tagIterator->second.value += itemTagResourcePtr->value * amount;
        }
    }
}

void glimmer::ItemContainer::RefreshTotalTags() {
    tagToValue_.clear();
    totalTagVector_.clear();
    for (const auto &currentItem: items_) {
        if (currentItem == nullptr) {
            continue;
        }
        Item *currentItemPtr = currentItem.get();
        if (currentItemPtr == nullptr) {
            continue;
        }
        CacheItemTag(currentItemPtr);
    }
    for (const auto &itemTag: tagToValue_ | std::views::values) {
        totalTagVector_.emplace_back(&itemTag);
    }
    needRefreshTag_ = false;
}

std::shared_ptr<std::function<void(uint8_t, glimmer::Item *, glimmer::ContainerChangeType)> >
glimmer::ItemContainer::AddOnContentChanged(
    const std::function<void(uint8_t, Item *, ContainerChangeType)> &onContentChanged) {
    auto ptr = std::make_shared<std::function<void(uint8_t, Item *, ContainerChangeType)> >(onContentChanged);
    onContentChanged_.emplace_back(ptr);
    // Return the pointer. This pointer needs to be saved for removal purposes.
    // 返回指针，需要保存这个指针用于移除。
    return ptr;
}

std::shared_ptr<std::function<void(uint8_t)> > glimmer::ItemContainer::AddOnSelectIndexChanged(
    const std::function<void(uint8_t)> &onSelectIndexChanged) {
    auto ptr = std::make_shared<std::function<void(uint8_t)> >(onSelectIndexChanged);
    onSelectIndexChanged_.emplace_back(ptr);
    // Return the pointer. This pointer needs to be saved for removal purposes.
    // 返回指针，需要保存这个指针用于移除。
    return ptr;
}

void glimmer::ItemContainer::Resize(const uint8_t capacity) {
    LogCat::d("container_resize", "Resize container to capacity={}", static_cast<int>(capacity));
    items_.resize(capacity);
}

void glimmer::ItemContainer::RemoveOnContentChanged(
    const std::shared_ptr<std::function<void(uint8_t, Item *, ContainerChangeType)> > &onContentChanged) {
    auto toRemove = std::ranges::remove_if(
        onContentChanged_,
        [&](const std::shared_ptr<std::function<void(uint8_t, Item *, ContainerChangeType)> > &ptr) {
            return ptr == onContentChanged;
        }
    );
    onContentChanged_.erase(toRemove.begin(), toRemove.end());
}

void glimmer::ItemContainer::RemoveOnSelectIndexChanged(
    const std::shared_ptr<std::function<void(uint8_t)> > &onSelectIndexChanged) {
    auto toRemove = std::ranges::remove_if(
        onSelectIndexChanged_,
        [&](const std::shared_ptr<std::function<void(uint8_t)> > &ptr) {
            return ptr == onSelectIndexChanged;
        }
    );
    onSelectIndexChanged_.erase(toRemove.begin(), toRemove.end());
}

void glimmer::ItemContainer::SetSelectIndex(const uint8_t index) {
    const uint8_t size = items_.size();
    if (size == 0) {
        LogCat::e(std::source_location::current(), "select_empty_container",
                  "Try to select elements in an empty container.");
        selectIndex_ = 0;
        InvokeOnSelectIndexChanged(selectIndex_);
        return;
    }
    if (index >= size) {
        LogCat::w(std::source_location::current(), "index_out_of_range",
                  "Beyond the legal data limits. index = {},size = {}", static_cast<int>(index),
                  static_cast<int>(size));
        selectIndex_ = size - 1;
    } else {
        selectIndex_ = index;
    }
    InvokeOnSelectIndexChanged(selectIndex_);
}

uint8_t glimmer::ItemContainer::GetSelectIndex() const {
    return selectIndex_;
}

std::unique_ptr<glimmer::Item> glimmer::ItemContainer::AddItem(std::unique_ptr<Item> newItem) {
    if (newItem == nullptr) {
        LogCat::w(std::source_location::current(), "add_item_null", "Try to add newItem is null.");
        return nullptr;
    }
    ItemStackModule *newItemStackModule = newItem->GetMutableStackModule();
    if (newItemStackModule == nullptr) {
        LogCat::w(std::source_location::current(), "new_item_stack_module_null", "NewItem ItemStackModule is null.");
        return nullptr;
    }

    LogCat::d("add_item_start", "AddItem start. ItemId={} initAmount={}container capacity={}", newItem->GetId(),
              static_cast<int>(newItemStackModule->GetAmount()), items_.size());

    uint8_t index = 0;
    for (const auto &currentItem: items_) {
        if (currentItem == nullptr) {
            ++index;
            continue;
        }
        if (currentItem->GetId() != newItem->GetId()) {
            ++index;
            continue;
        }
        ItemStackModule *currentItemStackModule = currentItem->GetMutableStackModule();
        if (currentItemStackModule == nullptr) {
            ++index;
            continue;
        }
        const uint8_t remainingStackCount = currentItemStackModule->GetRemainingStackCount();
        if (remainingStackCount == 0) {
            LogCat::d("slot_no_stack_space", "Slot[{}] no stack space. itemId:{}", static_cast<int>(index),
                      currentItem->GetId());
            ++index;
            continue;
        }
        LogCat::d("slot_can_stack", "Slot[{}] can stack. freeSpace:{}slotItemId:{}", static_cast<int>(index),
                  static_cast<int>(remainingStackCount), currentItem->GetId());

        const uint8_t stackedAmount = currentItemStackModule->AddAmount(newItemStackModule->GetAmount());
        if (stackedAmount == 0) {
            LogCat::d("slot_stacked_amount_zero", "Slot[{}] stackedAmount = 0, skip", static_cast<int>(index));
            ++index;
            continue;
        }
        LogCat::d("slot_try_stack_amount", "Slot[{}] try stack amount:{}", static_cast<int>(index),
                  std::to_string(stackedAmount));

        if (newItemStackModule->RemoveAmount(stackedAmount) == 0 && currentItemStackModule->
            RemoveAmount(stackedAmount) == 0) {
            //The attempt to deduct the quantity from the new item to be added failed, and there was also an error in the reduction of the quantity of the items already in the container.
            //在要添加的新物品内扣除数量失败，且在容器内的物品撤销增加的数量也发生了错误。
            LogCat::w(std::source_location::current(), "deduct_quantity_failed",
                      "The attempt to deduct the quantity from the new item to be added failed, and there was also an error in the reduction of the quantity of the items already in the container.");
        }

        LogCat::d("after_stack_remain", "After stack, newItem remain amount:{}",
                  static_cast<int>(newItemStackModule->GetAmount()));
        if (newItemStackModule->GetAmount() == 0) {
            LogCat::d("add_item_all_consumed", "AddItem complete: all consumed, return nullptr");
            return nullptr;
        }
        ++index;
    }

    LogCat::d("stack_loop_finished", "Stack loop finished, start find empty slot. remainAmount={}",
              static_cast<int>(newItemStackModule->GetAmount()));
    index = 0;
    for (auto &currentItem: items_) {
        if (currentItem == nullptr) {
            LogCat::d("found_empty_slot", "Found empty slot[{}], put remaining item", static_cast<int>(index));
            currentItem = std::move(newItem);
            BindItemEvent(index, currentItem);
            LogCat::d("add_item_placed_empty_slot", "AddItem complete: placed into empty slot, return nullptr");
            return nullptr;
        }
        ++index;
    }

    LogCat::d("no_empty_slot_left", "No empty slot left, cannot place. return leftover item. remainAmount:{}",
              static_cast<int>(newItemStackModule->GetAmount()));
    return newItem;
}

int glimmer::ItemContainer::FindIndex(const Item *item) const {
    if (item == nullptr) {
        return -1;
    }
    for (uint8_t i = 0; i < items_.size(); ++i) {
        const std::unique_ptr<Item> &currentItem = items_[i];
        if (currentItem == nullptr) {
            continue;
        }
        if (currentItem.get() == item) {
            return i;
        }
    }
    return -1;
}

const std::vector<const glimmer::ItemTagResource *> &glimmer::ItemContainer::GetTotalTags() {
    if (needRefreshTag_) {
        RefreshTotalTags();
    }
    return totalTagVector_;
}

bool glimmer::ItemContainer::HasTag(uint64_t tag) {
    if (needRefreshTag_) {
        RefreshTotalTags();
    }
    return tagToValue_.contains(tag);
}

uint8_t glimmer::ItemContainer::GetRemainingItemAmountAfterAdd(const Item *item) const {
    if (item == nullptr) {
        LogCat::e(std::source_location::current(), "item_is_null", "item == nullptr");
        return 0;
    }
    const ItemStackModule *itemStackModule = item->GetStackModule();
    if (itemStackModule == nullptr) {
        LogCat::e(std::source_location::current(), "item_stack_module_is_null", "itemStackModule == nullptr");
        return 0;
    }
    uint8_t remainingAmount = itemStackModule->GetAmount();
    for (const auto &slot: items_) {
        if (remainingAmount == 0) {
            break;
        }
        const Item *currentItemPtr = slot.get();
        if (currentItemPtr == nullptr) {
            //Found an empty position.
            //发现空位。
            remainingAmount = 0;
            break;
        }
        const ItemStackModule *currentStackModule = currentItemPtr->GetStackModule();
        if (currentStackModule == nullptr) {
            return 0;
        }
        const uint8_t remainingStackCount = currentStackModule->GetRemainingStackCount();
        if (remainingStackCount == 0) {
            continue;
        }
        if (remainingAmount <= remainingStackCount) {
            remainingAmount = 0;
        } else {
            remainingAmount -= remainingStackCount;
        }
    }
    return remainingAmount;
}

std::unique_ptr<glimmer::Item> glimmer::ItemContainer::ReplaceItem(const uint8_t index, std::unique_ptr<Item> item) {
    if (index >= items_.size()) {
        LogCat::w(std::source_location::current(), "container_replace_item_invalid_index",
                  "ReplaceItem index out of range. index={} size={}", static_cast<int>(index),
                  static_cast<int>(items_.size()));
        return nullptr;
    }
    std::unique_ptr<Item> oldItem = std::move(items_[index]);
    if (oldItem != nullptr) {
        InvokeOnContentChanged(index, oldItem.get(), ContainerChangeType::STACK_DESTROY);
        UnBindItemEvent(oldItem.get());
    }
    items_[index] = std::move(item);
    if (items_[index] != nullptr) {
        BindItemEvent(index, items_[index]);
    }
    LogCat::d("container_replace_item", "Replace item at slot[{}], hadOldItem={} hasNewItem={}",
              static_cast<int>(index), oldItem != nullptr, items_[index] != nullptr);
    return oldItem;
}

uint8_t glimmer::ItemContainer::RemoveItem(std::string_view id, const uint8_t amount) const {
    int unallocatedCount = amount;
    LogCat::d("container_remove_item", "RemoveItem start. itemId={} requestAmount={}", id,
              static_cast<int>(amount));
    for (auto &i: items_) {
        Item *itemPtr = i.get();
        if (itemPtr == nullptr) {
            continue;
        }
        if (itemPtr->GetId() == id) {
            ItemStackModule *itemStackModule = itemPtr->GetMutableStackModule();
            if (itemStackModule == nullptr) {
                continue;
            }
            const uint8_t actualAmount = itemStackModule->RemoveAmount(unallocatedCount);
            if (actualAmount == 0) {
                continue;
            }
            unallocatedCount -= static_cast<int>(actualAmount);
            if (unallocatedCount == 0) {
                break;
            }
        }
    }
    const uint8_t removedCount = static_cast<uint8_t>(amount - unallocatedCount);
    LogCat::d("container_remove_item_complete", "RemoveItem complete. itemId={} removed={}", id,
              static_cast<int>(removedCount));
    return removedCount;
}


uint8_t glimmer::ItemContainer::GetUsedCapacity() const {
    uint8_t count = 0;
    for (const auto &i: items_) {
        if (i != nullptr) {
            count++;
        }
    }
    return count;
}

uint8_t glimmer::ItemContainer::RemoveItemAt(const uint8_t index, const uint8_t amount) const {
    if (index >= items_.size() || items_[index] == nullptr) {
        LogCat::w(std::source_location::current(), "container_remove_item_at_invalid",
                  "RemoveItemAt invalid: index out of range or empty slot. index={} size={}",
                  static_cast<int>(index), static_cast<int>(items_.size()));
        return 0;
    }
    Item *item = items_[index].get();
    if (item == nullptr) {
        return 0;
    }
    ItemStackModule *itemStackModule = item->GetMutableStackModule();
    if (itemStackModule == nullptr) {
        return 0;
    }
    return itemStackModule->RemoveAmount(amount);
}

glimmer::Item *glimmer::ItemContainer::GetItem(const uint8_t index) const {
    if (index >= items_.size()) {
        return nullptr;
    }
    return items_[index].get();
}

uint8_t glimmer::ItemContainer::GetCapacity() const {
    return items_.size();
}

std::unique_ptr<glimmer::Item> glimmer::ItemContainer::TakeAllItem(const uint8_t index) {
    if (index >= items_.size()) {
        return nullptr;
    }
    LogCat::d("container_take_all_item", "Take all items from slot[{}]", static_cast<int>(index));
    std::unique_ptr<Item> item = std::move(items_[index]);
    InvokeOnContentChanged(index, item.get(), ContainerChangeType::STACK_DESTROY);
    UnBindItemEvent(item.get());
    return item;
}

std::unique_ptr<glimmer::Item> glimmer::ItemContainer::TakeItem(const uint8_t index, const uint8_t amount) const {
    if (index >= items_.size()) {
        return nullptr;
    }
    Item *item = items_[index].get();
    if (item == nullptr) {
        return nullptr;
    }
    LogCat::d("container_take_item", "Take item from slot[{}], amount={}", static_cast<int>(index),
              static_cast<int>(amount));
    std::unique_ptr<Item> newItem = item->Clone();
    UnBindItemEvent(newItem.get());
    ItemStackModule *itemStackModule = item->GetMutableStackModule();
    if (itemStackModule == nullptr) {
        return nullptr;
    }
    const uint8_t removedAmount = itemStackModule->RemoveAmount(amount);
    if (removedAmount == 0) {
        return nullptr;
    }
    ItemStackModule *newItemStackModule = newItem->GetMutableStackModule();
    if (newItemStackModule == nullptr) {
        return nullptr;
    }
    newItemStackModule->SetAmount(removedAmount);
    return newItem;
}

bool glimmer::ItemContainer::SwapItem(uint8_t index, ItemContainer *otherContainer, uint8_t otherIndex) {
    LogCat::d("container_swap_item", "SwapItem start. index={} otherIndex={}", static_cast<int>(index),
              static_cast<int>(otherIndex));
    if (otherContainer == nullptr) {
        LogCat::w(std::source_location::current(), "container_swap_item_null",
                  "SwapItem otherContainer is null");
        return false;
    }
    if (index >= items_.size() || otherIndex >= otherContainer->items_.size()) {
        LogCat::w(std::source_location::current(), "container_swap_item_invalid_index",
                  "SwapItem index out of range. index={} size={} otherIndex={} otherSize={}",
                  static_cast<int>(index), static_cast<int>(items_.size()), static_cast<int>(otherIndex),
                  static_cast<int>(otherContainer->items_.size()));
        return false;
    }
    if (this == otherContainer) {
        if (index == otherIndex) {
            return true;
        }
        auto item1 = TakeAllItem(index);
        auto item2 = TakeAllItem(otherIndex);
        (void) ReplaceItem(index, std::move(item2));
        (void) ReplaceItem(otherIndex, std::move(item1));
        return true;
    }
    auto itemThis = ReplaceItem(index, otherContainer->TakeAllItem(otherIndex));
    (void) otherContainer->ReplaceItem(otherIndex, std::move(itemThis));
    return true;
}

void glimmer::ItemContainer::FromMessage(WorldContext *worldContext, const ItemContainerMessage &message) {
    AppContext *appContext = worldContext->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    const uint8_t messageSize = message.itemresourceref_size();
    LogCat::d("container_from_message", "Load container from message, capacity={}",
              static_cast<int>(messageSize));
    Resize(messageSize);
    for (int i = 0; i < messageSize; ++i) {
        const ItemMessage &itemMessage = message.itemresourceref(i);
        auto item = appContext->GetResourceLocator()->FindItem(worldContext, itemMessage);
        if (item != nullptr) {
            items_[i] = std::move(item);
            BindItemEvent(i, items_[i]);
        }
    }
}

void glimmer::ItemContainer::ToMessage(ItemContainerMessage &message) const {
    message.mutable_itemresourceref()->Reserve(
        items_.size()
    );
    for (uint8_t i = 0; i < items_.size(); ++i) {
        //Even if the slot is empty, we create an empty object and put it into the serializer.
        //即使槽位为空，我们创建空对象，放到系列化器内。
        const auto itemMessage = message.add_itemresourceref();
        const auto item = items_[i].get();
        if (item == nullptr) {
            continue;
        }
        item->WriteItemMessage(*itemMessage);
    }
}

void glimmer::ItemContainer::ResetItems() {
    LogCat::d("container_reset_items", "Reset container items, capacity={}",
              static_cast<int>(items_.size()));
    for (uint8_t i = 0; i < items_.size(); ++i) {
        std::unique_ptr<Item> &item = items_[i];
        if (item == nullptr) {
            continue;
        }
        Item *itemPtr = item.get();
        InvokeOnContentChanged(i, itemPtr, ContainerChangeType::STACK_DESTROY);
        UnBindItemEvent(itemPtr);
        items_[i] = nullptr;
    }
}

std::unique_ptr<glimmer::ItemContainer> glimmer::ItemContainer::Clone() const {
    const uint8_t size = items_.size();
    LogCat::d("container_clone", "Clone container, capacity={}", static_cast<int>(size));
    auto newContainer = std::make_unique<ItemContainer>();
    newContainer->Resize(size);
    for (uint8_t i = 0; i < size; ++i) {
        if (items_[i] != nullptr) {
            newContainer->items_[i] = items_[i]->Clone();
        }
    }
    return newContainer;
}
