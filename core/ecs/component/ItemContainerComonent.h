//
// Created by coldmint on 2025/12/18.
//

#ifndef GLIMMERWORKS_ITEMCONTAINERCOMONENT_H
#define GLIMMERWORKS_ITEMCONTAINERCOMONENT_H
#include "../GameComponent.h"
#include "../../inventory/ItemContainer.h"

namespace glimmer {
    /**
     * 物品容器组建
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
    };
}

#endif //GLIMMERWORKS_ITEMCONTAINERCOMONENT_H
