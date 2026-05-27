//
// Created by coldmint on 2026/5/27.
//

#pragma once
#include <random>

#include "IAllocStrategy.h"

namespace glimmer
{
    /**
     * 随机分配
     * @tparam T
     */
    template <typename T>
    class RandomAllocStrategy : public IAllocStrategy<T>
    {
        static std::mt19937& GetGenerator()
        {
            static std::mt19937 gen(std::random_device{}());
            return gen;
        }

    public:
        void Allocate(std::vector<IAllocatable<T>*>& items, T total) override;

        AllocStrategyType GetStrategyType() const override;
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
            }

            // Pick one randomly
            // 随机选一个
            std::uniform_int_distribution<> dis(0, static_cast<int>(valid.size()) - 1);
            auto* target = valid[dis(GetGenerator())];

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
