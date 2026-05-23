//
// Created by Cold-Mint on 2025/12/18.
//

#pragma once
#include "core/ecs/GameComponent.h"
#include <vector>

#include "core/ecs/GameEntity.h"

namespace glimmer {
    class GameEntity;

    class HotBarComponent : public GameComponent {
        int maxSlot_;
        std::vector<GameEntity::ID> slotEntities_;

    public:
        explicit HotBarComponent(int maxSlot);

        [[nodiscard]] int GetMaxSlot() const;

        void AddSlotEntity(GameEntity::ID entity);

        [[nodiscard]] const std::vector<GameEntity::ID> &GetSlotEntities() const;

        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;
    };
}
