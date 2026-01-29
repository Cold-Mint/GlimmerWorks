//
// Created by coldmint on 2026/1/29.
//

#include "NoneAbility.h"

std::string glimmer::NoneAbility::GetId() const {
    return ABILITY_ID_NONE;
}

void glimmer::NoneAbility::OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity::ID user) {
}
