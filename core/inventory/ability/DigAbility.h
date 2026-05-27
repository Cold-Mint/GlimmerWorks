//
// Created by Cold-Mint on 2025/12/25.
//

#pragma once

#include "ItemAbility.h"
#include "MiningRangeData.h"
#include "core/ecs/GameEntity.h"

namespace glimmer
{
    class DigAbility : public ItemAbility
    {
        MiningRangeData miningRangeData_;

    public:
        explicit DigAbility(const AbilityConfig& abilityConfig);

        ~DigAbility() override = default;

        [[nodiscard]] std::string GetId() const override;

        void OnUse(WorldContext* worldContext, GameEntity::ID user, const AbilityConfig* abilityConfig,
            std::unordered_set<std::string>& popupAbility) override;

        [[nodiscard]] std::unique_ptr<ItemAbility> Clone() const override;
    };
}
