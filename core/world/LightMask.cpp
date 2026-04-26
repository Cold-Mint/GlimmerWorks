//
// Created by coldmint on 2026/4/15.
//

#include "LightMask.h"

glimmer::LightMask::LightMask(const SDL_Color *lightMaskColor) {
    if (lightMaskColor == nullptr) {
        return;
    }
    lightMaskColor_ = *lightMaskColor;
}

const SDL_Color *glimmer::LightMask::GetLightMaskColor() const {
    return &lightMaskColor_;
}
