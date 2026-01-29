//
// Created by coldmint on 2025/12/23.
//

#ifndef GLIMMERWORKS_ITEMFUNCTIONMOD_H
#define GLIMMERWORKS_ITEMFUNCTIONMOD_H
#include <string>
#include "../../mod/Resource.h"
#include "core/ecs/GameEntity.h"

namespace glimmer {
    class AppContext;
    class WorldContext;
    class GameEntity;

    /**
     * Item Ability
     * 物品能力
     */
    class ItemAbility {
    public:
        explicit ItemAbility(
            const VariableConfig &abilityData) {
        }

        virtual ~ItemAbility() = default;

        [[nodiscard]] virtual std::string GetId() const = 0;

        virtual void OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity::ID user) = 0;

        [[nodiscard]] virtual std::unique_ptr<ItemAbility> Clone() const = 0;
    };
}

#endif //GLIMMERWORKS_ITEMFUNCTIONMOD_H
