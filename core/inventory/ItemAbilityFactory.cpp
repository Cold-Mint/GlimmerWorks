//
// Created by coldmint on 2025/12/29.
//

#include "ItemAbilityFactory.h"

#include "../Constants.h"
#include "../mod/Resource.h"
#include "ability/DigAbility.h"


std::unique_ptr<glimmer::ItemAbility> glimmer::ItemAbilityFactory::CreateItemAbility(
    const std::string &id, const std::vector<AbilityData> &abilityData) {
    if (id == ABILITY_ID_DIG) {
        return std::make_unique<DigAbility>(abilityData);
    }
    return nullptr;
}
