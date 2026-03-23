//
// Created by Cold-Mint on 2025/12/29.
//

#ifndef GLIMMERWORKS_ITEMABILITYFACTORY_H
#define GLIMMERWORKS_ITEMABILITYFACTORY_H
#include <memory>
#include <string>

#include "ability/ItemAbility.h"
#include "core/mod/Resource.h"

namespace glimmer {
    class ItemAbilityFactory {
    public:
        /**
         * CreateItemAbility
         * 创建物品能力
         * @param appContext appContext
         * @param id id
         * @param abilityConfig abilityConfig
         * @return Return the item's ability, which may be empty 返回物品能力，可能为空
         */
        static std::shared_ptr<ItemAbility> CreateItemAbility(const AppContext *appContext, const std::string &id,
                                                              const AbilityConfig &abilityConfig);
    };
}

#endif //GLIMMERWORKS_ITEMABILITYFACTORY_H
