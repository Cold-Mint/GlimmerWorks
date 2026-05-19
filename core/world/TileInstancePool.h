//
// Created by coldmint on 2026/5/18.
//

#pragma once
#include <memory>
#include <unordered_map>

namespace glimmer {
    class AppContext;
    class Tile;
    class TileResource;
    class ResourceRef;

    class TileInstancePool {
        std::unordered_map<u_int64_t, std::weak_ptr<Tile> > tileInstanceMap_;

    public:
        std::shared_ptr<Tile> CreateTile(const AppContext *appContext,
                                         const TileResource *tileResource,
                                         const ResourceRef &resourceRef);
    };
}
