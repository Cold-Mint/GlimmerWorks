//
// Created by coldmint on 2026/4/15.
//

#ifndef GLIMMERWORKS_LIGHTMASK_H
#define GLIMMERWORKS_LIGHTMASK_H
#include "core/ecs/component/TileLayerComponent.h"
#include "SDL3/SDL_pixels.h"


namespace glimmer {
    /**
     * LightMask
     * 光线遮照
     */
    class LightMask {
        TileVector2D position_ = {};
        SDL_Color lightMaskColor_ = {};
        TileLayerType tileLayer_ = Ground;

    public:
        explicit LightMask(const TileVector2D &position, const TileLayerType &tileLayer,
                           const SDL_Color &lightMaskColor);


        [[nodiscard]] const TileLayerType &GetTileLayer() const;

        [[nodiscard]] const SDL_Color &GetLightMaskColor() const;

        [[nodiscard]] const TileVector2D &GetPosition() const;
    };
}


#endif //GLIMMERWORKS_LIGHTMASK_H
