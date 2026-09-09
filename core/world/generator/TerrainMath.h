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
     * TerrainMath
     * 地形数学工具
     * Stateless pure functions shared by the world generator and various debug / command tools.
     * 无状态纯函数，供世界生成器与各类调试、命令工具共用。
     */
    namespace TerrainMath {
        /**
         * GetElevation
         * 获取海拔
         * Normalize a world Y coordinate into a 0-1 elevation value.
         * 将世界Y坐标归一化为0-1的海拔值。
         * @param y y 世界Y坐标
         * @return Elevation in [0,1] 海拔值，范围0-1
         */
        float GetElevation(int y);

        /**
         * GetSurfaceProximity
         * 获取地表贴近度
         * Compute how close a world Y coordinate is to the surface.
         * 计算某个世界Y坐标距离地表的贴近程度。
         * @param firstTileTerrainY firstTileTerrainY 地表第一格Y坐标
         * @param worldY worldY 世界Y坐标
         * @return Proximity in [0,1] 贴近度，范围0-1
         */
        float GetSurfaceProximity(int firstTileTerrainY, int worldY);
    }
}
