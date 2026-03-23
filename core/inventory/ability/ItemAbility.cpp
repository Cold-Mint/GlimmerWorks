//
// Created by Cold-Mint on 2025/12/23.
//

#include "ItemAbility.h"

glimmer::ItemAbility::ItemAbility(const AppContext *appContext, const AbilityConfig &abilityConfig) {
    abilityConfig_ = abilityConfig;
}

const glimmer::AbilityConfig & glimmer::ItemAbility::GetAbilityConfig() const {
    return abilityConfig_;
}
