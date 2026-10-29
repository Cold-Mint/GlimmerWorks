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
#include "BiomeDecorator.h"

namespace glimmer {
    /**
     * LootChestBiomeDecorator
     * 战利品箱生物群系装饰器
     * Deterministically places loot-chest tiles (storage blocks with a container_loot_table)
     * into the chunk. The positions only depend on the world seed and chunk coordinates,
     * so the same seed always yields the same chest positions.
     * 以确定性的方式在区块内放置战利品箱瓦片（带 container_loot_table 的存储方块）。
     * 位置只取决于世界种子与区块坐标，因此相同种子总是得到相同箱子位置。
     */
    class LootChestBiomeDecorator : public BiomeDecorator<LootChestBiomeDecoratorResource> {
        void DecorationImpl(const ChunkVertexVector2D &chunkVertex, WorldContext *worldContext,
                            TerrainResult *terrainResult, LootChestBiomeDecoratorResource *decoratorResource,
                            BiomeResource *biomeResource,
                            std::unordered_map<TileLayerType, std::array<ResourceRef, CHUNK_AREA>> *tilesRefMap) override;

    public:
        BiomeDecoratorType GetBiomeDecoratorType() override;

        BiomeDecoratorPass GetPass() override;
    };
}
