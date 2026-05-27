//
// Created by Cold-Mint on 2025/12/17.
//

#pragma once
#include <memory>
#include <string>

#include "ContainerChangeType.h"
#include "SDL3/SDL_render.h"
#include "core/scene/AppContext.h"
#include "core/ecs/GameEntity.h"
#include "core/math/IAllocatable.h"

namespace glimmer
{
    /**
     * 物品类
     */
    class Item : public IAllocatable<uint32_t>
    {
        size_t amount_ = 1;

    protected:
        size_t maxStack_ = 1;

        std::function<void()> onAmountZero_ = nullptr;

        std::function<void(ContainerChangeType, size_t)> onAmountChanged_ = nullptr;

        ResourceRef resourceRef_;

        uint32_t usedDurability_ = 0;

    public:
        ~Item() override = default;

        virtual void ReadItemMessage(WorldContext* worldContext, const ItemMessage& itemMessage);

        virtual void WriteItemMessage(ItemMessage& itemMessage) const;

        [[nodiscard]] virtual uint32_t GetMaxDurability() const = 0;

        [[nodiscard]] uint32_t GetUsedDurability() const;

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
        [[nodiscard]] size_t GetAmount() const;


        /**
         * Get Max Stack
         * 获取最大堆叠数量
         * @return
         */
        [[nodiscard]] size_t GetMaxStack() const;


        /**
         * GetRemainingStackCount
         * 获取可堆叠的数量
         * @return
         */
        [[nodiscard]] size_t GetRemainingStackCount(const Item* item) const;


        /**
         * Set the callback when the quantity is 0.
         * 设置当数量为0时的回调。
         * @param onAmountZero
         */
        void SetOnAmountZero(const std::function<void()>& onAmountZero);

        void SetOnAmountChanged(const std::function<void(ContainerChangeType, size_t)>& onAmountChanged);

        /**
         * SetAmount
         * 设置数量
         * @param amount
         */
        void SetAmount(size_t amount);

        /**
         * AddAmount
         * 添加数量
         * @param amount amount 数量
         * @return 返回成功添加了多少个数量
         */
        [[nodiscard]] size_t AddAmount(size_t amount);

        /**
         * Remove Amount
         * 移除数量
         * @param amount 移除数量
         * @return 移除了多少个
         */
        [[nodiscard]] size_t RemoveAmount(size_t amount);


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


        virtual void OnUse(WorldContext* worldContext, GameEntity::ID user, const AbilityConfig* abilityConfig,
                           std::unordered_set<std::string>& popupAbility) = 0;

        unsigned GetRemaining() const override;

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
    };
}
