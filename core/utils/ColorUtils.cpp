//
// Created by coldmint on 2026/4/5.
//

#include "ColorUtils.h"

#include <algorithm>

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

SDL_Color glimmer::ColorUtils::ApplyOcclusion(const SDL_Color lightColor, const SDL_Color occlusionColor) {
    // Blocking intensity: 0.0 = No blocking, 1.0 = Complete blocking
    // 遮挡强度：0.0 = 不遮挡，1.0 = 完全遮挡
    const float occlusion = static_cast<float>(occlusionColor.a) / 255.0F;

    // The final light intensity = original intensity * (1 - occlusion intensity)
    // 最终光线强度 = 原强度 * (1 - 遮挡强度)
    // The stronger the obstruction, the less light remains.
    // 遮挡越强，剩下的光越少
    const float lightPower = 1.0F - occlusion;

    SDL_Color result{};

    // 1. Coloring: When light passes through an obstruction, it will acquire the color of the obstruction.
    // 1. 染色：光线穿过遮挡物，会带上遮挡物的颜色
    result.r = static_cast<Uint8>(std::clamp(
        static_cast<float>(lightColor.r) * (static_cast<float>(occlusionColor.r) / 255.0F) * lightPower,
        0.0F, 255.0F
    ));
    result.g = static_cast<Uint8>(std::clamp(
        static_cast<float>(lightColor.g) * (static_cast<float>(occlusionColor.g) / 255.0F) * lightPower,
        0.0F, 255.0F
    ));
    result.b = static_cast<Uint8>(std::clamp(
        static_cast<float>(lightColor.b) * (static_cast<float>(occlusionColor.b) / 255.0F) * lightPower,
        0.0F, 255.0F
    ));

    // 2. Brightness (Alpha) decreases with the intensity of occlusion.
    // 2. 亮度（Alpha）随遮挡强度衰减
    result.a = static_cast<Uint8>(std::clamp(
        static_cast<float>(lightColor.a) * lightPower,
        0.0F, 255.0F
    ));

    return result;
}

SDL_Color glimmer::ColorUtils::DecayColor(const SDL_Color color, float decayPercent) {
    decayPercent = std::clamp(decayPercent, 0.0f, 1.0f);

    // 最终亮度 = 1 - 衰减百分比
    const float brightness = 1.0f - decayPercent;

    SDL_Color result;

    // RGB 按亮度衰减
    result.r = static_cast<Uint8>(std::clamp(
        static_cast<float>(color.r) * brightness, 0.0f, 255.0f
    ));
    result.g = static_cast<Uint8>(std::clamp(
        static_cast<float>(color.g) * brightness, 0.0f, 255.0f
    ));
    result.b = static_cast<Uint8>(std::clamp(
        static_cast<float>(color.b) * brightness, 0.0f, 255.0f
    ));

    // Alpha（光照强度）也一起衰减！
    result.a = static_cast<Uint8>(std::clamp(
        static_cast<float>(color.a) * brightness, 0.0f, 255.0f
    ));

    return result;
}
