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
        [[nodiscard]] size_t AddItem(Item &item);

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
         * GetCapacity
         * 获取容量
         * @return
         */
        [[nodiscard]] size_t GetCapacity() const;
    };
}

#endif //GLIMMERWORKS_ITEMCONTAINER_H
