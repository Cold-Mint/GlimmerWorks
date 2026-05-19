//
// Created by Cold-Mint on 2026/2/28.
//

#pragma once

namespace glimmer {
    enum class ContainerChangeType {
        /**
         * Add items
         * 添加物品
         */
        ADD,
        /**
         * Remove items
         * 移除物品
         */
        REMOVE,
        /**
         * Increase in quantity
         * 数量增加
         */
        COUNT_ADD,
        /**
         * Quantity removal
         * 数量移除
         */
        COUNT_REMOVE
    };
}
