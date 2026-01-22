//
// Created by coldmint on 2025/12/28.
//

#ifndef GLIMMERWORKS_DRAGSTATE_H
#define GLIMMERWORKS_DRAGSTATE_H
#include <cstdint>
#include "DragSourceType.h"

namespace glimmer {
    class Item;

    struct DragState {
        DragSourceType sourceType = DragSourceType::NONE;
        uint32_t sourceContainer = 0;
        int sourceIndex = -1;
        Item *dragedItem = nullptr;
    };
}


#endif //GLIMMERWORKS_DRAGSTATE_H
