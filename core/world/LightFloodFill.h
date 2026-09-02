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
#include <functional>

#include "core/math/TileVector2D.h"

namespace glimmer {
    /**
     * LightFloodFill
     * 光照泛洪遍历器
     *
     * Breadth-first flood fill starting from a light source center, spreading
     * outward with 8-directional connectivity. This replaces the previous
     * ray-casting traversal and naturally produces round, artifact-free light
     * halos while supporting solid-tile occlusion.
     * 从光源中心开始的广度优先泛洪，以 8 方向连通向外扩散。它取代了此前的
     * 射线遍历，天然产生圆形、无伪影的光晕，并支持实体瓦片遮挡。
     */
    class LightFloodFill {
    public:
        using OpaquePredicate = std::function<bool(const TileVector2D &)>;
        using VisitCallback = std::function<void(const TileVector2D &)>;

        /**
         * Propagate
         * 传播光照
         * @param center center 光源中心
         * @param maxRadius maxRadius 最大半径
         * @param isOpaque isOpaque 判断某瓦片是否阻挡光线
         * @param visit visit 访问回调（每个可达瓦片恰好回调一次，含中心）
         * @param diagonalBlock diagonalBlock 是否启用对角防漏光规则
         */
        static void Propagate(const TileVector2D &center, int maxRadius,
                              const OpaquePredicate &isOpaque,
                              const VisitCallback &visit,
                              bool diagonalBlock = true);
    };
}
