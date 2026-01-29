//
// Created by coldmint on 2026/1/29.
//

#ifndef GLIMMERWORKS_NONEABILITY_H
#define GLIMMERWORKS_NONEABILITY_H
#include "ItemAbility.h"

namespace glimmer {
    class NoneAbility : public ItemAbility {
    public:
        explicit NoneAbility(const VariableConfig &abilityData)
            : ItemAbility(abilityData) {
        }

        [[nodiscard]] std::string GetId() const override;

        void OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity::ID user) override;

        [[nodiscard]] std::unique_ptr<ItemAbility> Clone() const override {
            return std::make_unique<NoneAbility>(*this);
        }
    };
}

#endif //GLIMMERWORKS_NONEABILITY_H
