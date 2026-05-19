//@genCode
//
// Created by Cold-Mint on 2026/2/15.
//

#pragma once
#include <string>
#include <cstdint>

namespace glimmer {
    /**
     * Package dependency information
     * 包依赖信息
     */
    //@genNextLine(PackDependence|包依赖信息)
    struct PackDependence {
        //@genNextLine(PackDependence.packId|依赖包的唯一标识)
        std::string packId;
        //@genNextLine(PackDependence.minVersion|依赖包的最低版本号)
        uint32_t minVersion;
    };
}
