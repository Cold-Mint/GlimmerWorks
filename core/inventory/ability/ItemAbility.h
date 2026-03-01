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
        VariableConfig variableConfig;

    public:
        explicit ItemAbility(
            const AppContext *appContext, const VariableConfig &abilityData);

        virtual ~ItemAbility() = default;

        [[nodiscard]] virtual std::string GetId() const = 0;

        [[nodiscard]] const VariableConfig &GetVariableConfig() const;

        virtual void OnUse(WorldContext *worldContext, GameEntity::ID user, const VariableConfig &abilityData,
                           std::unordered_set<std::string> &popupAbility) = 0;

        [[nodiscard]] virtual std::unique_ptr<ItemAbility> Clone() const = 0;
    };
}

#endif //GLIMMERWORKS_ITEMFUNCTIONMOD_H
