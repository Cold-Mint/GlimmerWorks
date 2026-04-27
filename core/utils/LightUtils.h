//
// Created by coldmint on 2026/4/21.
//

#ifndef GLIMMERWORKS_LIGHTUTILS_H
#define GLIMMERWORKS_LIGHTUTILS_H
#include <memory>
#include "core/math/Color.h"


namespace glimmer {
    class LightUtils {
    public:
        /**
        * 光照 × 光照遮罩 混合
        * light: 光源颜色(A=亮度)
        * mask:  遮罩颜色(RGB=滤色, A=遮挡强度 255=全挡, 0=不挡)
        */
        static std::unique_ptr<Color> ApplyLightingMask(const Color *light, const Color *mask);

        /**
    * 两束光照加法混合（越叠越亮）
    */
        static std::unique_ptr<Color> MixLights(const Color *colorA, const Color *colorB);
    };
}


#endif //GLIMMERWORKS_LIGHTUTILS_H
