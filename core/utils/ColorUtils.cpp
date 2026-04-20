//
// Created by coldmint on 2026/4/5.
//

#include "ColorUtils.h"

#include <algorithm>
#include <cmath>

SDL_Color glimmer::ColorUtils::LerpColor(const SDL_Color from, const SDL_Color to, const float percent) {
    const float blend_ratio = std::clamp(percent, 0.0F, 1.0F);
    const auto r = static_cast<Uint8>(static_cast<float>(from.r) + static_cast<float>(to.r - from.r) * blend_ratio);
    const auto g = static_cast<Uint8>(static_cast<float>(from.g) + static_cast<float>(to.g - from.g) * blend_ratio);
    const auto b = static_cast<Uint8>(static_cast<float>(from.b) + static_cast<float>(to.b - from.b) * blend_ratio);
    const auto a = static_cast<Uint8>(static_cast<float>(from.a) + static_cast<float>(to.a - from.a) * blend_ratio);
    return {r, g, b, a};
}

SDL_Color glimmer::ColorUtils::AverageColors(const std::vector<SDL_Color> &colors) {
    if (colors.empty()) {
        return {0, 0, 0, 0};
    }
    int total_r = 0, total_g = 0, total_b = 0, total_a = 0;
    for (const auto &color: colors) {
        total_r += color.r;
        total_g += color.g;
        total_b += color.b;
        total_a += color.a;
    }

    const auto count = static_cast<int>(colors.size());
    Uint8 r = static_cast<Uint8>(std::clamp(total_r / count, 0, 255));
    Uint8 g = static_cast<Uint8>(std::clamp(total_g / count, 0, 255));
    Uint8 b = static_cast<Uint8>(std::clamp(total_b / count, 0, 255));
    Uint8 a = static_cast<Uint8>(std::clamp(total_a / count, 0, 255));
    return {r, g, b, a};
}

SDL_FColor glimmer::ColorUtils::AdditiveBlend(const SDL_FColor firstColor, const SDL_FColor secondColor) {
    const float r = std::sqrt(firstColor.r * firstColor.r + secondColor.r * secondColor.r);
    const float g = std::sqrt(firstColor.g * firstColor.g + secondColor.g * secondColor.g);
    const float b = std::sqrt(firstColor.b * firstColor.b + secondColor.b * secondColor.b);
    const float a = std::sqrt(firstColor.a * firstColor.a + secondColor.a * secondColor.a);
    return SDL_FColor{
        std::clamp(r, 0.0F, 1.0F),
        std::clamp(g, 0.0F, 1.0F),
        std::clamp(b, 0.0F, 1.0F),
        std::clamp(a, 0.0F, 1.0F)
    };
}

SDL_Color glimmer::ColorUtils::ApplyOcclusion(SDL_Color lightColor, SDL_Color occlusionColor) {
    // const float alpha = static_cast<float>(occlusionColor.a) / 255.0F;
    // const float lightPower = 1.0f - alpha;
    // const float r = static_cast<float>(lightColor.r) / 255.0F * (static_cast<float>(occlusionColor.r) / 255.0F) *
    //                 lightPower;
    // const float g = static_cast<float>(lightColor.g) / 255.0F * (static_cast<float>(occlusionColor.g) / 255.0F) *
    //                 lightPower;
    // const float b = static_cast<float>(lightColor.b) / 255.0F * (static_cast<float>(occlusionColor.b) / 255.0F) *
    //                 lightPower;
    // SDL_Color result{};
    // result.r = static_cast<Uint8>(std::clamp(r * 255.0f, 0.0f, 255.0f));
    // result.g = static_cast<Uint8>(std::clamp(g * 255.0f, 0.0f, 255.0f));
    // result.b = static_cast<Uint8>(std::clamp(b * 255.0f, 0.0f, 255.0f));
    // result.a = lightColor.a;
    // return result;
    return lightColor;
}


SDL_FColor glimmer::ColorUtils::DecayColor(const SDL_FColor color, const float lightPercent) {
    const float realityLightPercent = std::clamp(lightPercent, 0.0F, 1.0F);
    SDL_FColor result;
    result.r = color.r * realityLightPercent;
    result.g = color.g * realityLightPercent;
    result.b = color.b * realityLightPercent;
    result.a = color.a * realityLightPercent;
    return result;
}

SDL_FColor glimmer::ColorUtils::ColorToFColor(const SDL_Color &sdlColor) {
    return {
        static_cast<float>(sdlColor.r) / 255.0F,
        static_cast<float>(sdlColor.g) / 255.0F,
        static_cast<float>(sdlColor.b) / 255.0F,
        static_cast<float>(sdlColor.a) / 255.0F
    };
}

SDL_Color glimmer::ColorUtils::FColorToColorRaw(const SDL_FColor &sdlFColor) {
    return SDL_Color{
        static_cast<Uint8>(std::clamp(sdlFColor.r, 0.0F, 1.0F) * 255),
        static_cast<Uint8>(std::clamp(sdlFColor.g, 0.0F, 1.0F) * 255),
        static_cast<Uint8>(std::clamp(sdlFColor.b, 0.0F, 1.0F) * 255),
        static_cast<Uint8>(std::clamp(sdlFColor.a, 0.0F, 1.0F) * 255)
    };
}

SDL_Color glimmer::ColorUtils::FColorToColorToneMapped(const SDL_FColor &sdlFColor, const float exposure) {
    return SDL_Color{
        static_cast<Uint8>(std::clamp(ToneMapReinhard(sdlFColor.r, exposure), 0.0F, 1.0F) * 255.0F),
        static_cast<Uint8>(std::clamp(ToneMapReinhard(sdlFColor.g, exposure), 0.0F, 1.0F) * 255.0F),
        static_cast<Uint8>(std::clamp(ToneMapReinhard(sdlFColor.b, exposure), 0.0F, 1.0F) * 255.0F),
        static_cast<Uint8>(std::clamp(ToneMapReinhard(sdlFColor.a, exposure), 0.0F, 1.0F) * 255.0F),
    };
}


float glimmer::ColorUtils::ToneMapReinhard(const float x, const float exposure) {
    return exposure * x / (1.0F + exposure * x);
}
