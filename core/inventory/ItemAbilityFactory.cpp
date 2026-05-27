//
// Created by Cold-Mint on 2025/12/29.
//

#include "ItemAbilityFactory.h"

#include "../Constants.h"
#include "ability/AreaMarkerAbility.h"
#include "ability/DigAbility.h"
#include "ability/NoneAbility.h"


std::shared_ptr<glimmer::ItemAbility> glimmer::ItemAbilityFactory::CreateItemAbility(const std::string& id,
    const AbilityConfig& abilityConfig)
{
    if (id == ABILITY_ID_NONE)
    {
        return std::make_shared<NoneAbility>(abilityConfig);
    }
    if (id == ABILITY_ID_DIG)
    {
        return std::make_shared<DigAbility>(abilityConfig);
    }
    if (id == ABILITY_ID_AREA_MARKER)
    {
        return std::make_shared<AreaMarkerAbility>(abilityConfig);
    }
    return nullptr;
}
