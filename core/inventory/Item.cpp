//
// Created by Cold-Mint on 2025/12/17.
//

#include "Item.h"


size_t glimmer::Item::GetAmount() const {
    return amount_;
}

size_t glimmer::Item::GetMaxStack() const {
    return maxStack_;
}


bool glimmer::Item::CanStackMore(const Item *item) const {
    if (item->GetId() != GetId()) {
        return false;
    }
    return amount_ <= maxStack_;
}

void glimmer::Item::SetOnAmountZero(const std::function<void()> &onAmountZero) {
    onAmountZero_ = onAmountZero;
}

void glimmer::Item::SetAmount(const size_t amount) {
    amount_ = std::min(amount, maxStack_);
    if (amount_ == 0) {
        onAmountZero_();
    }
}

size_t glimmer::Item::AddAmount(const size_t amount) {
    if (amount_ >= maxStack_ || amount <= 0) {
        return 0;
    }
    const size_t oldAmount = amount;
    const size_t count = amount + amount_;
    if (count > maxStack_) {
        SetAmount(maxStack_);
        return maxStack_ - oldAmount;
    }
    SetAmount(count);
    return amount;
}

size_t glimmer::Item::RemoveAmount(const size_t amount) {
    if (amount_ <= 0 || amount <= 0) {
        return 0;
    }
    const int oldAmount = static_cast<int>(amount_);
    const int newAmount = static_cast<int>(amount_) - static_cast<int>(amount);
    if (newAmount < 0) {
        SetAmount(0);
        return oldAmount;
    }
    SetAmount(newAmount);
    return amount;
}


bool glimmer::Item::IsStackable() const {
    return maxStack_ > 1;
}
