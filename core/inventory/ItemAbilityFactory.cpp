//
// Created by coldmint on 2025/12/29.
//

#include "ItemAbilityFactory.h"

#include "../Constants.h"
#include "../mod/Resource.h"
#include "ability/AreaMarkerAbility.h"
#include "ability/DigAbility.h"
#include "ability/NoneAbility.h"


std::shared_ptr<glimmer::ItemAbility> glimmer::ItemAbilityFactory::CreateItemAbility(
    const AppContext *appContext, const std::string &id,
    const VariableConfig &abilityData) {
    if (id == ABILITY_ID_NONE) {
        return std::make_shared<NoneAbility>(appContext, abilityData);
    }
    if (id == ABILITY_ID_DIG) {
        return std::make_shared<DigAbility>(appContext, abilityData);
    }
    if (id == ABILITY_ID_AREA_MARKER) {
        return std::make_shared<AreaMarkerAbility>(appContext, abilityData);
    }
    return nullptr;
}
