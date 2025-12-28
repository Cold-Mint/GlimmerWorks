//
// Created by Cold-Mint on 2025/12/17.
//

#ifndef GLIMMERWORKS_ITEMCONTAINER_H
#define GLIMMERWORKS_ITEMCONTAINER_H
#include <vector>

#include "Item.h"

namespace glimmer {
    class ItemContainer {
        std::vector<std::unique_ptr<Item> > items_;

        /**
         * capacity
         * 容器的容量
         */
        size_t capacity_ = 0;

    public:
        explicit ItemContainer(const size_t capacity) {
            capacity_ = capacity;
            items_ = std::vector<std::unique_ptr<Item> >(capacity);
        }

        ItemContainer()
            : ItemContainer(10) {
        }


        /**
         * AddItem
         * 添加物品
         * @param item item 物品
         * @return
         */
        [[nodiscard]] std::unique_ptr<Item> AddItem(std::unique_ptr<Item> item);

        /**
         * Remove Item
         * 移除物品
        * @param id id 物品
         * @param amount amount 数量
         * @return
         */
        [[nodiscard]] size_t RemoveItem(const std::string &id, size_t amount);


        /**
         * Get the capacity that has been used
         * 获取已经使用的容量
         * @return
         */
        [[nodiscard]] size_t GetUsedCapacity() const;


        /**
         * Remove Item At Index
         * 移除指定位置的物品
         * @param index
         * @param amount
         * @return
         */
        size_t RemoveItemAt(size_t index, size_t amount);

        /**
         * GetItem
         * 获取物品
         * @param index
         * @return
         */
        [[nodiscard]] Item *GetItem(size_t index) const;

        /**
         * GetCapacity
         * 获取容量
         * @return
         */
        [[nodiscard]] size_t GetCapacity() const;
        /**
         * Take Item At Index (Moves ownership out of container)
         * 取出指定位置的物品（所有权转移）
         * @param index
         * @return
         */
        [[nodiscard]] std::unique_ptr<Item> TakeItem(size_t index);

        /**
         * Swap Item
         * 交换物品
         * @param index Source index in this container
         * @param otherContainer Target container (can be same)
         * @param otherIndex Target index in other container
         * @return true if successful
         */
        bool SwapItem(size_t index, ItemContainer *otherContainer, size_t otherIndex);

        /**
         * Set Item At Index
         * 设置指定位置的物品
         * @param index
         * @param item
         * @return Old item at that index (if any)
         */
        std::unique_ptr<Item> SetItem(size_t index, std::unique_ptr<Item> item);
    };
}

#endif //GLIMMERWORKS_ITEMCONTAINER_H
