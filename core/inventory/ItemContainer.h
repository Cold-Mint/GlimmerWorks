//
// Created by Cold-Mint on 2025/12/17.
//

#ifndef GLIMMERWORKS_ITEMCONTAINER_H
#define GLIMMERWORKS_ITEMCONTAINER_H
#include <vector>

#include "Item.h"
#include "src/saves/item_container.pb.h"

namespace glimmer {
    class ItemContainer {
        std::vector<std::unique_ptr<Item> > items_;

        /**
         * capacity
         * 容器的容量
         */
        size_t capacity_ = 0;

        /**
         * Binding Item Event
         * 绑定物品事件
         * @param item
         */
        static void BindItemEvent(std::unique_ptr<Item>& item);

        /**
         * Unbinding Item Incident
         * 解绑物品事件
         * @param item
         */
        static void UnBindItemEvent(const std::unique_ptr<Item>& item);

    public:
        explicit ItemContainer(const size_t capacity) {
            capacity_ = capacity;
            items_ = std::vector<std::unique_ptr<Item> >(capacity);
        }

        ItemContainer()
            : ItemContainer(0) {
        }


        /**
         * AddItem
         * 添加物品
         * @param item item 物品
         * @return
         */
        [[nodiscard]] std::unique_ptr<Item> AddItem(std::unique_ptr<Item> item);

        /**
         * ReplaceItem
         * 替换物品
         * @param index index 位置
         * @param item item 物品
         * @return The replaced item 被替换的物品
         */
        [[nodiscard]] std::unique_ptr<Item> ReplaceItem(size_t index, std::unique_ptr<Item> item);

        /**
         * Remove Item
         * 移除物品
         * @param id id 物品
         * @param amount amount 数量
         * @return
         */
        [[nodiscard]] size_t RemoveItem(const std::string &id, size_t amount) const;


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
        size_t RemoveItemAt(size_t index, size_t amount) const;

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
        [[nodiscard]] std::unique_ptr<Item> TakeAllItem(size_t index);

        /**
         * 取出指定数量的物品
         * @param index
         * @param amount
         * @return
         */
        [[nodiscard]] std::unique_ptr<Item> TakeItem(size_t index, size_t amount) const;

        /**
         * Swap Item
         * 交换物品
         * @param index Source index in this container
         * @param otherContainer Target container (can be same)
         * @param otherIndex Target index in other container
         * @return true if successful
         */
        bool SwapItem(size_t index, ItemContainer *otherContainer, size_t otherIndex);

        void FromMessage(AppContext *appContext, const ItemContainerMessage &message);

        void ToMessage(ItemContainerMessage &message) const;


        [[nodiscard]] std::unique_ptr<ItemContainer> Clone() const;
    };
}

#endif //GLIMMERWORKS_ITEMCONTAINER_H
