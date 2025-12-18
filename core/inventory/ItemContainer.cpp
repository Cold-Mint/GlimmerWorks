//
// Created by Cold-Mint on 2025/12/17.
//

#include "ItemContainer.h"

#include "../log/LogCat.h"


size_t glimmer::ItemContainer::AddItem(Item &item) {
    size_t count = 0;
    for (auto &i: items_) {
        Item *itemPtr = i.get();
        if (itemPtr == nullptr) {
            i = item.Clone();
            count += item.GetAmount();
            return count;
        }
        if (bool canStackMore = itemPtr->CanStackMore(&item); !canStackMore) {
            continue;
        }
        const size_t stackedAmount = itemPtr->AddAmount(item.GetAmount());
        if (stackedAmount == 0) {
            continue;
        }
        if (const size_t removeAmount = item.RemoveAmount(stackedAmount); removeAmount == 0) {
            LogCat::w("The removal quantity failed.");
            if (itemPtr->RemoveAmount(stackedAmount) == 0) {
                LogCat::w("Failed to remove from the item container.");
            }
        }
        count += stackedAmount;
        if (item.GetAmount() == 0) {
            break;
        }
    }
    return count;
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
