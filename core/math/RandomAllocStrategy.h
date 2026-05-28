//
// Created by Cold-Mint on 2026/5/27.
//

#pragma once
#include <random>

#include "IAllocStrategy.h"
#include "core/utils/RandomUtils.h"

namespace glimmer
{
    /**
     * 随机分配
     * @tparam T
     */
    template <typename T>
    class RandomAllocStrategy : public IAllocStrategy<T>
    {
    public:
        void Allocate(std::vector<IAllocatable<T>*>& items, T total) override;

        [[nodiscard]] AllocStrategyType GetStrategyType() const override;
    };

    template <typename T>
    void RandomAllocStrategy<T>::Allocate(std::vector<IAllocatable<T>*>& items, T total)
    {
        T remaining = total;
        while (remaining > T{})
        {
            std::vector<IAllocatable<T>*> valid;
            for (auto* item : items)
            {
                if (item->GetRemaining() > T{})
                    valid.push_back(item);
            }

            if (valid.empty())
            {
                break;
            };
            auto* target = valid[RandomUtils::Random<int>(0, static_cast<int>(valid.size()) - 1)];

            // Deduct
            // 扣除
            T cost = std::min(target->GetRemaining(), remaining);
            target->Reduce(cost);
            remaining -= cost;
        }
    }

    template <typename T>
    AllocStrategyType RandomAllocStrategy<T>::GetStrategyType() const
    {
        return AllocStrategyType::Random;
    }
}
