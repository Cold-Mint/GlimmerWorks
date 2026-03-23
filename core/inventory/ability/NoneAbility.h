//
// Created by Cold-Mint on 2026/1/29.
//

#ifndef GLIMMERWORKS_NONEABILITY_H
#define GLIMMERWORKS_NONEABILITY_H
#include "ItemAbility.h"

namespace glimmer {
    class NoneAbility : public ItemAbility {
    public:
        NoneAbility(const AppContext *appContext, const AbilityConfig &abilityConfigMessage);

        void OnUse(WorldContext *worldContext, GameEntity::ID user, const AbilityConfig &abilityConfig,
                   std::unordered_set<std::string> &popupAbility) override;

        [[nodiscard]] std::string GetId() const override;


        [[nodiscard]] std::unique_ptr<ItemAbility> Clone() const override;
    };
}

#endif //GLIMMERWORKS_NONEABILITY_H
