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

#include "core/config/Constants.h"
#include "core/mod/ResourceRef.h"
#include "core/world/generator/TileLayerType.h"

namespace glimmer {
    class Chunk;
    class ResourceLocator;

    /**
     * ChunkTilePopulator
     * 区块瓦片填充器
     * Writes the resolved tile references into a chunk as concrete tile states.
     * 将解析出的瓦片引用写入区块，形成具体的瓦片状态。
     */
    class ChunkTilePopulator {
        /**
         * PopulateSingleTilePosition
         * 填充单个瓦片位置
         * @param chunk chunk 目标区块
         * @param resourceLocator resourceLocator 资源定位器
         * @param tilesRefMap tilesRefMap 瓦片引用映射
         * @param topLeftIndex topLeftIndex 左上角索引
         */
        static void PopulateSingleTilePosition(
            Chunk *chunk, const ResourceLocator *resourceLocator,
            const std::unordered_map<TileLayerType, std::array<ResourceRef, CHUNK_AREA> > &tilesRefMap,
            int topLeftIndex);

    public:
        /**
         * Populate
         * 填充区块瓦片
         * @param chunk chunk 目标区块
         * @param resourceLocator resourceLocator 资源定位器
         * @param tilesRefMap tilesRefMap 瓦片引用映射
         */
        static void Populate(Chunk *chunk, const ResourceLocator *resourceLocator,
                             const std::unordered_map<TileLayerType, std::array<ResourceRef, CHUNK_AREA> > &
                             tilesRefMap);
    };
}
