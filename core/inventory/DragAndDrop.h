//
// Created by coldmint on 2025/12/28.
//

#ifndef GLIMMERWORKS_DRAGANDDROP_H
#define GLIMMERWORKS_DRAGANDDROP_H

#include <functional>
#include <SDL3/SDL.h>

#include "DragSourceType.h"
#include "DragState.h"

namespace glimmer {
    class Item;
    class ItemAbility;
    class AppContext;



    class DragAndDrop {
        static DragState state_;

    public:
        static void BeginDrag(DragSourceType type, GameEntity *container, int index, const Item *item);

        static void EndDrag();

        static const DragState &GetState();

        static bool IsDragging();

        // Standard slot drawing with drag support
        // onDrop: Callback when something is dropped onto this slot. 
        //         Returns true if drop was handled.
        static void DrawSlot(AppContext *appContext, SDL_Renderer *renderer,
                             float x, float y, float size,
                             const Item *item, bool isSelected,
                             std::function<void(const DragState &)> onDrop,
                             std::function<void()> onDragStart = nullptr,
                             std::function<void()> onClick = nullptr);

        /**
         * 在鼠标位置呈现拖动的项目图标
         * Render the dragged item icon at mouse position
         * @param renderer
         */
        static void RenderCombined(SDL_Renderer *renderer);
    };
}

#endif //GLIMMERWORKS_DRAGANDDROP_H
