//
// Created by coldmint on 2026/1/23.
//

#ifndef GLIMMERWORKS_ITEMEDITORCOMPONENT_H
#define GLIMMERWORKS_ITEMEDITORCOMPONENT_H
#include <vector>

#include "core/ecs/GameComponent.h"
#include "core/ecs/GameEntity.h"

namespace glimmer {
    class ItemEditorComponent : public GameComponent {
        std::vector<GameEntity::ID> slotEntities;

    public:
        void Reserve(size_t size);

        void AddSlotEntity(GameEntity::ID id);

        std::vector<GameEntity::ID> &GetSlotEntities();
    };
}

#endif //GLIMMERWORKS_ITEMEDITORCOMPONENT_H
