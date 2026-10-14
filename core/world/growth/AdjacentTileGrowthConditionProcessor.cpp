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
#include "AdjacentTileGrowthConditionProcessor.h"

#include "core/config/Constants.h"
#include "core/math/CoordinateTransformer.h"
#include "core/math/TileVector2D.h"
#include "core/mod/Resource.h"
#include "core/world/ChunkManager.h"
#include "core/world/TileResourceData.h"
#include "core/world/WorldContext.h"
#include "core/world/generator/Chunk.h"

glimmer::GrowthConditionProcessorType
glimmer::AdjacentTileGrowthConditionProcessor::GetGrowthConditionProcessorType() {
    return GrowthConditionProcessorType::AdjacentTile;
}

bool glimmer::AdjacentTileGrowthConditionProcessor::Match(const WorldContext *worldContext,
                                                          const TileVector2D &position,
                                                          const IGrowthConditionResource *growthConditionResource) {
    const auto adjacentTileCondition = dynamic_cast<const AdjacentTileGrowthConditionResource *>(
        growthConditionResource);
    if (adjacentTileCondition == nullptr || worldContext == nullptr) {
        return false;
    }
    const uint64_t requiredTagId = adjacentTileCondition->cachedTagId;
    if (requiredTagId == 0) {
        return false;
    }
    const TileVector2D adjacentPosition(position.x + adjacentTileCondition->offset.x,
                                        position.y + adjacentTileCondition->offset.y);
    const ChunkManager *chunkManager = worldContext->GetChunkManager();
    if (chunkManager == nullptr) {
        return false;
    }
    const Chunk *chunk = chunkManager->GetChunk(
        CoordinateTransformer::TileToChunkVertex(adjacentPosition));
    if (chunk == nullptr) {
        return false;
    }
    const ChunkRelativeVector2D relative = CoordinateTransformer::TileToChunkRelative(adjacentPosition);
    const size_t index = CoordinateTransformer::GetArrayIndex(relative.x, relative.y, CHUNK_SHIFT);
    for (int i = 0; i < TILE_LAYER_TYPE_COUNT; ++i) {
        const auto layerType = static_cast<TileLayerType>(1 << i);
        if ((std::byte{adjacentTileCondition->layerMask} & static_cast<std::byte>(std::to_underlying(layerType))) ==
            std::byte{0}) {
            continue;
        }
        const Tile *tile = chunk->GetTile(layerType, index);
        if (tile == nullptr) {
            continue;
        }
        const TileResourceData *tileData = tile->GetResourceData();
        if (tileData == nullptr) {
            continue;
        }
        for (const auto &tag: tileData->GetTags()) {
            if (tag.cachedTagId == requiredTagId) {
                return true;
            }
        }
    }
    return false;
}
