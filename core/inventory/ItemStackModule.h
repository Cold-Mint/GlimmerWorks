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
#include <cstdint>
#include <functional>

#include "ContainerChangeType.h"

namespace glimmer {
    /**
     * Item stack module
     * 物品堆叠模块
     * Only retains the item quantity and whether it is stackable. The per-slot stacking
     * limit is provided by the container (ItemContainer::maxStackPerSlot).
     * 仅保留物品数量以及是否可堆叠。每格堆叠上限由容器（ItemContainer::maxStackPerSlot）提供。
     */
    class ItemStackModule {
        std::function<void(ContainerChangeType, uint16_t)> onAmountChanged_ = nullptr;
        uint16_t amount_ = 1;
        bool stackable_ = false;

    public:
        [[nodiscard]] uint16_t GetAmount() const;

        [[nodiscard]] bool IsStackable() const;

        void SetStackable(bool stackable);

        /**
         * Get the remaining stackable quantity for the given per-slot limit.
         * 获取在给定每格上限下剩余的可堆叠数量。
         * @param maxStack maxStack 容器提供的每格堆叠上限
         * @return
         */
        [[nodiscard]] uint16_t GetRemainingStackCount(uint16_t maxStack) const;

        /**
         * AddAmount
         * 添加数量
         * @param amount amount 数量
         * @param maxStack maxStack 容器提供的每格堆叠上限
         * @return The number of successful additions. If it is 0, it indicates that no additions were made. 成功添加的数量，如果为0表示未添加。
         */
        uint16_t AddAmount(uint16_t amount, uint16_t maxStack);

        /**
         * RemoveAmount
         * 扣除数量
         * @param amount amount 数量
         * @return The number of successful deductions, if it is 0, indicates a failure in the deduction process. 成功扣除的数量，如果为0表示扣除失败。
         */
        uint16_t RemoveAmount(uint16_t amount);

        void SetAmount(uint16_t amount);

        void SetOnAmountChanged(const std::function<void(ContainerChangeType, uint16_t)> &onAmountChanged);
    };
}
