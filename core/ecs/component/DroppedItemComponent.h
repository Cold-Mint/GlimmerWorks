//
// Created by coldmint on 2025/12/20.
//

#ifndef GLIMMERWORKS_DROPPEDITEMCOMPONENT_H
#define GLIMMERWORKS_DROPPEDITEMCOMPONENT_H
#include "../GameComponent.h"
#include "../../inventory/Item.h"

namespace glimmer {
    class DroppedItemComponent : public GameComponent {
        std::unique_ptr<Item> item_;
        //How long will it be before it disappears
        //还剩多久会消失(秒)
        float remainingTime_ = 60.0F;

    public:
        explicit DroppedItemComponent(std::unique_ptr<Item> item) : item_(std::move(item)) {
        }

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

        [[nodiscard]] std::unique_ptr<Item> ExtractItem();

        [[nodiscard]] Item *GetItem() const;
    };
}

#endif //GLIMMERWORKS_DROPPEDITEMCOMPONENT_H
