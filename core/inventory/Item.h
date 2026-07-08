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
#include <memory>
#include <string>

#include "ContainerChangeType.h"
#include "SDL3/SDL_render.h"
#include "core/scene/AppContext.h"
#include "core/math/IAllocatable.h"

namespace glimmer
{
    /**
     * 物品类
     */
    class Item : public IAllocatable<uint32_t>
    {
        uint8_t amount_ = 1;
        bool locked_ = false;
        uint32_t usedDurability_ = 0;
        std::unordered_map<uint64_t, ItemTagResource> tagMap_;
        std::vector<uint64_t> tags_;

        uint8_t maxStack_ = 1;
        std::function<void(ContainerChangeType, uint8_t)> onAmountChanged_ = nullptr;
        std::function<void(bool)> onLockStatusChanged_ = nullptr;
        std::function<void(uint32_t, uint32_t)> onUsedDurabilityChanged_ = nullptr;
        ResourceRef resourceRef_;

        void SetLockStatus(bool locked);

    protected:
        void SetMaxStack(uint8_t maxStack);

        [[nodiscard]] const ResourceRef& GetResourceRef() const;

        void SetResourceRef(const ResourceRef& resourceRef);

        void SetTags(const std::vector<ItemTagResource>& tags);

    public:
        ~Item() override = default;

        virtual void ReadItemMessage(WorldContext* worldContext, const ItemMessage& itemMessage);

        virtual void WriteItemMessage(ItemMessage& itemMessage) const;

        [[nodiscard]] virtual uint32_t GetMaxDurability() const = 0;

        [[nodiscard]] uint32_t GetUsedDurability() const;

        [[nodiscard]] bool IsLocked() const;

        void Lock();

        void Unlock();

        /**
         * Is it indestructible (with infinite durability)?
         * 是否为坚不可摧的（无限耐久）
         * @return
         */
        [[nodiscard]] virtual bool IsUnbreakable() const = 0;

        /**
          * GetId
          * 获取id
          * @return
          */
        [[nodiscard]] virtual const std::string& GetId() const = 0;


        /**
         * GetCount
         * 获取物品数量
         * @return
         */
        [[nodiscard]] uint8_t GetAmount() const;


        /**
         * Get Max Stack
         * 获取最大堆叠数量
         * @return
         */
        [[nodiscard]] uint8_t GetMaxStack() const;


        /**
         * GetRemainingStackCount
         * 获取可堆叠的数量
         * @return
         */
        [[nodiscard]] uint8_t GetRemainingStackCount(const Item* item) const;


        void SetOnAmountChanged(const std::function<void(ContainerChangeType, uint8_t)>& onAmountChanged);

        void SetOnLockStatusChanged(const std::function<void(bool)>& onLockStatusChanged);

        void SetOnUsedDurabilityChanged(const std::function<void(uint32_t, uint32_t)>& onUsedDurabilityChanged);

        /**
         * SetAmount
         * 设置数量
         * @param amount
         */
        void SetAmount(uint8_t amount);

        /**
         * GetTag
         * 获取标签
         * @param tag tag 标签
         * @return Return the label value. If not found, return nullptr. 返回标签值，若找不到则返回nullptr
         */
        [[nodiscard]] const ItemTagResource* GetItemTagResource(uint64_t tag) const;

        /**
         * HasTag
         * 是否包含某个标签。
         * @param tag tag 标签
         * @return
         */
        [[nodiscard]] bool HasTag(uint64_t tag) const;


        /**
         * GetTags
         * 获取全部标签
         * @return
         */
        [[nodiscard]] const std::vector<uint64_t>& GetTags() const;

        /**
         * AddAmount
         * 添加数量
         * @param amount amount 数量
         * @return 返回成功添加了多少个数量
         */
        [[nodiscard]] uint8_t AddAmount(uint8_t amount);

        /**
         * Remove Amount
         * 移除数量
         * @param amount 移除数量
         * @return 移除了多少个
         */
        [[nodiscard]] uint8_t RemoveAmount(uint8_t amount);


        /**
         *IsStackable
         * 是否可堆叠的
         * @return
         */
        [[nodiscard]] bool IsStackable() const;

        /**
         * GetName
         * 获取物品名称
         * @return
         */
        [[nodiscard]] virtual const std::string& GetName() const = 0;

        /**
         * GetDescription
         * 获取描述
         * @return
         */
        [[nodiscard]] virtual const std::optional<std::string>& GetDescription() const = 0;

        /**
         * Variable configuration for obtaining items
         * 获取物品的变量配置
         * @return
         */
        [[nodiscard]] virtual const AbilityConfig* GetAbilityConfig() const = 0;

        /**
         * 获取图标
         * @return
         */
        [[nodiscard]] virtual SDL_Texture* GetIcon() const = 0;


        virtual void OnUse(WorldContext* worldContext, uint32_t user, const AbilityConfig* abilityConfig,
                           std::unordered_set<std::string, TransparentStringHash, std::equal_to<>>& popupAbility) = 0;

        /**
         * Obtain the remaining durability points.
         * 获取剩余的耐久点数。
         * @return
         */
        [[nodiscard]] unsigned GetRemaining() const override;

        /**
      * Subtract the durability points specified
      * 扣除指定点数的耐久
      * @param value
      * @return The cancelled points, with 0 indicating no cancellation, are used for durability transfer between modules. 抵消的点数，0表示未抵消，用于模块间的耐久度传递。
      */
        void Reduce(unsigned value) override = 0;

        /**
         * Clone
         * 克隆
         * @return
         */
        [[nodiscard]] virtual std::unique_ptr<Item> Clone() const = 0;

        void AddUsedDurability(uint32_t value);

        void RemoveUsedDurability(uint32_t value);

        void SetUsedDurability(uint32_t value);
    };
}
