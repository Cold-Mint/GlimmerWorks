//
// Created by coldmint on 2025/12/28.
//

#ifndef GLIMMERWORKS_DRAGSTATE_H
#define GLIMMERWORKS_DRAGSTATE_H
#include "DragSourceType.h"

namespace glimmer {
    class Item;
    class GameEntity;

    struct DragState {
        DragSourceType sourceType = DragSourceType::NONE;
        GameEntity *sourceContainer = nullptr; // For Inventory
        int sourceIndex = -1;

        // Payload
        const Item *dragedItem = nullptr;
        // For Editor dragging (if we support dragging abilities out)
        // For now, let's focus on Item dragging, and maybe Ability dragging if needed later
        // But the prompt implies dragging generic items into slots.
    };
}


#endif //GLIMMERWORKS_DRAGSTATE_H
