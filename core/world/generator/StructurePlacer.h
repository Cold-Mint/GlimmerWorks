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
#include <optional>

#include "core/config/Constants.h"

namespace glimmer {
    class WorldContext;
    class TerrainManager;
    class TileVector2D;
    class StructureInfo;
    class TerrainResult;
    class AppContext;
    struct IStructureResource;

    /**
     * StructurePlacer
     * 结构放置器
     * Matches structure placement conditions and writes structure tiles into the terrain.
     * 匹配结构放置条件，并将结构瓦片写入地形。
     */
    class StructurePlacer {
        WorldContext *worldContext_;

        /**
         * PlaceStructureTiles
         * 放置结构瓦片
         * @param terrainManager terrainManager 地形管理器
         * @param structureInfo structureInfo 结构信息
         * @param globalOrigin globalOrigin 全局原点
         */
        static void PlaceStructureTiles(TerrainManager *terrainManager, const StructureInfo &structureInfo,
                                        const TileVector2D &globalOrigin);

        /**
         * MatchStructureConditions
         * 匹配结构放置条件
         * @param appContext appContext 应用上下文
         * @param terrainResult terrainResult 地形结果
         * @param structureResource structureResource 结构资源
         * @return A bitset of candidate points, or nullopt if no condition matched 候选点集合，未匹配到条件时返回nullopt
         */
        static std::optional<std::bitset<CHUNK_AREA> > MatchStructureConditions(
            const AppContext *appContext, TerrainResult *terrainResult, const IStructureResource *structureResource);

        /**
         * PlaceStructureAtCandidatePoints
         * 在候选点放置结构
         * @param appContext appContext 应用上下文
         * @param terrainManager terrainManager 地形管理器
         * @param position position 区块位置
         * @param candidatePoints candidatePoints 候选点集合
         * @param structureResource structureResource 结构资源
         * @return The number of marked points 已标记的点数
         */
        int PlaceStructureAtCandidatePoints(const AppContext *appContext, TerrainManager *terrainManager,
                                            const TileVector2D &position,
                                            const std::bitset<CHUNK_AREA> &candidatePoints,
                                            IStructureResource *structureResource) const;

    public:
        explicit StructurePlacer(WorldContext *worldContext);

        /**
         * GenerateStructure
         * 生成结构
         * @param position position 区块位置
         */
        void GenerateStructure(const TileVector2D &position) const;
    };
}
