//
// Created by Cold-Mint on 2025/12/29.
//

#pragma once
#include <memory>
#include <string>

#include "ability/ItemAbility.h"
#include "core/mod/Resource.h"

namespace glimmer
{
    class ItemAbilityFactory
    {
    public:
        /**
         * CreateItemAbility
         * 创建物品能力
         * @param id id
         * @param abilityConfig abilityConfig
         * @return Return the item's ability, which may be empty 返回物品能力，可能为空
         */
        static std::shared_ptr<ItemAbility> CreateItemAbility(const std::string& id,
                                                              const AbilityConfig& abilityConfig);
    };
}
