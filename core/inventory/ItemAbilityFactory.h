//
// Created by coldmint on 2025/12/29.
//

#ifndef GLIMMERWORKS_ITEMABILITYFACTORY_H
#define GLIMMERWORKS_ITEMABILITYFACTORY_H
#include <memory>
#include <string>

#include "ability/ItemAbility.h"

namespace glimmer {
    class ItemAbilityFactory {
    public:
        /**
         * CreateItemAbility
         * 创建物品能力
         * @param id id
         * @return Return the item's ability, which may be empty 返回物品能力，可能为空
         */
        static std::unique_ptr<ItemAbility> CreateItemAbility(const std::string &id);
    };
}

#endif //GLIMMERWORKS_ITEMABILITYFACTORY_H
