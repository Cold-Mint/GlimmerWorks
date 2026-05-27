//
// Created by coldmint on 2026/5/27.
//

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

        AllocStrategyType GetStrategyType() const override;
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
            for (auto* item : items)
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
    AllocStrategyType BalanceAllocStrategy<T>::GetStrategyType() const
    {
        return AllocStrategyType::Balance;
    }
}
