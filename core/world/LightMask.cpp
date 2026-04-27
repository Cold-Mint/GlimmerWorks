//
// Created by coldmint on 2026/4/15.
//

#include "LightMask.h"

glimmer::LightMask::LightMask(const Color *lightMaskColor) {
    if (lightMaskColor == nullptr) {
        return;
    }
    lightMaskColor_ = *lightMaskColor;
}

const glimmer::Color *glimmer::LightMask::GetLightMaskColor() const {
    return &lightMaskColor_;
}
