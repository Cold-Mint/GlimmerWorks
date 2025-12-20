//
// Created by Cold-Mint on 2025/11/2.
//

#include "TileLayerComponent.h"

#include <assert.h>

#include "../../Constants.h"
#include "../../log/LogCat.h"
#include "../../world/Tile.h"
#include "../../math/Vector2DI.h"
#include "../../world/Chunk.h"


WorldVector2D glimmer::TileLayerComponent::TileToWorld(const WorldVector2D &tileLayerPos,
                                                       const TileVector2D &tilePos) {
    return {
        static_cast<float>(tilePos.x) * TILE_SIZE + tileLayerPos.x,
        static_cast<float>(tilePos.y) * TILE_SIZE + tileLayerPos.y
    };
}

TileVector2D glimmer::TileLayerComponent::WorldToTile(const WorldVector2D &tileLayerPos,
                                                      const WorldVector2D &worldPos) {
    const float localX = worldPos.x - tileLayerPos.x;
    const float localY = worldPos.y - tileLayerPos.y;

    return {
        static_cast<int>(std::floor(localX / TILE_SIZE)),
        static_cast<int>(std::floor(localY / TILE_SIZE))
    };
}


std::vector<std::pair<TileVector2D, glimmer::Tile> > glimmer::TileLayerComponent::GetTilesInViewport(
    const WorldVector2D &tileLayerPos, const SDL_FRect &worldViewport) const {
    const TileVector2D topLeft = WorldToTile(tileLayerPos, {worldViewport.x, worldViewport.y});
    //The purpose of adding "TILE_SIZE" in the lower right corner is to prevent blank areas from appearing.
    //右下角加TILE_SIZE的目的是，防止出现空白区域。
    const TileVector2D bottomRight = WorldToTile(tileLayerPos, {
                                                     worldViewport.x + worldViewport.w + TILE_SIZE,
                                                     worldViewport.y + worldViewport.h + TILE_SIZE
                                                 });
    std::vector<std::pair<TileVector2D, Tile> > visibleTiles;
    for (int y = topLeft.y; y <= bottomRight.y; ++y) {
        for (int x = topLeft.x; x <= bottomRight.x; ++x) {
            TileVector2D tileVector2D = TileVector2D(x, y);
            std::optional<Tile> tile = GetTile(tileVector2D);
            if (tile.has_value()) {
                visibleTiles.emplace_back(tileVector2D, tile.value());
            }
        }
    }
    return visibleTiles;
}

bool glimmer::TileLayerComponent::SetTile(const TileVector2D &tilePos, const Tile &tile) const {
    TileVector2D vertexCoordinate = Chunk::TileCoordinatesToChunkVertexCoordinates(tilePos);
    auto it = chunks_->find(vertexCoordinate);
    if (it == chunks_->end()) {
        LogCat::w("An attempt was made to place a tile in x=", tilePos.x, ",y=", tilePos.y,
                  " but the block it belongs to has not yet been loaded.");
        return false;
    }
    Chunk &chunk = it->second;
    chunk.SetTile(Chunk::TileCoordinatesToChunkRelativeCoordinates(tilePos), tile);
    return true;
}


std::optional<glimmer::Tile> glimmer::TileLayerComponent::GetTile(const TileVector2D &tilePos) const {
    TileVector2D vertexCoordinate = Chunk::TileCoordinatesToChunkVertexCoordinates(tilePos);
    auto it = chunks_->find(vertexCoordinate);
    if (it == chunks_->end()) {
        return std::nullopt;
    }
    Chunk &chunk = it->second;
    Tile tile = chunk.GetTile(GetTileLayerType(), Chunk::TileCoordinatesToChunkRelativeCoordinates(tilePos));
    return tile;
}

glimmer::TileLayerType glimmer::TileLayerComponent::GetTileLayerType() const {
    return tileLayerType_;
}
