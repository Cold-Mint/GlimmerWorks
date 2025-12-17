//
// Created by Cold-Mint on 2025/12/17.
//

#include "ItemContainer.h"

#include "../log/LogCat.h"


bool glimmer::ItemContainer::AddItem(Item *item) const {
    for (const auto &i: items_) {
        Item *itemPtr = i.get();
        if (itemPtr == nullptr) {
            //todo:设置新物品到i位置。
            continue;
        }
        if (bool canStackMore = itemPtr->CanStackMore(item); !canStackMore) {
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
            //All the items have been fully distributed.
            //所有的物品已被分配完成
            break;
        }
    }
    return true;
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
