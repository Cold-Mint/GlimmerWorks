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
    struct LightMask {
        TileVector2D position = {};
        SDL_Color emissionColor = {};
    };
}


#endif //GLIMMERWORKS_LIGHTMASK_H
