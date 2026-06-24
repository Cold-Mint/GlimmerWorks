/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#pragma once
#include <vector>
#include <memory>
#include <functional>
#include "ContainerChangeType.h"
#include "Item.h"
#include "src/saves/item_container.pb.h"

namespace glimmer
{
    class Item;

    class ItemContainer
    {
        std::vector<std::unique_ptr<Item>> items_;
        std::unordered_map<uint64_t, std::unique_ptr<ItemTagResource>> tagToValue_;
        std::vector<ItemTagResource*> totalTagVector_;

        bool needRefreshTag_ = true;


        std::vector<std::shared_ptr<std::function<void(uint8_t, Item*, ContainerChangeType)>>> onContentChanged_;

        /**
         * Binding Item Event
         * 绑定物品事件
         * @param index
         * @param item
         */
        void BindItemEvent(uint8_t index, std::unique_ptr<Item>& item);

        /**
         * Unbinding Item Incident
         * 解绑物品事件
         * @param index
         * @param item
         */
        void UnBindItemEvent(uint8_t index, const std::unique_ptr<Item>& item);

        void InvokeOnContentChanged(uint8_t index, Item* item, ContainerChangeType containerChange);

        void RefreshTotalTags();

    public:
        std::shared_ptr<std::function<void(uint8_t, Item*, ContainerChangeType)>> AddOnContentChanged(
            const std::function<void(uint8_t, Item*, ContainerChangeType)>& onContentChanged);

        /**
         * Set the capacity of the item container.
         * 设置物品容器的容量。
         * @param capacity
         */
        void Resize(uint8_t capacity);

        void RemoveOnContentChanged(
            const std::shared_ptr<std::function<void(uint8_t, Item*, ContainerChangeType)>>& onContentChanged);


        /**
         * AddItem
         * 添加物品
         * @param newItem item 物品
         * @return Added successfully, returned null. 添加成功返回null。
         */
        [[nodiscard]] std::unique_ptr<Item> AddItem(std::unique_ptr<Item> newItem);

        /**
         * Locate the position where the item is located
         * 查找物品所在的位置
         * @param item
         * @return A return value of -1 indicates that the item is not within the container. 返回-1,则表示不在容器内。
         */
        [[nodiscard]] int FindIndex(const Item* item);

        const std::vector<ItemTagResource*>& GetTotalTags();

        bool HasTag(uint64_t tag);

        /**
         * Get Remaining Item Amount After Add
         * 获取添加后的剩余物品数量
         * @param item  item 物品
         * @return 返回0表示可完全添加
         */
        [[nodiscard]] uint8_t GetRemainingItemAmountAfterAdd(const Item* item) const;

        /**
         * ReplaceItem
         * 替换物品
         * @param index index 位置
         * @param item item 物品
         * @return The replaced item 被替换的物品
         */
        [[nodiscard]] std::unique_ptr<Item> ReplaceItem(uint8_t index, std::unique_ptr<Item> item);

        /**
         * Remove Item
         * 移除物品
         * @param id id 物品
         * @param amount amount 数量
         * @return The total number of items successfully removed from the container this time 本次成功从容器中移除掉的物品总数量
         */
        [[nodiscard]] uint8_t RemoveItem(const std::string& id, uint8_t amount) const;


        /**
         * Get the capacity that has been used
         * 获取已经使用的容量
         * @return
         */
        [[nodiscard]] uint8_t GetUsedCapacity() const;


        /**
         * Remove Item At Index
         * 移除指定位置的物品
         * @param index
         * @param amount
         * @return The total number of items successfully removed from the container this time 本次成功从容器中移除掉的物品总数量
         */
        [[nodiscard]] uint8_t RemoveItemAt(uint8_t index, uint8_t amount) const;

        /**
         * GetItem
         * 获取物品
         * @param index
         * @return
         */
        [[nodiscard]] Item* GetItem(uint8_t index) const;

        /**
         * GetCapacity
         * 获取容量
         * @return
         */
        [[nodiscard]] uint8_t GetCapacity() const;

        /**
         * Take Item At Index (Moves ownership out of container)
         * 取出指定位置的物品（所有权转移）
         * @param index
         * @return
         */
        [[nodiscard]] std::unique_ptr<Item> TakeAllItem(uint8_t index);

        /**
         * Take out the specified quantity of items
         * 取出指定数量的物品
         * @param index
         * @param amount
         * @return
         */
        [[nodiscard]] std::unique_ptr<Item> TakeItem(uint8_t index, uint8_t amount) const;

        /**
         * Swap Item
         * 交换物品
         * @param index Source index in this container
         * @param otherContainer Target container (can be same)
         * @param otherIndex Target index in other container
         * @return true if successful
         */
        bool SwapItem(uint8_t index, ItemContainer* otherContainer, uint8_t otherIndex);

        void FromMessage(WorldContext* worldContext, const ItemContainerMessage& message);

        void ToMessage(ItemContainerMessage& message) const;

        void ResetItems();

        [[nodiscard]] std::unique_ptr<ItemContainer> Clone() const;
    };
}
