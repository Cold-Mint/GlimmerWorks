//
// Created by Cold-Mint on 2025/12/23.
//

#ifndef GLIMMERWORKS_ITEMFUNCTIONMOD_H
#define GLIMMERWORKS_ITEMFUNCTIONMOD_H
#include <memory>
#include <string>
#include <unordered_set>

#include "core/ecs/GameEntity.h"
#include "core/mod/Resource.h"

namespace glimmer {
    class AppContext;
    class WorldContext;
    class GameEntity;

    /**
     * Item Ability
     * 物品能力
     */
    class ItemAbility {
        AbilityConfig abilityConfig_;

    public:
        explicit ItemAbility(
            const AppContext *appContext, const AbilityConfig &abilityConfig);

        virtual ~ItemAbility() = default;

        [[nodiscard]] virtual std::string GetId() const = 0;

        [[nodiscard]] const AbilityConfig &GetAbilityConfig() const;

        virtual void OnUse(WorldContext *worldContext, GameEntity::ID user, const AbilityConfig &abilityConfig,
                           std::unordered_set<std::string> &popupAbility) = 0;

        [[nodiscard]] virtual std::unique_ptr<ItemAbility> Clone() const = 0;
    };
}

#endif //GLIMMERWORKS_ITEMFUNCTIONMOD_H
