//
// Created by coldmint on 2026/4/5.
//

#include "ColorUtils.h"

#include <algorithm>
#include <cmath>


glimmer::Color glimmer::ColorUtils::LerpColor(Color from, Color to, float percent) {
    const float blend_ratio = std::clamp(percent, 0.0F, 1.0F);
    const auto r = static_cast<uint8_t>(static_cast<float>(from.r) + static_cast<float>(to.r - from.r) * blend_ratio);
    const auto g = static_cast<uint8_t>(static_cast<float>(from.g) + static_cast<float>(to.g - from.g) * blend_ratio);
    const auto b = static_cast<uint8_t>(static_cast<float>(from.b) + static_cast<float>(to.b - from.b) * blend_ratio);
    const auto a = static_cast<uint8_t>(static_cast<float>(from.a) + static_cast<float>(to.a - from.a) * blend_ratio);
    return {r, g, b, a};
}

glimmer::Color glimmer::ColorUtils::AverageColors(const std::vector<Color> &colors) {
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
    uint8_t r = static_cast<uint8_t>(std::clamp(total_r / count, 0, 255));
    uint8_t g = static_cast<uint8_t>(std::clamp(total_g / count, 0, 255));
    uint8_t b = static_cast<uint8_t>(std::clamp(total_b / count, 0, 255));
    uint8_t a = static_cast<uint8_t>(std::clamp(total_a / count, 0, 255));
    return {r, g, b, a};
}

glimmer::Color glimmer::ColorUtils::AdditiveBlend(const Color firstColor, const Color secondColor) {
    Color result{};
    result.r = static_cast<uint8_t>(std::min(255, static_cast<int>(firstColor.r) + secondColor.r));
    result.g = static_cast<uint8_t>(std::min(255, static_cast<int>(firstColor.g) + secondColor.g));
    result.b = static_cast<uint8_t>(std::min(255, static_cast<int>(firstColor.b) + secondColor.b));
    result.a = std::max(firstColor.a, secondColor.a);
    return result;
}
