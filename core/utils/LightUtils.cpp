//
// Created by coldmint on 2026/4/21.
//

#include "LightUtils.h"

#include <algorithm>

SDL_Color glimmer::LightUtils::ApplyLightingMask(const SDL_Color &light, const SDL_Color &mask) {
    const float intensity = light.a / 255.0F;
    const float maskStrength = mask.a / 255.0F;
    const float pass = 1.0F - maskStrength;
    const float finalScale = intensity * pass;
    Uint8 r = static_cast<Uint8>(std::min(255.0F, light.r * mask.r / 255.0F * finalScale));
    Uint8 g = static_cast<Uint8>(std::min(255.0F, light.g * mask.g / 255.0F * finalScale));
    Uint8 b = static_cast<Uint8>(std::min(255.0F, light.b * mask.b / 255.0F * finalScale));
    Uint8 a = static_cast<Uint8>(finalScale * 255.0F);
    return {r, g, b, a};
}

SDL_Color glimmer::LightUtils::MixLights(const SDL_Color &colorA, const SDL_Color &colorB) {
    Uint8 r = static_cast<Uint8>(std::min(255, static_cast<int>(colorA.r) + colorB.r));
    Uint8 g = static_cast<Uint8>(std::min(255, static_cast<int>(colorA.g) + colorB.g));
    Uint8 b = static_cast<Uint8>(std::min(255, static_cast<int>(colorA.b) + colorB.b));
    Uint8 a = std::max(colorA.a, colorB.a);
    return {r, g, b, a};
}
