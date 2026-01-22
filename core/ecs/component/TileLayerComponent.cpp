//
// Created by Cold-Mint on 2025/11/2.
//

#include "TileLayerComponent.h"

#include <cassert>

#include "../../Constants.h"
#include "../../log/LogCat.h"
#include "../../world/Tile.h"
#include "../../math/Vector2DI.h"
#include "../../world/Chunk.h"


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


std::vector<std::pair<TileVector2D, glimmer::Tile *> > glimmer::TileLayerComponent::GetTilesInViewport(
    const SDL_FRect &worldViewport) const {
    const TileVector2D topLeft = WorldToTile({worldViewport.x, worldViewport.y});
    //The purpose of adding "TILE_SIZE" in the lower right corner is to prevent blank areas from appearing.
    //右下角加TILE_SIZE的目的是，防止出现空白区域。
    const TileVector2D bottomRight = WorldToTile({
        worldViewport.x + worldViewport.w + TILE_SIZE,
        worldViewport.y + worldViewport.h + TILE_SIZE
    });
    std::vector<std::pair<TileVector2D, Tile *> > visibleTiles;
    for (int y = topLeft.y; y <= bottomRight.y; ++y) {
        for (int x = topLeft.x; x <= bottomRight.x; ++x) {
            auto tileVector2D = TileVector2D(x, y);
            if (Tile *tile = GetTile(tileVector2D); tile != nullptr) {
                visibleTiles.emplace_back(tileVector2D, tile);
            }
        }
    }
    return visibleTiles;
}

bool glimmer::TileLayerComponent::SetTile(const TileVector2D &tilePos, std::unique_ptr<Tile> tile) const {
    const auto chunk = Chunk::GetChunkByTileVector2D(chunks_, tilePos);
    if (chunk == nullptr) {
        return false;
    }
    chunk->SetTile(Chunk::TileCoordinatesToChunkRelativeCoordinates(tilePos), std::move(tile));
    return true;
}


glimmer::Tile *glimmer::TileLayerComponent::GetTile(const TileVector2D &tilePos) const {
    const auto chunk = Chunk::GetChunkByTileVector2D(chunks_, tilePos);
    if (chunk == nullptr) {
        return nullptr;
    }
    return chunk->GetTile(GetTileLayerType(), Chunk::TileCoordinatesToChunkRelativeCoordinates(tilePos));
}

std::unique_ptr<glimmer::Tile> glimmer::TileLayerComponent::ReplaceTile(const TileVector2D &tileVector2d,
                                                                        std::unique_ptr<Tile> newTile) const {
    const auto chunk = Chunk::GetChunkByTileVector2D(chunks_, tileVector2d);
    if (chunk == nullptr) {
        return nullptr;
    }
    return chunk->ReplaceTile(GetTileLayerType(), Chunk::TileCoordinatesToChunkRelativeCoordinates(tileVector2d),
                              std::move(newTile));
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

u_int32_t glimmer::TileLayerComponent::GetId() {
    return COMPONENT_ID_TILE_LAYER;
}
