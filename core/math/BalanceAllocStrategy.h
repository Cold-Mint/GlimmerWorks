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

#include "IAllocStrategy.h"

namespace glimmer
{
    /**
     * 负载均衡分配策略
     * Load balancing distribution strategy
     * 核心：每次优先扣除 剩余耐久最多 的模块
     * Core: Deduct the module with the highest remaining durability first each time.
     */
    template <typename T>
    class BalanceAllocStrategy : public IAllocStrategy<T>
    {
    public:
        void Allocate(std::vector<IAllocatable<T>*>& items, T total) override;

        [[nodiscard]] AllocStrategyTypeMessage GetStrategyType() const override;
    };

    template <typename T>
    void BalanceAllocStrategy<T>::Allocate(std::vector<IAllocatable<T>*>& items, T total)
    {
        T remaining = total;

        while (remaining > T{})
        {
            IAllocatable<T>* max_item = nullptr;
            T max_value = T{};

            // Traverse all the modules and find the one with the most remaining durability.
            // 遍历所有模块，找到【剩余耐久最多】的那个
            for (auto item : items)
            {
                T rest = item->GetRemaining();
                if (rest > max_value)
                {
                    max_value = rest;
                    max_item = item;
                }
            }
            if (max_item == nullptr || max_value == T{})
            {
                break;
            }
            T cost = std::min(max_value, remaining);
            max_item->Reduce(cost);
            remaining -= cost;
        }
    }

    template <typename T>
    AllocStrategyTypeMessage BalanceAllocStrategy<T>::GetStrategyType() const
    {
        return ALLOC_STRATEGY_BALANCE;
    }
}
