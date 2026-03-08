//
// Created by coldmint on 2026/3/7.
//

#ifndef GLIMMERWORKS_SHAPETYPE_H
#define GLIMMERWORKS_SHAPETYPE_H
#include <cstdint>

namespace glimmer {
    enum class ShapeType : uint8_t {
        RECTANGLE = 0,
        CIRCLE = 1,
        ROUNDED_RECTANGLE = 2,
    };
}

#endif //GLIMMERWORKS_SHAPETYPE_H
