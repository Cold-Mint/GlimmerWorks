//
// Created by Cold-Mint on 2025/12/25.
//

#pragma once
#include "ItemContainerComonent.h"
#include "core/ecs/GameComponent.h"

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

        [[nodiscard]] std::unique_ptr<Item> TakeAllItem() const;

        [[nodiscard]] std::unique_ptr<Item> ReplaceItem(std::unique_ptr<Item> item) const;

        [[nodiscard]] bool IsHovered() const;

        void SetHovered(bool hovered);

        [[nodiscard]] bool IsSelected() const;

        void SetSelected(bool selected);

        [[nodiscard]] uint32_t GetId() override;
    };
}
