//
// Created by Cold-Mint on 2025/12/17.
//

#include "ItemContainer.h"

#include "../log/LogCat.h"


std::unique_ptr<glimmer::Item> glimmer::ItemContainer::AddItem(std::unique_ptr<Item> item) {
    for (auto &i: items_) {
        Item *itemPtr = i.get();
        if (itemPtr == nullptr) {
            i = std::move(item);
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
            break;
        }
    }
    return item;
}

size_t glimmer::ItemContainer::RemoveItem(const std::string &id, const size_t amount) {
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
            if (itemPtr->GetAmount() == 0) {
                i.reset();
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

glimmer::Item *glimmer::ItemContainer::GetItem(size_t index) const {
    if (index >= items_.size()) {
        return nullptr;
    }
    return items_[index].get();
}

size_t glimmer::ItemContainer::RemoveItemAt(const size_t index, const size_t amount) {
    if (index >= items_.size() || items_[index] == nullptr) {
        return 0;
    }
    Item *item = items_[index].get();
    size_t removed = item->RemoveAmount(amount);
    if (item->GetAmount() == 0) {
        items_[index].reset();
    }
    return removed;
}

std::unique_ptr<glimmer::Item> glimmer::ItemContainer::TakeItem(size_t index) {
    if (index >= items_.size()) {
        return nullptr;
    }
    return std::move(items_[index]);
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
