//
// Created by Cold-Mint on 2025/11/2.
//

#ifndef GLIMMERWORKS_TILE_H
#define GLIMMERWORKS_TILE_H
#include "SDL3/SDL_pixels.h"

namespace glimmer
{
    enum class TilePhysicsType : uint8_t
    {
        /**
         * No physical effects (air, background)
         * 无物理效果（空气、背景）
         */
        None = 0,
        /**
         * Static solids (ground, rocks)
         * 静态固体（地面、岩石）
         */
        Static,
        /**
         * Can withstand gravity (sand, gravel)
         * 可受重力（沙子、碎石）
         */
        Dynamic,
        /**
         *  Fluids (such as water, lava)
         *  流体（如水、熔岩）
         */
        Fluid,
    };

    struct Tile
    {
        SDL_Color color = {255, 255, 255, 255};
        float humidity = 0.0F;
        TilePhysicsType physicsType = TilePhysicsType::None;
    };
}


#endif //GLIMMERWORKS_TILE_H
