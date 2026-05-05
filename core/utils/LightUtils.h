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
         *
         * @param light 光源颜色
         * @param mask 遮挡颜色
         * @param tintFactor tintFactor染色权重（0为光源颜色，1为mask颜色）
         * @return
         */
        static std::unique_ptr<Color> ApplyLightingMask(const Color *light, const Color *mask, float tintFactor);

        /**
    * 两束光照加法混合（越叠越亮）
    */
        static std::unique_ptr<Color> MixLights(const Color *colorA, const Color *colorB);
    };
}


#endif //GLIMMERWORKS_LIGHTUTILS_H
