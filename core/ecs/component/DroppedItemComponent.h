//
// Created by coldmint on 2025/12/20.
//

#ifndef GLIMMERWORKS_DROPPEDITEMCOMPONENT_H
#define GLIMMERWORKS_DROPPEDITEMCOMPONENT_H
#include "../GameComponent.h"
#include "../../inventory/Item.h"

namespace glimmer {
    class DroppedItemComponent : public GameComponent {
        std::unique_ptr<Item> item_ = nullptr;
        //How long will it be before it disappears
        //还剩多久会消失(秒)
        float remainingTime_ = 60.0F;
        float pickupCooldown_ = 0.0F;

    public:
        explicit DroppedItemComponent();

        /**
         * Get Remaining Time
         * 获取剩余时间
         * @return
         */
        [[nodiscard]] float GetRemainingTime() const;

        /**
         * Set Remaining Time
         * 设置剩余时间
         * @param remainingTime
         */
        void SetRemainingTime(float remainingTime);

        /**
         * Is Expired
         * 是否过期了
         * @return
         */
        [[nodiscard]] bool IsExpired() const;

        void SetItem(std::unique_ptr<Item> item);

        /**
         * ExtractItem
         * 提取物品
         * @return
         */
        [[nodiscard]] std::unique_ptr<Item> ExtractItem();

        [[nodiscard]] Item *GetItem() const;

        /**
         * Set Pickup Cooldown
         * 设置拾取冷却
         * @param cooldown
         */
        void SetPickupCooldown(float cooldown);

        /**
         * Get Pickup Cooldown
         * 获取拾取冷却
         * @return
         */
        [[nodiscard]] float GetPickupCooldown() const;

        /**
         * Can Be Picked Up
         * 是否可以被拾取
         * @return
         */
        [[nodiscard]] bool CanBePickedUp() const;

        [[nodiscard]] u_int32_t GetId() override;

        [[nodiscard]] bool IsSerializable() override;

        [[nodiscard]] std::string Serialize() override;

        void Deserialize(WorldContext *worldContext, const std::string &data) override;
    };
}

#endif //GLIMMERWORKS_DROPPEDITEMCOMPONENT_H
