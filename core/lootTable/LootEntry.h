//@genCode
//
// Created by coldmint on 2026/1/30.
//

#ifndef GLIMMERWORKS_LOOTENTRY_H
#define GLIMMERWORKS_LOOTENTRY_H
#include "core/mod/ResourceRef.h"

namespace glimmer {
    //@genNextLine(LootEntry|战利品实体)
    struct LootEntry {
        //@genNextLine(LootEntry.item|战利品物品)
        ResourceRef item;

        //@genNextLine(LootEntry.weight|掉落权重)
        uint32_t weight = 30;

        //@genNextLine(LootEntry.min|最小掉落数量)
        uint32_t min = 1;

        //@genNextLine(LootEntry.max|最大掉落数量)
        uint32_t max = 1;

        /**
         * Whether it is an inevitable drop
         * 是否为必然掉落
         */
        //@genNextLine(LootEntry.mandatory|是否必然掉落)
        bool mandatory = false;
    };
}

#endif //GLIMMERWORKS_LOOTENTRY_H
