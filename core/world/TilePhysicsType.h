//
// Created by coldmint on 2025/12/3.
//

#ifndef GLIMMERWORKS_TILEPHYSICSTYPE_H
#define GLIMMERWORKS_TILEPHYSICSTYPE_H
#include <cstdint>

namespace glimmer {
    enum class TilePhysicsType : uint8_t {
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
}
#endif //GLIMMERWORKS_TILEPHYSICSTYPE_H
