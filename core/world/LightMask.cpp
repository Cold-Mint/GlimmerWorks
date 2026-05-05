//
// Created by coldmint on 2026/4/15.
//

#include "LightMask.h"

glimmer::LightMask::LightMask(const Color *lightMaskColor, float tintFactor) {
    if (lightMaskColor == nullptr) {
        return;
    }
    lightMaskColor_ = *lightMaskColor;
    tintFactor_ = tintFactor;
}

float glimmer::LightMask::GetTintFactor() const {
    return tintFactor_;
}

const glimmer::Color *glimmer::LightMask::GetLightMaskColor() const {
    return &lightMaskColor_;
}
