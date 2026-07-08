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

namespace glimmer
{
    class ItemStackModule
    {
        uint8_t amount_ = 1;
        uint8_t maxStack_ = 1;
        bool stackable_ = false;
        std::function<void(ContainerChangeType, uint8_t)> onAmountChanged_;

    public:
        [[nodiscard]] uint8_t GetAmount() const;

        [[nodiscard]] uint8_t GetMaxStack() const;

        [[nodiscard]] bool IsStackable() const;

        [[nodiscard]] uint8_t GetRemainingStackCount(const ItemStackModule* other) const;

        uint8_t AddAmount(uint8_t amount);

        uint8_t RemoveAmount(uint8_t amount);

        void SetAmount(uint8_t amount);

        void SetMaxStack(uint8_t maxStack);

        void SetOnAmountChanged(const std::function<void(ContainerChangeType, uint8_t)>& onAmountChanged);
    };
}
