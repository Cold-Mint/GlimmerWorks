//
// Created by coldmint on 2026/5/27.
//

#pragma once
#include <vector>

#include "AllocStrategyType.h"
#include "IAllocatable.h"

namespace glimmer
{
    template <typename T>
    class IAllocStrategy
    {
    public:
        virtual ~IAllocStrategy() = default;

        /**
         * Allocate
         * 执行分配
         * @param items 可分配对象数组
         * @param total 期望扣除的数量
         */
        virtual void Allocate(
            std::vector<IAllocatable<T>*>& items,
            T total
        ) = 0;

        /**
         * 获取当前策略类型
         * 获取当前策略类型
         */
        virtual AllocStrategyType GetStrategyType() const = 0;
    };
}
