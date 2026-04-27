//
// Created by coldmint on 2026/4/27.
//

#ifndef GLIMMERWORKS_COLOR_H
#define GLIMMERWORKS_COLOR_H
#include <cstdint>

#ifndef UNIT_TEST_MODE
#include "SDL3/SDL_pixels.h"
#endif

namespace glimmer {
    struct Color {
        std::uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 0;
#ifndef UNIT_TEST_MODE
        [[nodiscard]] SDL_Color ToSDLColor() const;
#endif
    };
}

#endif //GLIMMERWORKS_COLOR_H
