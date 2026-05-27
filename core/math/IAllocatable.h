//
// Created by coldmint on 2026/5/27.
//

#pragma once
namespace glimmer
{
    /**
     * Capable of being allocated a certain quantity.
     * 可分配数量的对象抽象接口
     */
    template <typename T>
    class IAllocatable
    {
    public:
        virtual ~IAllocatable() = default;


        /**
         * GetRemaining
         * 获取剩余数量。
         * @return
         */
        virtual T GetRemaining() const = 0;

        /**
         * Subtract the specified quantity.
         * 扣除指定数量。
         * @param value 要扣除的数量。
         */
        virtual void Reduce(T value) = 0;
    };
}
