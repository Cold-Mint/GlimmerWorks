//
// Created by Cold-Mint on 2025/12/19.
//

#ifndef GLIMMERWORKS_TILETYPE_H
#define GLIMMERWORKS_TILETYPE_H
#include <cstdint>

namespace glimmer {
    enum TileLayerType : uint8_t {
        /**
     * BackGround
     * 背景墙层
     */
        BackGround = 1,
        /**
         * Ground
         * 地面图层
         *
         * The ground layer allows players to stand on it.
         * 地面图层玩家可以站在上面。
         */
        Ground = 2,
    };

}

#endif //GLIMMERWORKS_TILETYPE_H
