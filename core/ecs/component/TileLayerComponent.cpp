//
// Created by Cold-Mint on 2025/11/2.
//

#include "TileLayerComponent.h"

#include "../../Constants.h"
#include "../../world/Tile.h"
#include "../../math/Vector2DI.h"


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
            long long key = EncodeTileKey(x, y);
            auto it = tileMap_.find(key);
            if (it != tileMap_.end()) { visibleTiles.emplace_back(TileVector2D{x, y}, it->second); }
        }
    }
    return visibleTiles;
}

void glimmer::TileLayerComponent::SetTile(const TileVector2D &tilePos, const Tile &tile) {
    tileMap_[EncodeTileKey(tilePos.x, tilePos.y)] = tile;
}

void glimmer::TileLayerComponent::ClearTile(const TileVector2D &tilePos) {
    const auto key = EncodeTileKey(tilePos.x, tilePos.y);
    tileMap_.erase(key);
}


std::optional<glimmer::Tile> glimmer::TileLayerComponent::GetTile(const TileVector2D &tilePos) const {
    long long key = EncodeTileKey(tilePos.x, tilePos.y);
    auto it = tileMap_.find(key);
    if (it != tileMap_.end())
        return it->second;
    return std::nullopt;
}

long long glimmer::TileLayerComponent::EncodeTileKey(int x, int y) {
    return static_cast<long long>(x) << 32 | static_cast<unsigned int>(y);
}
