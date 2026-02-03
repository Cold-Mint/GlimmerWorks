//
// Created by coldmint on 2026/1/29.
//

#ifndef GLIMMERWORKS_NONEABILITY_H
#define GLIMMERWORKS_NONEABILITY_H
#include "ItemAbility.h"

namespace glimmer {
    class NoneAbility : public ItemAbility {
    public:
        explicit NoneAbility(const VariableConfig &abilityData);

        [[nodiscard]] std::string GetId() const override;

        void OnUse(WorldContext *worldContext, GameEntity::ID user) override;

        [[nodiscard]] std::unique_ptr<ItemAbility> Clone() const override;
    };
}

#endif //GLIMMERWORKS_NONEABILITY_H
