//
// Created by Cold-Mint on 2025/12/25.
//

#ifndef GLIMMERWORKS_ITEMSLOTCOMPONENT_H
#define GLIMMERWORKS_ITEMSLOTCOMPONENT_H
#include "ItemContainerComonent.h"
#include "../GameComponent.h"

namespace glimmer {
    class ItemSlotComponent : public GameComponent {
        ItemContainer *itemContainer_;
        int slotIndex_;
        bool isHovered_ = false;
        bool isSelected_ = false;

    public:
        explicit ItemSlotComponent(ItemContainer *itemContainer, const int slotIndex)
            : itemContainer_(itemContainer), slotIndex_(slotIndex) {
        }

        [[nodiscard]] ItemContainer *GetItemContainer() const;

        [[nodiscard]] int GetSlotIndex() const;

        [[nodiscard]] bool IsHovered() const;

        void SetHovered(bool hovered);

        [[nodiscard]] bool IsSelected() const;

        void SetSelected(bool selected);

        [[nodiscard]] u_int32_t GetId() override;
    };
}

#endif //GLIMMERWORKS_ITEMSLOTCOMPONENT_H
