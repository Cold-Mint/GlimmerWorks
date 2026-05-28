//
// Created by Cold-Mint on 2026/5/18.
//

#include "TileInstancePool.h"

#include "Tile.h"


std::shared_ptr<glimmer::Tile> glimmer::TileInstancePool::CreateTile(const AppContext *appContext,
                                                                     const TileResource *tileResource,
                                                                     uint64_t fingerprint) {
    const auto cache = tileInstanceMap_.find(fingerprint);
    if (cache != tileInstanceMap_.end()) {
        if (auto cachePtr = cache->second.lock()) {
            return cachePtr;
        }
        tileInstanceMap_.erase(cache);
    }
    std::unique_ptr<Tile> unique_tile = Tile::FromTileResource(appContext, tileResource);
    auto deleter = [this, fingerprint](const Tile *tile) {
        if (tile != nullptr) {
            delete tile;
        }
        tileInstanceMap_.erase(fingerprint);
    };
    std::shared_ptr<Tile> tile(unique_tile.release(), std::move(deleter));
    tileInstanceMap_.emplace(fingerprint, tile);
    return tile;
}
