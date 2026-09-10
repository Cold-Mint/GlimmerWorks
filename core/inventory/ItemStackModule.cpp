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
#include "ItemStackModule.h"
#include "core/log/LogCat.h"

uint8_t glimmer::ItemStackModule::GetAmount() const {
    return amount_;
}

uint8_t glimmer::ItemStackModule::GetMaxStack() const {
    return maxStack_;
}

bool glimmer::ItemStackModule::IsStackable() const {
    return maxStack_ > 1;
}

uint8_t glimmer::ItemStackModule::GetRemainingStackCount() const {
    return maxStack_ - amount_;
}

uint8_t glimmer::ItemStackModule::AddAmount(const uint8_t amount) {
    LogCat::d("stack_add_amount", "ItemStackModule::AddAmount request={} current={} maxStack={}",
              static_cast<int>(amount), static_cast<int>(amount_), static_cast<int>(maxStack_));
    if (amount_ >= maxStack_ || amount <= 0) {
        LogCat::d("stack_add_amount_rejected",
                  "ItemStackModule::AddAmount rejected: full or invalid amount. request={} current={} maxStack={}",
                  static_cast<int>(amount), static_cast<int>(amount_), static_cast<int>(maxStack_));
        return 0;
    }
    const int current = amount_;
    const int addNum = amount;
    const int max = maxStack_;
    if (const int spaceLeft = max - current; addNum > spaceLeft) {
        LogCat::d("stack_add_amount_clamped",
                  "ItemStackModule::AddAmount clamped to max. request={} current={} maxStack={}",
                  static_cast<int>(amount), static_cast<int>(amount_), static_cast<int>(maxStack_));
        SetAmount(maxStack_);
        return static_cast<uint8_t>(spaceLeft);
    }
    SetAmount(static_cast<uint8_t>(current + addNum));
    return amount;
}

uint8_t glimmer::ItemStackModule::RemoveAmount(const uint8_t amount) {
    LogCat::d("stack_remove_amount", "ItemStackModule::RemoveAmount request={} current={}",
              static_cast<int>(amount), static_cast<int>(amount_));
    if (amount_ == 0 || amount == 0) {
        LogCat::d("stack_remove_amount_rejected",
                  "ItemStackModule::RemoveAmount rejected: empty or zero request. request={} current={}",
                  static_cast<int>(amount), static_cast<int>(amount_));
        return 0;
    }
    const int currentCount = amount_;
    const int removeCount = amount;
    const int result = currentCount - removeCount;
    if (result < 0) {
        LogCat::d("stack_remove_amount_clamped", "ItemStackModule::RemoveAmount clamped to zero. request={} current={}",
                  static_cast<int>(amount), static_cast<int>(amount_));
        SetAmount(0);
        return static_cast<uint8_t>(currentCount);
    }
    SetAmount(static_cast<uint8_t>(result));
    return amount;
}

void glimmer::ItemStackModule::SetAmount(const uint8_t amount) {
    if (amount == amount_) {
        return;
    }
    const std::function<void(ContainerChangeType, uint8_t)> onAmountChangedCopy = onAmountChanged_;
    const bool add = amount >= amount_;
    const uint8_t oldAmount = amount_;
    amount_ = std::min(amount, maxStack_);
    LogCat::d("stack_amount_changed", "ItemStackModule amount changed: {} -> {}, increase={}",
              static_cast<int>(oldAmount), static_cast<int>(amount_), add);
    if (onAmountChangedCopy != nullptr) {
        onAmountChangedCopy(
            add ? ContainerChangeType::STACK_AMOUNT_INCREASE : ContainerChangeType::STACK_AMOUNT_DECREASE,
            amount_);
    }
}

void glimmer::ItemStackModule::SetMaxStack(const uint8_t maxStack) {
    maxStack_ = maxStack;
}

void glimmer::ItemStackModule::SetOnAmountChanged(
    const std::function<void(ContainerChangeType, uint8_t)> &onAmountChanged) {
    onAmountChanged_ = onAmountChanged;
}
