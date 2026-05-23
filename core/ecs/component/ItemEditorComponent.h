//
// Created by Cold-Mint on 2026/1/23.
//

#pragma once
#include <vector>
#include "core/ecs/GameComponent.h"
#include "core/ecs/GameEntity.h"

namespace glimmer {
    class ItemEditorComponent : public GameComponent {
        std::vector<GameEntity::ID> slotEntities;

    public:
        void Reserve(size_t size);

        void AddSlotEntity(GameEntity::ID id);

        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;

        std::vector<GameEntity::ID> &GetSlotEntities();
    };
}
