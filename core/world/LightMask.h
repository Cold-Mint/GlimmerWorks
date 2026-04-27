//
// Created by coldmint on 2026/4/15.
//

#ifndef GLIMMERWORKS_LIGHTMASK_H
#define GLIMMERWORKS_LIGHTMASK_H
#include "core/math/Color.h"


namespace glimmer {
    /**
     * LightMask
     * 光线遮照
     */
    class LightMask {
        Color lightMaskColor_ = {};

    public:
        explicit LightMask(const Color *lightMaskColor);

        [[nodiscard]] const Color *GetLightMaskColor() const;
    };
}


#endif //GLIMMERWORKS_LIGHTMASK_H
