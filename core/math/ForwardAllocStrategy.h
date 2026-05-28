//
// Created by Cold-Mint on 2026/5/27.
//

#pragma once
#include "IAllocStrategy.h"

namespace glimmer
{
    /**
     * Sequential allocation strategy: Start deducting from the first module, and continue until all is used up before moving on to the next one.
     * 正序分配策略：从第一个模块开始扣，耗尽再换下一个
     * @tparam T
     */
    template <typename T>
    class ForwardAllocStrategy : public IAllocStrategy<T>
    {
    public:
        void Allocate(std::vector<IAllocatable<T>*>& items, T total) override;

        [[nodiscard]] AllocStrategyType GetStrategyType() const override;
    };

    template <typename T>
    void ForwardAllocStrategy<T>::Allocate(std::vector<IAllocatable<T>*>& items, T total)
    {
        T remaining = total;

        while (remaining > T{})
        {
            bool hasValid = false;

            for (auto* item : items)
            {
                T rest = item->GetRemaining();
                if (rest <= T{})
                {
                    continue;
                }

                hasValid = true;
                T cost = std::min(rest, remaining);
                item->Reduce(cost);
                remaining -= cost;

                if (remaining <= T{})
                {
                    break;
                }
            }

            if (!hasValid)
            {
                break;
            }
        }
    }

    template <typename T>
    AllocStrategyType ForwardAllocStrategy<T>::GetStrategyType() const
    {
        return AllocStrategyType::Forward;
    }
}
