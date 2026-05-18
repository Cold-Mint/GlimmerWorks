//
// Created by coldmint on 2026/5/18.
//

#include "TileInstancePool.h"

#include "Tile.h"

std::shared_ptr<glimmer::Tile> glimmer::TileInstancePool::CreateTile(const AppContext *appContext,
                                                                     const TileResource *tileResource,
                                                                     const ResourceRef &resourceRef) {
    const uint64_t signature = resourceRef.GetSignature();
    const auto cache = tileInstanceMap_.find(signature);
    if (cache != tileInstanceMap_.end()) {
        if (auto cachePtr = cache->second.lock()) {
            return cachePtr;
        }
        tileInstanceMap_.erase(cache);
    }
    std::unique_ptr<Tile> unique_tile = Tile::FromTileResource(appContext, tileResource, resourceRef);
    auto deleter = [this, signature](const Tile *tile) {
        if (tile != nullptr) {
            delete tile;
        }
        tileInstanceMap_.erase(signature);
    };
    std::shared_ptr<Tile> tile(unique_tile.release(), std::move(deleter));
    tileInstanceMap_.emplace(signature, tile);
    return tile;
}
