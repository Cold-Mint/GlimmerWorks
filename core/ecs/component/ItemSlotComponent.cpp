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
#include "ItemSlotComponent.h"

#include "core/inventory/ItemContainer.h"


glimmer::ItemSlotComponent::ItemSlotComponent(ItemContainer* itemContainer, int slotIndex,
                                              bool allowSelected) : itemContainer_(itemContainer),
                                                                    slotIndex_(slotIndex),
                                                                    allowSelected_(allowSelected)
{
}

float glimmer::ItemSlotComponent::GetPadding() const
{
    return padding_;
}

void glimmer::ItemSlotComponent::SetPadding(const float padding)
{
    padding_ = padding;
}

bool glimmer::ItemSlotComponent::AllowSelected() const
{
    return allowSelected_;
}

glimmer::Item* glimmer::ItemSlotComponent::GetItem() const
{
    return itemContainer_->GetItem(slotIndex_);
}

std::unique_ptr<glimmer::Item> glimmer::ItemSlotComponent::TakeAllItem() const
{
    return itemContainer_->TakeAllItem(slotIndex_);
}

std::unique_ptr<glimmer::Item> glimmer::ItemSlotComponent::ReplaceItem(std::unique_ptr<Item> item) const
{
    return itemContainer_->ReplaceItem(slotIndex_, std::move(item));
}


bool glimmer::ItemSlotComponent::IsHovered() const
{
    return isHovered_;
}

void glimmer::ItemSlotComponent::SetHovered(const bool hovered)
{
    isHovered_ = hovered;
}

bool glimmer::ItemSlotComponent::IsSelected() const
{
    return isSelected_;
}

void glimmer::ItemSlotComponent::SetSelected(const bool selected)
{
    isSelected_ = selected;
}

GameComponentTypeMessage glimmer::ItemSlotComponent::GetComponentType()
{
    return COMPONENT_ITEM_SLOT;
}
