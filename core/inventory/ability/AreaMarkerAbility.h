//
// Created by coldmint on 2026/2/26.
//

#ifndef GLIMMERWORKS_AREAMARKERABILITY_H
#define GLIMMERWORKS_AREAMARKERABILITY_H
#include "ItemAbility.h"

namespace glimmer {
    class AreaMarkerAbility : public ItemAbility {
        SDL_Color areaMarkerBorderColor_{};
        SDL_Color areaMarkerFullColor_{};

    public:
        explicit AreaMarkerAbility(const AppContext *appContext, const VariableConfig &abilityData);

        [[nodiscard]] std::string GetId() const override;

        void OnUse(WorldContext *worldContext, GameEntity::ID user) override;

        [[nodiscard]] std::unique_ptr<ItemAbility> Clone() const override;
    };
}


#endif //GLIMMERWORKS_AREAMARKERABILITY_H
