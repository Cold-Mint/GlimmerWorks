//
// Created by Cold-Mint on 2025/12/25.
//

#ifndef GLIMMERWORKS_ITEMSLOTCOMPONENT_H
#define GLIMMERWORKS_ITEMSLOTCOMPONENT_H
#include "../GameComponent.h"
#include "../GameEntity.h"

namespace glimmer {
    class ItemSlotComponent : public GameComponent {
        GameEntity::ID containerEntity_;
        int slotIndex_;
        bool isHovered_ = false;
        bool isSelected_ = false;

    public:
        explicit ItemSlotComponent(const GameEntity::ID containerEntity, const int slotIndex)
            : containerEntity_(containerEntity), slotIndex_(slotIndex) {
        }

        [[nodiscard]] GameEntity::ID GetContainerEntity() const;

        [[nodiscard]] int GetSlotIndex() const;

        [[nodiscard]] bool IsHovered() const;

        void SetHovered(bool hovered);

        [[nodiscard]] bool IsSelected() const;

        void SetSelected(bool selected);

        [[nodiscard]] u_int32_t GetId() override;
    };
}

#endif //GLIMMERWORKS_ITEMSLOTCOMPONENT_H
