//
// Created by Cold-Mint on 2025/12/19.
//

#ifndef GLIMMERWORKS_TILETYPE_H
#define GLIMMERWORKS_TILETYPE_H

namespace glimmer {
    enum class TileLayerType {
        /**
         * Main
         * 主要的瓦片层
         */
        Main = 0,
        /**
         * Wire
         * 电线层
         */
        Wire,
        /**
         * WaterPipe
         * 水管
         */
        WaterPipe,
        /**
         * SignalLine
         * 信号线
         */
        SignalLine
    };
}

#endif //GLIMMERWORKS_TILETYPE_H
