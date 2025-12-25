//
// Created by Cold-Mint on 2025/12/25.
//

#ifndef GLIMMERWORKS_ITEMSLOTCOMPONENT_H
#define GLIMMERWORKS_ITEMSLOTCOMPONENT_H
#include "../GameComponent.h"
#include "../GameEntity.h"

namespace glimmer {
    class ItemSlotComponent : public GameComponent {
        GameEntity *containerEntity_;
        int slotIndex_;
        bool isHovered_ = false;
        bool isSelected_ = false;

    public:
        explicit ItemSlotComponent(GameEntity *containerEntity, const int slotIndex)
            : containerEntity_(containerEntity), slotIndex_(slotIndex) {
        }

        [[nodiscard]] GameEntity *GetContainerEntity() const ;

        [[nodiscard]] int GetSlotIndex() const;

        [[nodiscard]] bool IsHovered() const ;

        void SetHovered(bool hovered) ;

        [[nodiscard]] bool IsSelected() const;

        void SetSelected(bool selected);
    };
}

#endif //GLIMMERWORKS_ITEMSLOTCOMPONENT_H
