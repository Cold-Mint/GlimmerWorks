//
// Created by coldmint on 2026/4/27.
//

#include "Color.h"
#ifndef UNIT_TEST_MODE
SDL_Color glimmer::Color::ToSDLColor() const {
    return SDL_Color{r, g, b, a};
}
#endif
