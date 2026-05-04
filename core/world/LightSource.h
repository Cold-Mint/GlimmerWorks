//
// Created by coldmint on 2026/4/15.
//

#ifndef GLIMMERWORKS_LIGHTSOURCE_H
#define GLIMMERWORKS_LIGHTSOURCE_H
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


#endif //GLIMMERWORKS_LIGHTSOURCE_H
