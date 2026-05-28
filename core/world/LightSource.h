//
// Created by Cold-Mint on 2026/4/15.
//

#pragma once
#include "core/math/Vector2DI.h"
#include "core/math/Color.h"


namespace glimmer {
    class LightSource {
        TileVector2D center_ = {};
        int maxRadius_ = 0;
        Color emissionColor_ = {};

    public:
        explicit LightSource(const TileVector2D &center, int maxRadius, const Color &emissionColor);

        [[nodiscard]] int GetMaxRadius() const;

        [[nodiscard]] const TileVector2D &GetCenter() const;

        [[nodiscard]] const Color *GetEmissionColor() const;
    };
}
