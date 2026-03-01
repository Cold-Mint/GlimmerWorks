//
// Created by coldmint on 2026/2/26.
//

#ifndef GLIMMERWORKS_AREAMARKERABILITY_H
#define GLIMMERWORKS_AREAMARKERABILITY_H
#include "ItemAbility.h"

namespace glimmer {
    class AreaMarkerAbility : public ItemAbility {
    public:
        explicit AreaMarkerAbility(const AppContext *appContext, const VariableConfig &abilityData);

        [[nodiscard]] std::string GetId() const override;

        void OnUse(WorldContext *worldContext, GameEntity::ID user, const VariableConfig &abilityData,
            std::unordered_set<std::string> &popupAbility) override;

        [[nodiscard]] std::unique_ptr<ItemAbility> Clone() const override;
    };
}


#endif //GLIMMERWORKS_AREAMARKERABILITY_H
