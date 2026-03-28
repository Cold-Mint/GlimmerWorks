//
// Created by Cold-Mint on 2025/12/17.
//

#ifndef GLIMMERWORKS_ITEMCONTAINER_H
#define GLIMMERWORKS_ITEMCONTAINER_H
#include <vector>
#include <memory>
#include <functional>
#include "ContainerChangeType.h"
#include "Item.h"
#include "src/saves/item_container.pb.h"

namespace glimmer {
    class Item;

    class ItemContainer {
        std::vector<std::unique_ptr<Item> > items_;

        std::vector<std::shared_ptr<std::function<void(ContainerChangeType)> > > onContentChanged_;

        /**
         * Binding Item Event
         * 绑定物品事件
         * @param item
         */
        void BindItemEvent(std::unique_ptr<Item> &item) const;

        /**
         * Unbinding Item Incident
         * 解绑物品事件
         * @param item
         */
        void UnBindItemEvent(const std::unique_ptr<Item> &item) const;

        void InvokeOnContentChanged(ContainerChangeType containerChange) const;

    public:
        explicit ItemContainer(size_t capacity);

        ItemContainer();

        ~ItemContainer();

        std::shared_ptr<std::function<void(ContainerChangeType)> > AddOnContentChanged(
            const std::function<void(ContainerChangeType)> &onContentChanged);

        void RemoveOnContentChanged(const std::shared_ptr<std::function<void(ContainerChangeType)> > &onContentChanged);


        /**
         * AddItem
         * 添加物品
         * @param item item 物品
         * @return
         */
        [[nodiscard]] std::unique_ptr<Item> AddItem(std::unique_ptr<Item> item);

        /**
         * Get Remaining Item Amount After Add
         * 获取添加后的剩余物品数量
         * @param item  item 物品
         * @return 返回0表示可完全添加
         */
        [[nodiscard]] size_t GetRemainingItemAmountAfterAdd(const Item* item) const;

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
        [[nodiscard]] size_t RemoveItemAt(size_t index, size_t amount) const;

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

        void FromMessage(const AppContext *appContext, const ItemContainerMessage &message);

        void ToMessage(ItemContainerMessage &message) const;

        void ResetItems();

        [[nodiscard]] std::unique_ptr<ItemContainer> Clone() const;
    };
}

#endif //GLIMMERWORKS_ITEMCONTAINER_H
