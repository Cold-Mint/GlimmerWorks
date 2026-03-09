//@genCode
//
// Created by coldmint on 2026/3/9.
//

#ifndef GLIMMERWORKS_BOX2DFILTER_H
#define GLIMMERWORKS_BOX2DFILTER_H

#include "box2d/types.h"

namespace glimmer {
    //@genNextLine(Box2dFilter|物理层过滤)
    struct Box2dFilter {
        //@genNextLine(categoryBits|分类的物理层)
        uint64_t categoryBits = 0;
        //@genNextLine(maskBits|可碰撞的物理层)
        uint64_t maskBits = 0;

        [[nodiscard]] b2QueryFilter Tob2QueryFilter() const;
    };
}

#endif //GLIMMERWORKS_BOX2DFILTER_H
