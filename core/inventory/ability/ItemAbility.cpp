//
// Created by coldmint on 2025/12/23.
//

#include "ItemAbility.h"

glimmer::ItemAbility::ItemAbility(const AppContext *appContext, const VariableConfig &abilityData) {
    variableConfig = abilityData;
}

const glimmer::VariableConfig &glimmer::ItemAbility::GetVariableConfig() const {
    return variableConfig;
}
