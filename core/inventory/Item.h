//
// Created by Cold-Mint on 2025/12/17.
//

#ifndef GLIMMERWORKS_ITEM_H
#define GLIMMERWORKS_ITEM_H
#include <memory>
#include <string>

#include "SDL3/SDL_render.h"
#include "../scene/AppContext.h"
#include "../../core/ecs/GameEntity.h"

namespace glimmer {
    /**
     * 物品类
     */
    class Item {
        size_t amount_ = 1;

        [[nodiscard]] virtual std::optional<ResourceRef> ActualToResourceRef() = 0;

    protected:
        size_t maxStack_ = 1;

        std::function<void()> onAmountZero_ = nullptr;

    public:
        virtual ~Item() = default;


        /**
          * GetId
          * 获取id
          * @return
          */
        [[nodiscard]] virtual std::string GetId() const = 0;


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
         * CanStackMore
         * 是否还能堆叠更多
         * @return
         */
        [[nodiscard]] bool CanStackMore(const Item *item) const;


        /**
         * Set the callback when the quantity is 0.
         * 设置当数量为0时的回调。
         * @param onAmountZero
         */
        void SetOnAmountZero(const std::function<void()> &onAmountZero);

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
         * Convert to resource reference
         * 转为资源引用
         * @return
         */
        [[nodiscard]] std::optional<ResourceRef> ToResourceRef();

        void ApplyResourceRefArgs(const ResourceRef &resourceRef);


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
        [[nodiscard]] virtual std::string GetName() const = 0;

        /**
         * GetDescription
         * 获取描述
         * @return
         */
        [[nodiscard]] virtual std::string GetDescription() const = 0;

        /**
         * 获取图标
         * @return
         */
        [[nodiscard]] virtual std::shared_ptr<SDL_Texture> GetIcon() const = 0;


        virtual void OnUse(WorldContext *worldContext, GameEntity::ID user) = 0;

        /**
         * Clone
         * 克隆
         * @return
         */
        [[nodiscard]] virtual std::unique_ptr<Item> Clone() const = 0;
    };
}

#endif //GLIMMERWORKS_ITEM_H
