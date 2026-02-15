//
// Created by coldmint on 2026/2/15.
//

#ifndef GLIMMERWORKS_PACKDEPENDENCE_H
#define GLIMMERWORKS_PACKDEPENDENCE_H
#include <string>
#include <cstdint>

namespace glimmer {
    /**
     * Package dependency information
     * 包依赖信息
     */
    struct PackDependence {
        std::string packId;
        uint32_t minVersion;
    };
}

#endif //GLIMMERWORKS_PACKDEPENDENCE_H
