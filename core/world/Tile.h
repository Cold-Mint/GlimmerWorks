//
// Created by Cold-Mint on 2025/11/2.
//

#ifndef GLIMMERWORKS_TILE_H
#define GLIMMERWORKS_TILE_H
#include <memory>
#include <SDL3/SDL.h>

#include "TileLayerType.h"
#include "TilePhysicsType.h"

namespace glimmer {
    struct Tile {
        std::string id;
        std::string name;
        std::string description;
        std::shared_ptr<SDL_Texture> texture;
        TilePhysicsType physicsType = TilePhysicsType::None;
        TileLayerType layerType = TileLayerType::Main;
    };
}


#endif //GLIMMERWORKS_TILE_H
