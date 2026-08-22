/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#pragma once

namespace glimmer {
    enum class ContainerChangeType {
        /**
         * The quantity of items has increased.
         * 物品数量增加
         */
        STACK_AMOUNT_INCREASE,
        /**
         * The quantity of items has decreased.
         * 物品数量减少
         */
        STACK_AMOUNT_DECREASE,

        /**
         * Increase in durability (restoration of item durability)
         * 耐久度增加（恢复物品耐久）
         */
        STACK_DURABILITY_INCREASE,

        /**
         * Durability reduction (durability reduction)
         * 耐久度减少（耐久度减少）
         */
        STACK_DURABILITY_DECREASE,

        /**
         * New items have been added to the container.
         * 新的物品被添加到了容器内
         */
        STACK_CREATE,
        /**
         * The item amount has been exhausted (amount reached zero).
         * 物品数量耗尽（数量为零时）
         */
        STACK_AMOUNT_EXHAUSTED,
        /**
         * The item durability has been exhausted (used durability reached max).
         * 物品耐久耗尽（已用耐久达到上限时）
         */
        STACK_DURABILITY_EXHAUSTED,
        /**
         * The item has been removed from the container by external operations (e.g. replacement, take, reset).
         * 物品被外部操作从容器中移除（如替换、取走、重置）
         */
        STACK_DESTROY,
    };
}
