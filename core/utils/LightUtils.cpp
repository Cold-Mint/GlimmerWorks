//
// Created by coldmint on 2026/4/21.
//

#include "LightUtils.h"

#include <algorithm>


std::unique_ptr<SDL_Color> glimmer::LightUtils::ApplyLightingMask(const SDL_Color *light, const SDL_Color *mask) {
    auto result = std::make_unique<SDL_Color>();
    const float intensity = static_cast<float>(light->a) / 255.0F;
    const float maskStrength = static_cast<float>(mask->a) / 255.0F;
    const float pass = 1.0F - maskStrength;
    const float finalScale = intensity * pass;
    result->r = static_cast<Uint8>(
        std::min(255.0F, static_cast<float>(light->r) * static_cast<float>(mask->r) / 255.0F * finalScale));
    result->g = static_cast<Uint8>(std::min(
        255.0F, static_cast<float>(light->g) * static_cast<float>(mask->g) / 255.0F * finalScale));
    result->b = static_cast<Uint8>(std::min(
        255.0F, static_cast<float>(light->b) * static_cast<float>(mask->b) / 255.0F * finalScale));
    result->a = static_cast<Uint8>(finalScale * 255.0F);
    return result;
}

std::unique_ptr<SDL_Color> glimmer::LightUtils::MixLights(const SDL_Color *colorA, const SDL_Color *colorB) {
    auto result = std::make_unique<SDL_Color>();
    result->r = static_cast<Uint8>(std::min(255, static_cast<int>(colorA->r) + colorB->r));
    result->g = static_cast<Uint8>(std::min(255, static_cast<int>(colorA->g) + colorB->g));
    result->b = static_cast<Uint8>(std::min(255, static_cast<int>(colorA->b) + colorB->b));
    result->a = std::max(colorA->a, colorB->a);
    return result;
}
