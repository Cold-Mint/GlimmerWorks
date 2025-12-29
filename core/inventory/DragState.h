//
// Created by coldmint on 2025/12/28.
//

#ifndef GLIMMERWORKS_DRAGSTATE_H
#define GLIMMERWORKS_DRAGSTATE_H
#include <memory>

#include "DragSourceType.h"

namespace glimmer {
    class Item;
    class GameEntity;

    struct DragState {
        DragSourceType sourceType = DragSourceType::NONE;
        GameEntity *sourceContainer = nullptr;
        int sourceIndex = -1;
        Item *dragedItem = nullptr;
    };
}


#endif //GLIMMERWORKS_DRAGSTATE_H
