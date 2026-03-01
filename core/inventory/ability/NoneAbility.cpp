//
// Created by coldmint on 2026/1/29.
//

#include "NoneAbility.h"

glimmer::NoneAbility::NoneAbility(const AppContext *appContext, const VariableConfig &abilityData) : ItemAbility(
    appContext, abilityData) {
}

std::string glimmer::NoneAbility::GetId() const {
    return ABILITY_ID_NONE;
}

void glimmer::NoneAbility::OnUse(WorldContext *worldContext, GameEntity::ID user, const VariableConfig &abilityData,
                                 std::unordered_set<std::string> &popupAbility) {
}

std::unique_ptr<glimmer::ItemAbility> glimmer::NoneAbility::Clone() const {
    return std::make_unique<NoneAbility>(*this);
}
