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
#include "Dimension.h"

namespace glimmer {
    /**
     * SimpleDimensionManager
     * 单人游戏内使用简易维度管理器
     *
     * When adding multiplayer games in the future, consider moving them to a separate library.
     * 当往后加入多人游戏时，可考虑将其移动到单独的库中。
     */
    class SimpleDimensionManager {
        /**
        * In a single-player game, the player can only exist in one dimension. They cannot be in both dimension A and dimension B simultaneously. (Multiple dimensions coexisting in memory only occur in multiplayer games.)
        * 单人游戏内，玩家只能处在一个维度。不能同时在A维度又在B维度。（多人游戏才会出现多个维度共存在内存中）
        */
        Dimension *dimension_ = nullptr;

    public:
        /**
         * Obtain the current tick count of the current dimension's experience
         * 获取当前维度经历的tick数
         * @return
         */
        uint64_t GetDimensionTick() const;
    };
}
