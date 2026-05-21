//
// Created by Cold-Mint on 2025/11/2.
//

#include "TileLayerComponent.h"

#include <cassert>

#include "../../Constants.h"
#include "../../world/Tile.h"
#include "../../math/Vector2DI.h"
#include "../../world/generator/Chunk.h"
#include "core/world/WorldContext.h"


const glimmer::Tile *glimmer::TileLayerComponent::GetTile(const TileLayerType layerType,
                                                          const TileVector2D &tilePos) const {
    if (worldContext_ == nullptr) {
        return nullptr;
    }
    const auto chunk = worldContext_->GetChunk(Chunk::TileCoordinatesToChunkVertexCoordinates(tilePos));
    if (chunk == nullptr) {
        return nullptr;
    }
    const TileVector2D pos = Chunk::TileCoordinatesToChunkRelativeCoordinates(tilePos);
    return chunk->GetTile(layerType, pos.y << CHUNK_SHIFT | pos.x);
}

std::shared_ptr<glimmer::Tile> glimmer::TileLayerComponent::GetTilePtr(const TileLayerType layerType,
                                                                       const TileVector2D &tilePos) const {
    if (worldContext_ == nullptr) {
        return nullptr;
    }
    const auto chunk = worldContext_->GetChunk(Chunk::TileCoordinatesToChunkVertexCoordinates(tilePos));
    if (chunk == nullptr) {
        return nullptr;
    }
    const TileVector2D pos = Chunk::TileCoordinatesToChunkRelativeCoordinates(tilePos);
    return chunk->GetTilePtr(layerType, pos.y << CHUNK_SHIFT | pos.x);
}


std::vector<const glimmer::Tile *> glimmer::TileLayerComponent::GetTopVisibleTiles(
    const Chunk *chunk, const uint8_t layerFilter,
    const TileVector2D &tilePos) {
    if (chunk == nullptr) {
        return {};
    }
    const TileVector2D pos = Chunk::TileCoordinatesToChunkRelativeCoordinates(tilePos);
    return chunk->GetTopVisibleTiles(layerFilter, pos.y << CHUNK_SHIFT | pos.x);
}

WorldVector2D glimmer::TileLayerComponent::TileToWorld(
    const TileVector2D &tilePos) {
    return {
        static_cast<float>(tilePos.x) * TILE_SIZE,
        static_cast<float>(tilePos.y) * TILE_SIZE
    };
}

WorldVector2D glimmer::TileLayerComponent::TileToWorldCenter(const TileVector2D &tilePos) {
    return {
        (static_cast<float>(tilePos.x) + 0.5F) * TILE_SIZE,
        (static_cast<float>(tilePos.y) + 0.5F) * TILE_SIZE
    };
}

TileVector2D glimmer::TileLayerComponent::WorldToTile(const WorldVector2D &worldPos) {
    return {
        static_cast<int>(std::floor(worldPos.x / TILE_SIZE + 0.5F)),
        static_cast<int>(std::floor(worldPos.y / TILE_SIZE + 0.5F))
    };
}

std::vector<std::pair<TileVector2D, std::vector<const glimmer::Tile *> > > glimmer::TileLayerComponent::
GetTopVisibleTilesInViewport(const uint8_t layerFilter, const SDL_FRect &worldViewport) const {
    if (worldContext_ == nullptr) {
        return {};
    }
    const TileVector2D topLeft = WorldToTile({worldViewport.x, worldViewport.y});
    //The purpose of adding "TILE_SIZE" in the lower right corner is to prevent blank areas from appearing.
    //右下角加TILE_SIZE的目的是，防止出现空白区域。
    const TileVector2D bottomRight = WorldToTile({
        worldViewport.x + worldViewport.w + TILE_SIZE,
        worldViewport.y + worldViewport.h + TILE_SIZE
    });
    std::vector<std::pair<TileVector2D, std::vector<const Tile *> > > visibleTiles;
    for (int y = topLeft.y; y <= bottomRight.y; ++y) {
        for (int x = topLeft.x; x <= bottomRight.x; ++x) {
            TileVector2D tileVector2D(x, y);
            const auto chunk = worldContext_->GetChunk(Chunk::TileCoordinatesToChunkVertexCoordinates(tileVector2D));
            if (chunk == nullptr) {
                continue;
            }
            visibleTiles.emplace_back(tileVector2D, GetTopVisibleTiles(chunk, layerFilter, tileVector2D));
        }
    }
    return visibleTiles;
}

const glimmer::Tile *glimmer::TileLayerComponent::GetSelfLayerTile(const TileVector2D &tilePos) const {
    return GetTile(GetTileLayerType(), tilePos);
}


std::shared_ptr<glimmer::Tile> glimmer::TileLayerComponent::GetSelfLayerTilePtr(const TileVector2D &tilePos) const {
    return GetTilePtr(GetTileLayerType(), tilePos);
}


bool glimmer::TileLayerComponent::CommitTileState(const TileLayerType layerType, const TileVector2D &tilePos,
                                                  const bool fallback) const {
    if (worldContext_ == nullptr) {
        return false;
    }
    const auto chunk = worldContext_->GetChunk(Chunk::TileCoordinatesToChunkVertexCoordinates(tilePos));
    if (chunk == nullptr) {
        return false;
    }
    const TileVector2D pos = Chunk::TileCoordinatesToChunkRelativeCoordinates(tilePos);
    return chunk->CommitTileState(layerType, pos.y << CHUNK_SHIFT | pos.x, fallback);
}

TileStateMessage *glimmer::TileLayerComponent::GetTileStatePtr(const TileLayerType layerType,
                                                               const TileVector2D &tilePos) const {
    if (worldContext_ == nullptr) {
        return nullptr;
    }
    const auto chunk = worldContext_->GetChunk(Chunk::TileCoordinatesToChunkVertexCoordinates(tilePos));
    if (chunk == nullptr) {
        return nullptr;
    }
    const TileVector2D pos = Chunk::TileCoordinatesToChunkRelativeCoordinates(tilePos);
    return chunk->GetTileState(layerType, pos.y << CHUNK_SHIFT | pos.x);
}

const TileStateMessage *glimmer::TileLayerComponent::GetTileState(const TileLayerType layerType,
                                                                  const TileVector2D &tilePos) const {
    return GetTileStatePtr(layerType, tilePos);
}


glimmer::TileLayerType glimmer::TileLayerComponent::GetTileLayerType() const {
    return tileLayerType_;
}

void glimmer::TileLayerComponent::SetFocusPosition(const TileVector2D focusPosition) {
    focusPosition_ = focusPosition;
}

const TileVector2D &glimmer::TileLayerComponent::GetFocusPosition() const {
    return focusPosition_;
}


uint32_t glimmer::TileLayerComponent::GetId() {
    return COMPONENT_ID_TILE_LAYER;
}
