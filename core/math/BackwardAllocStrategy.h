//
// Created by Cold-Mint on 2026/5/27.
//

#pragma once
#include "IAllocStrategy.h"

namespace glimmer
{
    /**
     * Reverse allocation strategy: Start deducting from the last module, and stop when the limit is reached. Then switch to the previous module.
     * 倒序分配策略：从最后一个模块开始扣，耗尽再换前一个
     */
    template <typename T>
    class BackwardAllocStrategy : public IAllocStrategy<T>
    {
    public:
        void Allocate(std::vector<IAllocatable<T>*>& items, T total) override;

       [[nodiscard]] AllocStrategyType GetStrategyType() const override;
    };

    template <typename T>
    void BackwardAllocStrategy<T>::Allocate(std::vector<IAllocatable<T>*>& items, T total)
    {
        T remaining = total;

        while (remaining > T{})
        {
            bool has_valid = false;

            // Reverse traversal: From the end to the beginning
            // 倒序遍历：从尾到头
            for (auto it = items.rbegin(); it != items.rend(); ++it)
            {
                IAllocatable<T>* item = *it;
                T rest = item->GetRemaining();
                if (rest <= T{})
                {
                    continue;
                }
                has_valid = true;
                T cost = std::min(rest, remaining);
                item->Reduce(cost);
                remaining -= cost;

                if (remaining <= T{})
                {
                    break;
                }
            }

            if (has_valid)
            {
                break;
            }
        }
    }

    template <typename T>
    AllocStrategyType BackwardAllocStrategy<T>::GetStrategyType() const
    {
        return AllocStrategyType::Backward;
    }
}
