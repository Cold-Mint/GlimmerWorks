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

const glimmer::ResourceRef& glimmer::Item::GetResourceRef() const
{
    return resourceRef_;
}

void glimmer::Item::SetResourceRef(const ResourceRef& resourceRef)
{
    resourceRef_ = resourceRef;
}

glimmer::ItemDurabilityModule* glimmer::Item::GetMutableDurabilityModule()
{
    return &itemDurabilityModule_;
}

glimmer::ItemStackModule* glimmer::Item::GetMutableStackModule()
{
    return &itemStackModule_;
}

glimmer::ItemTagModule* glimmer::Item::GetMutableTagModule()
{
    return &itemTagModule_;
}

glimmer::ItemLockModule* glimmer::Item::GetMutableLockModule()
{
    return &itemLockModule_;
}

void glimmer::Item::SetTags(const std::vector<ItemTagResource>& tags)
{
    if (ItemTagModule* itemTagModule = GetMutableTagModule(); itemTagModule != nullptr)
    {
        itemTagModule->SetTags(tags);
    }
}

void glimmer::Item::SetMaxStack(const uint8_t maxStack)
{
    itemStackModule_.SetMaxStack(maxStack);
}

void glimmer::Item::ReadItemMessage(WorldContext* worldContext, const ItemMessage& itemMessage)
{
    uint8_t amount = itemMessage.amount();
    if (amount == 0)
    {
        amount = 1;
    }
    itemStackModule_.SetAmount(amount);
    itemLockModule_.SetLockStatus(itemMessage.locked());
    itemDurabilityModule_.SetUsedDurability(itemMessage.useddurability());
    resourceRef_.ReadResourceRefMessage(itemMessage.itemresourceref());
}

void glimmer::Item::WriteItemMessage(ItemMessage& itemMessage) const
{
    if (resourceRef_.GetResourceType() == RESOURCE_NONE)
    {

#if  !defined(NDEBUG)
        assert(false);
#endif
    }
    itemMessage.set_amount(itemStackModule_.GetAmount());
    itemMessage.set_locked(itemLockModule_.IsLocked());
    itemMessage.set_useddurability(itemDurabilityModule_.GetUsedDurability());
    resourceRef_.WriteResourceRefMessage(*itemMessage.mutable_itemresourceref());
}

const glimmer::ItemDurabilityModule* glimmer::Item::GetDurabilityModule() const
{
    return &itemDurabilityModule_;
}

const glimmer::ItemStackModule* glimmer::Item::GetStackModule() const
{
    return &itemStackModule_;
}

const glimmer::ItemTagModule* glimmer::Item::GetTagModule() const
{
    return &itemTagModule_;
}

const glimmer::ItemLockModule* glimmer::Item::GetLockModule() const
{
    return &itemLockModule_;
}

unsigned glimmer::Item::GetRemaining() const
{
    return itemDurabilityModule_.GetMaxDurability() - itemDurabilityModule_.GetUsedDurability();
}

void glimmer::Item::Reduce(const unsigned value)
{
    itemDurabilityModule_.AddUsedDurability(value);
}
