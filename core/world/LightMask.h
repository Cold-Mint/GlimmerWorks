//
// Created by coldmint on 2026/4/15.
//

#ifndef GLIMMERWORKS_LIGHTMASK_H
#define GLIMMERWORKS_LIGHTMASK_H
#include "SDL3/SDL_pixels.h"


namespace glimmer {
    /**
     * LightMask
     * 光线遮照
     */
    class LightMask {
        SDL_Color lightMaskColor_ = {};

    public:
        explicit LightMask(const SDL_Color *lightMaskColor);

        [[nodiscard]] const SDL_Color *GetLightMaskColor() const;
    };
}


#endif //GLIMMERWORKS_LIGHTMASK_H
