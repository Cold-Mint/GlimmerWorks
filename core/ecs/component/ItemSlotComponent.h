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
        bool allowSelected_ = false;

    public:
        explicit ItemSlotComponent(ItemContainer *itemContainer, int slotIndex, bool allowSelected);

        [[nodiscard]] bool AllowSelected() const;

        [[nodiscard]] Item *GetItem() const;

        [[nodiscard]] bool IsHovered() const;

        void SetHovered(bool hovered);

        [[nodiscard]] bool IsSelected() const;

        void SetSelected(bool selected);

        [[nodiscard]] uint32_t GetId() override;
    };
}

#endif //GLIMMERWORKS_ITEMSLOTCOMPONENT_H
