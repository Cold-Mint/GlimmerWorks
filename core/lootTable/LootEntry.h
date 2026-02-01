//
// Created by coldmint on 2026/1/30.
//

#ifndef GLIMMERWORKS_LOOTENTRY_H
#define GLIMMERWORKS_LOOTENTRY_H
#include "core/mod/ResourceRef.h"

namespace glimmer {
    struct LootEntry {
        ResourceRef item;
        uint32_t weight = 30;
        uint32_t min = 1;
        uint32_t max = 1;
        /**
         * Whether it is an inevitable drop
         * 是否为必然掉落
         */
        bool mandatory = false;
    };
}

#endif //GLIMMERWORKS_LOOTENTRY_H
