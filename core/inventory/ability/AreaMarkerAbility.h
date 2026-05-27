//
// Created by Cold-Mint on 2026/2/26.
//

#pragma once
#include "ItemAbility.h"

namespace glimmer
{
    class AreaMarkerAbility : public ItemAbility
    {
    public:
        explicit AreaMarkerAbility(const AbilityConfig& abilityConfigMessage);

        void OnUse(WorldContext* worldContext, GameEntity::ID user, const AbilityConfig* abilityConfig,
            std::unordered_set<std::string>& popupAbility) override;

        [[nodiscard]] std::string GetId() const override;

        [[nodiscard]] std::unique_ptr<ItemAbility> Clone() const override;
    };
}
