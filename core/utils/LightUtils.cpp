//
// Created by coldmint on 2026/4/21.
//

#include "LightUtils.h"

#include <algorithm>
#include <cmath>

#include "core/log/LogCat.h"
#include "core/math/Color.h"


std::unique_ptr<glimmer::Color> glimmer::LightUtils::ApplyLightingMask(const Color *light, const Color *mask,
                                                                       const float tintFactor) {
    auto result = std::make_unique<Color>();
    const float intensity = static_cast<float>(light->a) / 255.0F;
    const float maskStrength = static_cast<float>(mask->a) / 255.0F;
    const float pass = 1.0F - maskStrength;
    const float finalScale = intensity * pass;
    result->r = static_cast<uint8_t>(std::lerp(static_cast<float>(light->r), static_cast<float>(mask->r), tintFactor));
    result->g = static_cast<uint8_t>(std::lerp(static_cast<float>(light->g), static_cast<float>(mask->g), tintFactor));
    result->b = static_cast<uint8_t>(std::lerp(static_cast<float>(light->b), static_cast<float>(mask->b), tintFactor));
    result->a = static_cast<uint8_t>(finalScale * 255.0F);
    return result;
}

std::unique_ptr<glimmer::Color> glimmer::LightUtils::MixLights(const Color *colorA, const Color *colorB) {
    auto result = std::make_unique<Color>();
    result->r = static_cast<uint8_t>(std::min(255, static_cast<int>(colorA->r) + colorB->r));
    result->g = static_cast<uint8_t>(std::min(255, static_cast<int>(colorA->g) + colorB->g));
    result->b = static_cast<uint8_t>(std::min(255, static_cast<int>(colorA->b) + colorB->b));
    result->a = std::max(colorA->a, colorB->a);
    return result;
}
