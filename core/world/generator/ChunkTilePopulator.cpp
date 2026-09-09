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
#include "ChunkTilePopulator.h"

#include "Chunk.h"
#include "core/mod/Resource.h"
#include "core/mod/ResourceLocator.h"

void glimmer::ChunkTilePopulator::PopulateSingleTilePosition(
    Chunk *chunk, const ResourceLocator *resourceLocator,
    const std::unordered_map<TileLayerType, std::array<ResourceRef, CHUNK_AREA> > &tilesRefMap,
    const int topLeftIndex) {
    for (const auto &[tileLayerType, tileArray]: tilesRefMap) {
        const ResourceRef &resourceRef = tileArray[topLeftIndex];
        const TileResource *tileResource = resourceLocator->FindTileRaw(&resourceRef);
        if (tileResource == nullptr) {
            continue;
        }
        for (int x = 0; x < tileResource->tileWidth; x++) {
            for (int y = 0; y < tileResource->tileHeight; y++) {
                const int unitIndex = topLeftIndex + y * CHUNK_SIZE + x;
                TileStateMessage *tileStateMessage = chunk->GetOrCreateTileState(tileLayerType, unitIndex);
                tileStateMessage->set_placesource(PLACE_SOURCE_WORLD_GEN);
                tileStateMessage->set_width(tileResource->tileWidth);
                tileStateMessage->set_height(tileResource->tileHeight);
                tileStateMessage->mutable_offset()->set_x(x);
                tileStateMessage->mutable_offset()->set_y(y);
                resourceRef.WriteResourceRefMessage(*tileStateMessage->mutable_resourceref());
                chunk->CommitTileState(BreakSource::ChunkGenerate, tileLayerType, unitIndex, true);
            }
        }
    }
}

void glimmer::ChunkTilePopulator::Populate(
    Chunk *chunk, const ResourceLocator *resourceLocator,
    const std::unordered_map<TileLayerType, std::array<ResourceRef, CHUNK_AREA> > &tilesRefMap) {
    for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
        for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
            const int topLeftIndex = localY * CHUNK_SIZE + localX;
            PopulateSingleTilePosition(chunk, resourceLocator, tilesRefMap, topLeftIndex);
        }
    }
}
