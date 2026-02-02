//
// Created by coldmint on 2026/1/29.
//

#include "NoneAbility.h"

glimmer::NoneAbility::NoneAbility(const VariableConfig &abilityData)
    : ItemAbility(abilityData) {
}

std::string glimmer::NoneAbility::GetId() const {
    return ABILITY_ID_NONE;
}

void glimmer::NoneAbility::OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity::ID user) {
}

std::unique_ptr<glimmer::ItemAbility> glimmer::NoneAbility::Clone() const {
    return std::make_unique<NoneAbility>(*this);
}
