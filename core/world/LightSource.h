//
// Created by coldmint on 2026/4/15.
//

#ifndef GLIMMERWORKS_LIGHTSOURCE_H
#define GLIMMERWORKS_LIGHTSOURCE_H
#include "core/ecs/component/TileLayerComponent.h"
#include "SDL3/SDL_pixels.h"


namespace glimmer {
    struct LightSource {
        TileVector2D center = {};
        int maxRadius = 0;
        float lightAttenuationPerCell = 1.0F;
        SDL_Color emissionColor = {};
    };
}


#endif //GLIMMERWORKS_LIGHTSOURCE_H
