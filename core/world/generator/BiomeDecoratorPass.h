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
    /**
     * BiomeDecoratorPass
     * 生物群系装饰阶段。
     */
    enum class BiomeDecoratorPass {
        /**
         * The sculpting decorator relies on terrain data to carve out the blocks.
         * 雕刻型装饰器依赖地形数据，来雕刻区块。
         * For example: Filling block
         * 例如：填充区块
         */
        TerrainCarving,
        /**
         * Content-type decorators require the block data that has been carved by the carving decorators.
         * 内容型装饰器，需要依赖雕刻装饰器雕刻后的区块数据。
         * For example: Place the loot chest.
         * 例如：放置战利品箱。
         */
        ContentPlacing
    };
}
