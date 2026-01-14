//
// Created by Cold-Mint on 2025/12/18.
//

#ifndef GLIMMERWORKS_ITEMCONTAINERCOMONENT_H
#define GLIMMERWORKS_ITEMCONTAINERCOMONENT_H
#include "../GameComponent.h"
#include "../../inventory/ItemContainer.h"

namespace glimmer {
    /**
     * Item container component
     * 物品容器组件
     * Add it to a game object to give the game object the ability to hold items.
     * 将其添加到一个游戏对象上，使游戏对象拥有持有物品的能力。
     */
    class ItemContainerComponent : public GameComponent {
        std::unique_ptr<ItemContainer> itemContainer_;

    public:
        explicit ItemContainerComponent(const size_t capacity) {
            itemContainer_ = std::make_unique<ItemContainer>(capacity);
        }

        /**
         * GetItemContainer
         * 获取物品容器
         * @return
         */
        [[nodiscard]] ItemContainer *GetItemContainer() const;

        [[nodiscard]] u_int32_t GetId() override;
    };
}

#endif //GLIMMERWORKS_ITEMCONTAINERCOMONENT_H
