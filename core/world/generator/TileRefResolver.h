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
#include <array>
#include <unordered_map>
#include <unordered_set>

#include "core/config/Constants.h"
#include "core/math/ChunkVertexVector2D.h"
#include "core/mod/ResourceRef.h"
#include "core/world/generator/TerrainResult.h"
#include "core/world/generator/TileLayerType.h"

namespace glimmer {
    struct BiomeResource;

    /**
     * TileRefResolver
     * 瓦片引用解析器
     * Maps each terrain tile result into concrete tile resource references.
     * 将每个瓦片地形结果映射为具体的瓦片资源引用。
     */
    class TileRefResolver {
        static void WriteWaterResourceRef(TileLayerType layerType, ResourceRef &resourceRef);

        static void WriteAirResourceRef(TileLayerType layerType, ResourceRef &resourceRef);

        static void WriteBedRockResourceRef(TileLayerType layerType, ResourceRef &resourceRef);


        static void WriteVoidResourceRef(TileLayerType layerType, ResourceRef &resourceRef);

    public:
        /**
         * Initialize
         * 初始化瓦片引用
         * @param terrainResult terrainResult 地形结果
         * @param chunkVertexVector2D
         * @param tilesRefMap tilesRefMap 瓦片引用映射（会被修改）
         * @param biomeResourcesSet biomeResourcesSet 生物群系集合（会被修改）
         */
        static void Initialize(const TerrainResult *terrainResult, const ChunkVertexVector2D &chunkVertexVector2D,
                               std::unordered_map<TileLayerType, std::array<ResourceRef, CHUNK_AREA> > &tilesRefMap,
                               std::unordered_set<BiomeResource *> &biomeResourcesSet);
    };
}
