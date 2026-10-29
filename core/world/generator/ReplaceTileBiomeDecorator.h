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
     * ReplaceTileBiomeDecorator
     * 替换瓦片生物群系装饰器
     * Replaces a tile with another tile according to the configured rules. For each rule, when the
     * tile at the offset relative to the current cell matches the rule's source tile, the current
     * cell is replaced with the rule's target tile.
     * 根据配置的规则将一种瓦片替换为另一种瓦片。对每条规则，当「当前格子 + offset」处的瓦片与规则的源瓦片
     * 匹配时，就把当前格子替换为规则的目标瓦片。
     */
    class ReplaceTileBiomeDecorator : public BiomeDecorator<ReplaceTileBiomeDecoratorResource> {
        void DecorationImpl(const ChunkVertexVector2D &chunkVertex, WorldContext *worldContext,
                            TerrainResult *terrainResult, ReplaceTileBiomeDecoratorResource *decoratorResource,
                            BiomeResource *biomeResource,
                            std::unordered_map<TileLayerType, std::array<ResourceRef, CHUNK_AREA> > *
                            tilesRefMap) override;

    public:
        BiomeDecoratorPass GetPass() override;

        BiomeDecoratorType GetBiomeDecoratorType() override;
    };
}
