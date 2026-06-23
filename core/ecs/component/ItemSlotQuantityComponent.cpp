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
#include "ItemSlotQuantityComponent.h"


DesignDimension glimmer::ItemSlotQuantityComponent::GetPadding() const
{
    return padding_;
}

uint8_t glimmer::ItemSlotQuantityComponent::GetSlotIndex() const
{
    return slotIndex_;
}

void glimmer::ItemSlotQuantityComponent::SetPadding(const DesignDimension padding)
{
    padding_ = padding;
}

glimmer::Item* glimmer::ItemSlotQuantityComponent::GetItem() const
{
    if (itemContainer_ == nullptr)
    {
        return nullptr;
    }
    return itemContainer_->GetItem(slotIndex_);
}

const glimmer::DesignVector2D& glimmer::ItemSlotQuantityComponent::GetPosition() const
{
    return position_;
}

const glimmer::DesignVector2D& glimmer::ItemSlotQuantityComponent::GetSize() const
{
    return size_;
}

void glimmer::ItemSlotQuantityComponent::SetItemContainer(ItemContainer* itemContainer)
{
    itemContainer_ = itemContainer;
}

void glimmer::ItemSlotQuantityComponent::SetSlotIndex(const uint8_t slotIndex)
{
    slotIndex_ = slotIndex;
}

void glimmer::ItemSlotQuantityComponent::AddSelectQuantity()
{
    Item* item = GetItem();
    if (item == nullptr)
    {
        return;
    }
    uint8_t max = item->GetMaxStack();
    if (selectQuantity_ < max)
    {
        selectQuantity_++;
    }
}

void glimmer::ItemSlotQuantityComponent::RemoveSelectQuantity()
{
    if (selectQuantity_ > 0)
    {
        selectQuantity_--;
    }
}

uint8_t glimmer::ItemSlotQuantityComponent::GetSelectQuantity() const
{
    return selectQuantity_;
}

void glimmer::ItemSlotQuantityComponent::SetSize(const DesignVector2D& size)
{
    size_ = size;
}

void glimmer::ItemSlotQuantityComponent::SetPosition(const DesignVector2D& position)
{
    position_ = position;
}

bool glimmer::ItemSlotQuantityComponent::IsHovered() const
{
    return isHovered_;
}

void glimmer::ItemSlotQuantityComponent::SetHovered(bool hovered)
{
    isHovered_ = hovered;
}

GameComponentTypeMessage glimmer::ItemSlotQuantityComponent::GetComponentTypeStatic()
{
    return COMPONENT_ITEM_SLOT_QUANTITY;
}

GameComponentTypeMessage glimmer::ItemSlotQuantityComponent::GetComponentType()
{
    return GetComponentTypeStatic();
}
