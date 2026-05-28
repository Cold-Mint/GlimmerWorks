//
// Created by Cold-Mint on 2026/4/15.
//
#pragma once
#include "core/math/Color.h"


namespace glimmer {
    /**
     * LightMask
     * 光线遮照
     */
    class LightMask {
        Color lightMaskColor_ = {};
        float tintFactor_ = 0.0F;

    public:
        explicit LightMask(const Color *lightMaskColor, float tintFactor);

        [[nodiscard]] float GetTintFactor() const;

        [[nodiscard]] const Color *GetLightMaskColor() const;
    };
}
