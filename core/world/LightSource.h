//
// Created by coldmint on 2026/4/15.
//

#ifndef GLIMMERWORKS_LIGHTSOURCE_H
#define GLIMMERWORKS_LIGHTSOURCE_H
#include "core/ecs/component/TileLayerComponent.h"
#include "SDL3/SDL_pixels.h"


namespace glimmer {
    class LightSource {
        TileVector2D center_ = {};
        int maxRadius_ = 0;
        SDL_Color emissionColor_ = {};
        TileLayerType tileLayer_ = Ground;

    public:
        explicit LightSource(const TileVector2D &center, int maxRadius, const SDL_Color &emissionColor,
                             const TileLayerType &tileLayer);

        [[nodiscard]] int GetMaxRadius() const;

        [[nodiscard]] const TileLayerType &GetTileLayerType() const;

        [[nodiscard]] const TileVector2D &GetCenter() const;

        [[nodiscard]] const SDL_Color &GetEmissionColor() const;
    };
}


#endif //GLIMMERWORKS_LIGHTSOURCE_H
