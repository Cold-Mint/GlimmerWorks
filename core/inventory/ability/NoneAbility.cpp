//
// Created by Cold-Mint on 2026/1/29.
//

#include "NoneAbility.h"

#include "core/Constants.h"

glimmer::NoneAbility::NoneAbility(const AppContext *appContext, const AbilityConfig &abilityConfigMessage)
    : ItemAbility(appContext, abilityConfigMessage) {
}

void glimmer::NoneAbility::OnUse(WorldContext *worldContext, GameEntity::ID user, const AbilityConfig &abilityConfig,
                                 std::unordered_set<std::string> &popupAbility) {
}

std::string glimmer::NoneAbility::GetId() const {
    return ABILITY_ID_NONE;
}

std::unique_ptr<glimmer::ItemAbility> glimmer::NoneAbility::Clone() const {
    return std::make_unique<NoneAbility>(*this);
}
