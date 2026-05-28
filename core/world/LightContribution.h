//
// Created by Cold-Mint on 2026/4/24.
//

#pragma once
#include <memory>

#include "LightSource.h"

namespace glimmer {
    struct Color;

    class LightContribution {
        const LightSource *lightSource_ = nullptr;
        std::unique_ptr<Color> lightColor_;
        int rayIndex_ = 0;

    public:
        void SetRayIndex(int rayIndex);

        [[nodiscard]] int GetRayIndex() const;

        void SetLightSource(const LightSource *lightSource);

        [[nodiscard]] const LightSource *GetLightSource() const;

        void SetLightColor(std::unique_ptr<Color> lightColor);

        [[nodiscard]] std::unique_ptr<Color> TakeLightColor();

        [[nodiscard]] const Color *GetLightColor() const;
    };
}
