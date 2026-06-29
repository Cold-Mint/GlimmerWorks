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
#include "TileLayerComponent.h"

#include <cassert>

#include "../../Constants.h"
#include "../../world/Tile.h"
#include "../../math/Vector2DI.h"
#include "../../world/generator/Chunk.h"
#include "../../math/CoordinateTransformer.h"
#include "core/world/WorldContext.h"


const glimmer::Tile* glimmer::TileLayerComponent::GetTile(const TileLayerType layerType,
                                                          const TileVector2D& tilePos) const
{
    if (worldContext_ == nullptr)
    {
        return nullptr;
    }
    const auto chunk = worldContext_->GetChunk(Chunk::TileCoordinatesToChunkVertexCoordinates(tilePos));
    if (chunk == nullptr)
    {
        return nullptr;
    }
    const TileVector2D pos = Chunk::TileCoordinatesToChunkRelativeCoordinates(tilePos);
    return chunk->GetTile(layerType, pos.y << CHUNK_SHIFT | pos.x);
}

std::vector<std::pair<glimmer::TileVector2D, std::vector<glimmer::TileSnapshot*>>>* glimmer::TileLayerComponent::
GetTopVisibleTileSnapshotsInViewport(uint8_t layerFilter, const SDL_FRect& worldViewport)
{
    if (worldContext_ == nullptr)
    {
        return nullptr;
    }
    const TileVector2D topLeft = CoordinateTransformer::WorldToTile({worldViewport.x, worldViewport.y});
    bool visibleTileRectChanged = false;
    Vector2DIFingerprint visibleTileTopLeftFingerprint = topLeft.GetFingerprint();
    if (visibleTileTopLeftFingerprint != visibleTileTopLeftFingerprint_)
    {
        visibleTileRectChanged = true;
    }
    //The purpose of adding "TILE_SIZE" in the lower right corner is to prevent blank areas from appearing.
    //右下角加TILE_SIZE的目的是，防止出现空白区域。
    const TileVector2D bottomRight = CoordinateTransformer::WorldToTile({
        worldViewport.x + worldViewport.w + TILE_SIZE,
        worldViewport.y + worldViewport.h + TILE_SIZE
    });
    Vector2DIFingerprint visibleTileBottomRightFingerprint = bottomRight.GetFingerprint();
    if (visibleTileBottomRightFingerprint != visibleTileBottomRightFingerprint_)
    {
        visibleTileRectChanged = true;
    }
    if (visibleTileRectChanged)
    {
        visibleTiles_.clear();
        bool allChunkExist = true;
        for (int y = topLeft.y; y <= bottomRight.y; ++y)
        {
            for (int x = topLeft.x; x <= bottomRight.x; ++x)
            {
                TileVector2D tileVector2D(x, y);
                const auto chunk = worldContext_->
                    GetChunk(Chunk::TileCoordinatesToChunkVertexCoordinates(tileVector2D));
                if (chunk == nullptr)
                {
                    allChunkExist = false;
                    continue;
                }
                visibleTiles_.emplace_back(tileVector2D, GetTopVisibleTileSnapshots(chunk, layerFilter, tileVector2D));
            }
        }
        if (allChunkExist)
        {
            //When all the blocks are available, we establish the cache.
            //当全部区块都存在时，我们建立缓存。
            visibleTileTopLeftFingerprint_ = visibleTileTopLeftFingerprint;
            visibleTileBottomRightFingerprint_ = visibleTileBottomRightFingerprint;
        }
    }
    return &visibleTiles_;
}


std::shared_ptr<glimmer::Tile> glimmer::TileLayerComponent::GetTileShared(const TileLayerType layerType,
                                                                          const TileVector2D& tilePos) const
{
    if (worldContext_ == nullptr)
    {
        return nullptr;
    }
    const auto chunk = worldContext_->GetChunk(Chunk::TileCoordinatesToChunkVertexCoordinates(tilePos));
    if (chunk == nullptr)
    {
        return nullptr;
    }
    const TileVector2D pos = Chunk::TileCoordinatesToChunkRelativeCoordinates(tilePos);
    return chunk->GetTileShared(layerType, pos.y << CHUNK_SHIFT | pos.x);
}

std::vector<glimmer::TileSnapshot*> glimmer::TileLayerComponent::GetTopVisibleTileSnapshots(const Chunk* chunk,
    uint8_t layerFilter, const TileVector2D& tilePos)
{
    if (chunk == nullptr)
    {
        return {};
    }
    const TileVector2D pos = Chunk::TileCoordinatesToChunkRelativeCoordinates(tilePos);
    return chunk->GetTopVisibleTileSnapshots(layerFilter, pos.y << CHUNK_SHIFT | pos.x);
}

const glimmer::Tile* glimmer::TileLayerComponent::GetSelfLayerTile(const TileVector2D& tilePos) const
{
    return GetTile(tileLayerType_, tilePos);
}

std::shared_ptr<glimmer::Tile> glimmer::TileLayerComponent::GetSelfLayerTileShared(const TileVector2D& tilePos) const
{
    return GetTileShared(tileLayerType_, tilePos);
}


bool glimmer::TileLayerComponent::CommitTileState(const TileLayerType layerType, const TileVector2D& tilePos,
                                                  const bool fallback) const
{
    if (worldContext_ == nullptr)
    {
        return false;
    }
    const auto chunk = worldContext_->GetChunk(Chunk::TileCoordinatesToChunkVertexCoordinates(tilePos));
    if (chunk == nullptr)
    {
        return false;
    }
    const TileVector2D pos = Chunk::TileCoordinatesToChunkRelativeCoordinates(tilePos);
    return chunk->CommitTileState(layerType, pos.y << CHUNK_SHIFT | pos.x, fallback);
}

TileStateMessage* glimmer::TileLayerComponent::GetTileStatePtr(const TileLayerType layerType,
                                                               const TileVector2D& tilePos) const
{
    if (worldContext_ == nullptr)
    {
        return nullptr;
    }
    const auto chunk = worldContext_->GetChunk(Chunk::TileCoordinatesToChunkVertexCoordinates(tilePos));
    if (chunk == nullptr)
    {
        return nullptr;
    }
    const TileVector2D pos = Chunk::TileCoordinatesToChunkRelativeCoordinates(tilePos);
    return chunk->GetTileState(layerType, pos.y << CHUNK_SHIFT | pos.x);
}

uint64_t glimmer::TileLayerComponent::GenerateTileFingerprint(const TileVector2D& tileTopLeftPosition,
                                                              const TileLayerType tileLayerType)
{
    uint64_t fingerprint = 0;
    fingerprint |= (static_cast<uint64_t>(static_cast<uint32_t>(tileTopLeftPosition.x)) & 0xFFFFFFULL) << 32;
    fingerprint |= (static_cast<uint64_t>(static_cast<uint32_t>(tileTopLeftPosition.y)) & 0xFFFFFFULL) << 8;
    fingerprint |= static_cast<uint64_t>(static_cast<uint8_t>(tileLayerType));
    return fingerprint;
}

const TileStateMessage* glimmer::TileLayerComponent::GetSelfLayerTileState(const TileVector2D& tilePos) const
{
    return GetTileStatePtr(tileLayerType_, tilePos);
}


TileStateMessage* glimmer::TileLayerComponent::GetSelfLayerTileStateMutable(const TileVector2D& tilePos) const
{
    return GetTileStatePtr(tileLayerType_, tilePos);
}


glimmer::TileLayerType glimmer::TileLayerComponent::GetTileLayerType() const
{
    return tileLayerType_;
}

void glimmer::TileLayerComponent::SetFocusPosition(const TileVector2D focusPosition)
{
    focusPosition_ = focusPosition;
}

const glimmer::TileVector2D& glimmer::TileLayerComponent::GetFocusPosition() const
{
    return focusPosition_;
}

GameComponentTypeMessage glimmer::TileLayerComponent::GetComponentTypeStatic()
{
    return COMPONENT_TILE_LAYER;
}

GameComponentTypeMessage glimmer::TileLayerComponent::GetComponentType()
{
    return GetComponentTypeStatic();
}
