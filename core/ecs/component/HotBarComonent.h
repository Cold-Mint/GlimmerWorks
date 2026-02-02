//
// Created by Cold-Mint on 2025/12/18.
//

#ifndef GLIMMERWORKS_HOTBARCOMONENT_H
#define GLIMMERWORKS_HOTBARCOMONENT_H
#include "../GameComponent.h"
#include <vector>

#include "core/ecs/GameEntity.h"

namespace glimmer {
    class GameEntity;

    class HotBarComponent : public GameComponent {
        int selectedSlot_ = 0;
        int maxSlot_;
        std::vector<GameEntity::ID> slotEntities_;

    public:
        explicit HotBarComponent(int maxSlot);

        [[nodiscard]] int GetSelectedSlot() const;

        void SetSelectedSlot(int selectedSlot);

        [[nodiscard]] int GetMaxSlot() const;

        void AddSlotEntity(GameEntity::ID entity);

        [[nodiscard]] const std::vector<GameEntity::ID> &GetSlotEntities() const;

        [[nodiscard]] u_int32_t GetId() override;
    };
}

#endif //GLIMMERWORKS_HOTBARCOMONENT_H
