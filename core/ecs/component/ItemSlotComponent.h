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
#pragma once
#include "core/Constants.h"
#include "core/ecs/GameComponent.h"

namespace glimmer
{
    class Item;
    class ItemContainer;

    class ItemSlotComponent : public GameComponent
    {
        ItemContainer* itemContainer_;
        int slotIndex_;
        bool isHovered_ = false;
        bool isSelected_ = false;
        bool allowSelected_ = false;
        float padding_ = ITEM_SLOT_PADDING;

    public:
        explicit ItemSlotComponent(ItemContainer* itemContainer, int slotIndex, bool allowSelected);

        [[nodiscard]] float GetPadding() const;

        void SetPadding(float padding);

        [[nodiscard]] bool AllowSelected() const;

        [[nodiscard]] Item* GetItem() const;

        [[nodiscard]] std::unique_ptr<Item> TakeAllItem() const;

        [[nodiscard]] std::unique_ptr<Item> ReplaceItem(std::unique_ptr<Item> item) const;

        [[nodiscard]] bool IsHovered() const;

        void SetHovered(bool hovered);

        [[nodiscard]] bool IsSelected() const;

        void SetSelected(bool selected);

        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;
    };
}
