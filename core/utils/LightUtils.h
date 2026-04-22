//
// Created by coldmint on 2026/4/21.
//

#ifndef GLIMMERWORKS_LIGHTUTILS_H
#define GLIMMERWORKS_LIGHTUTILS_H
#include <memory>

#include "SDL3/SDL_pixels.h"


namespace glimmer {
    class LightUtils {
    public:
        /**
        * 光照 × 光照遮罩 混合
        * light: 光源颜色(A=亮度)
        * mask:  遮罩颜色(RGB=滤色, A=遮挡强度 255=全挡, 0=不挡)
        */
        static std::unique_ptr<SDL_Color> ApplyLightingMask(const SDL_Color *light, const SDL_Color *mask);

        /**
    * 两束光照加法混合（越叠越亮）
    */
        static std::unique_ptr<SDL_Color> MixLights(const SDL_Color *colorA, const SDL_Color *colorB);
    };
}


#endif //GLIMMERWORKS_LIGHTUTILS_H
