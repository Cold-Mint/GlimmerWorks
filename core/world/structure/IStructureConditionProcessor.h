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
#include <bitset>

#include "StructureConditionProcessorType.h"
#include "core/world/generator/TerrainResult.h"

namespace glimmer {
    class IStructureConditionProcessor {
    public:
        virtual ~IStructureConditionProcessor() = default;

        /**
         * GetStructureConditionProcessorType
         * 获取结构条件处理器类型
         * @return
         */
        virtual StructureConditionProcessorType GetStructureConditionProcessorType() = 0;

        /**
         * Calculate the feasible placement points for the structure within the terrain data.
         * 在地形数据内计算可行的结构放置点。
         * @param terrainResult terrainResult 地形数据
         * @param placementConditionsResource placementConditionsResource 放置条件资源
         * @return The bitset for the structure placement points. If a certain point is 1, it indicates that a structure can be placed at that point. 结构放置点的bitset，若某个点为1,那么表示可以在该点放置。
         */
        virtual std::bitset<CHUNK_AREA> Match(const TerrainResult *terrainResult,
                                              const IStructurePlacementConditionsResource *placementConditionsResource)
        = 0;
    };
}
