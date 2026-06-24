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
#include "Item.h"

#include "core/log/LogCat.h"


void glimmer::Item::SetTags(const std::vector<ItemTagResource>& tags)
{
#if  !defined(NDEBUG)
    tags_.clear();
#endif
    tagMap_.clear();
    for (auto& tag : tags)
    {
#if  !defined(NDEBUG)
        tags_.emplace_back(tag);
#endif
        tagMap_.emplace(tag.GetCachedTagId(), tag.value);
    }
}

void glimmer::Item::ReadItemMessage(WorldContext* worldContext, const ItemMessage& itemMessage)
{
    amount_ = itemMessage.amount();
    usedDurability_ = itemMessage.useddurability();
    resourceRef_.ReadResourceRefMessage(itemMessage.itemresourceref());
}

void glimmer::Item::WriteItemMessage(ItemMessage& itemMessage) const
{
    if (resourceRef_.GetResourceType() == RESOURCE_NONE)
    {
        LogCat::e("Item references must be set.");
#if  !defined(NDEBUG)
        assert(false);
#endif
    }
    itemMessage.set_amount(amount_);
    itemMessage.set_useddurability(usedDurability_);
    resourceRef_.WriteResourceRefMessage(*itemMessage.mutable_itemresourceref());
}

uint32_t glimmer::Item::GetUsedDurability() const
{
    return usedDurability_;
}

uint8_t glimmer::Item::GetAmount() const
{
    return amount_;
}

uint8_t glimmer::Item::GetMaxStack() const
{
    return maxStack_;
}

uint8_t glimmer::Item::GetRemainingStackCount(const Item* item) const
{
    if (item == nullptr)
    {
        return 0;
    }
    if (item->GetId() != GetId())
    {
        return 0;
    }
    return maxStack_ - amount_;
}

void glimmer::Item::SetOnAmountChanged(const std::function<void(ContainerChangeType, uint8_t)>& onAmountChanged)
{
    onAmountChanged_ = onAmountChanged;
}

void glimmer::Item::SetOnUsedDurabilityChanged(const std::function<void(uint32_t, uint32_t)>& onUsedDurabilityChanged)
{
    onUsedDurabilityChanged_ = onUsedDurabilityChanged;
}


void glimmer::Item::SetAmount(uint8_t amount)
{
    const bool add = amount >= amount_;
    amount_ = std::min(amount, maxStack_);
    if (onAmountChanged_ != nullptr)
    {
        onAmountChanged_(add ? ContainerChangeType::ADD : ContainerChangeType::REMOVE, amount_);
    }
}

uint8_t glimmer::Item::GetTagValue(const uint64_t tag) const
{
    const auto tagIterator = tagMap_.find(tag);
    if (tagIterator == tagMap_.end())
    {
        return 0;
    }
    return tagIterator->second;
}

bool glimmer::Item::HasTag(const uint64_t tag) const
{
    return tagMap_.contains(tag);
}

#if  !defined(NDEBUG)
const std::vector<glimmer::ItemTagResource>& glimmer::Item::GetTags() const
{
    return tags_;
}
#endif

uint8_t glimmer::Item::AddAmount(uint8_t amount)
{
    if (amount_ >= maxStack_ || amount <= 0)
    {
        return 0;
    }
    const size_t oldAmount = amount;
    const size_t count = amount + amount_;
    if (count > maxStack_)
    {
        SetAmount(maxStack_);
        return maxStack_ - oldAmount;
    }
    SetAmount(count);
    return amount;
}

uint8_t glimmer::Item::RemoveAmount(uint8_t amount)
{
    if (amount_ <= 0 || amount <= 0)
    {
        return 0;
    }
    const int oldAmount = amount_;
    const int newAmount = static_cast<int>(amount_) - static_cast<int>(amount);
    if (newAmount < 0)
    {
        SetAmount(0);
        return oldAmount;
    }
    SetAmount(newAmount);
    return amount;
}


bool glimmer::Item::IsStackable() const
{
    return maxStack_ > 1;
}

unsigned glimmer::Item::GetRemaining() const
{
    return GetMaxDurability() - usedDurability_;
}

void glimmer::Item::AddUsedDurability(const uint32_t value)
{
    const int newValue = static_cast<int>(usedDurability_) + value;
    if (newValue > GetMaxDurability())
    {
        SetUsedDurability(GetMaxDurability());
        return;
    }
    SetUsedDurability(static_cast<uint32_t>(newValue));
}

void glimmer::Item::RemoveUsedDurability(const uint32_t value)
{
    const int newValue = static_cast<int>(usedDurability_) - value;
    if (newValue < 0)
    {
        SetUsedDurability(0);
        return;
    }
    SetUsedDurability(static_cast<uint32_t>(newValue));
}

void glimmer::Item::SetUsedDurability(const uint32_t value)
{
    usedDurability_ = value;
    if (onUsedDurabilityChanged_ != nullptr)
    {
        onUsedDurabilityChanged_(GetMaxDurability(), value);
    }
}
