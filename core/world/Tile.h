//
// Created by Cold-Mint on 2025/11/2.
//

#ifndef GLIMMERWORKS_TILE_H
#define GLIMMERWORKS_TILE_H
#include <memory>
#include <SDL3/SDL.h>
#include "TilePhysicsType.h"

namespace glimmer {
    struct Tile {
        std::shared_ptr<SDL_Texture> texture;
        float humidity = 0.0F;
        TilePhysicsType physicsType = TilePhysicsType::None;
    };
}


#endif //GLIMMERWORKS_TILE_H
