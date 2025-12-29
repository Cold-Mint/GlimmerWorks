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

    /**
     * DragAndDrop
     * A static utility class responsible for item drag-and-drop logic and rendering.
     *
     * DragAndDrop
     * 一个负责物品拖拽逻辑与渲染的静态工具类。
     *
     * - Maintains a global drag state
     * - Handles slot interaction (click / drag / drop)
     * - Renders the dragged item following the mouse
     *
     * - 维护全局唯一的拖拽状态
     * - 处理槽位的点击 / 拖拽 / 放下逻辑
     * - 在鼠标位置渲染被拖拽的物品
     */
    class DragAndDrop {
        /**
        * Global drag state.
        *
        * 全局拖拽状态（一次只允许一个拖拽操作）。
        */
        static DragState state_;

    public:
        /**
       * Begin a drag operation.
       *
       * 开始一次拖拽操作。
       *
       * @param type      Source type of the drag (inventory, container, hotbar, etc.)
       *                  拖拽来源类型（背包、容器、快捷栏等）
       * @param container Entity that owns the source slot
       *                  拖拽来源所属的实体
       * @param index     Index of the source slot
       *                  拖拽来源槽位索引
       * @param item      Item being dragged (non-owning pointer)
       *                  被拖拽的物品指针（不拥有所有权）
       */
        static void BeginDrag(DragSourceType type, GameEntity *container, int index, Item *item);

        /**
        * End the current drag operation.
        *
        * 结束当前的拖拽操作，清空拖拽状态。
        */
        static void EndDrag();


        /**
         * Get the current drag state.
         *
         * 获取当前拖拽状态（只读）。
         *
         * @return Reference to the current DragState
         *         当前 DragState 的常量引用
         */
        static const DragState &GetState();

        /**
         * Check whether an item is currently being dragged.
         *
         * 判断当前是否处于拖拽状态。
         *
         * @return true if dragging, false otherwise
         *         正在拖拽返回 true，否则返回 false
         */
        static bool IsDragging();

        /**
    * Draw a slot and handle all related interactions.
    *
    * 绘制一个槽位，并处理其所有交互逻辑。
    *
    * This function is responsible for:
    * - Slot background rendering
    * - Item icon rendering
    * - Mouse click detection
    * - Drag start detection
    * - Drop handling
    *
    * 该函数同时负责：
    * - 槽位背景绘制
    * - 物品图标绘制
    * - 鼠标点击检测
    * - 拖拽开始检测
    * - 拖拽放下处理
    *
    * @param appContext Application context (input, UI state, etc.)
    *                   应用上下文（输入、UI 状态等）
    * @param renderer   SDL renderer
    *                   SDL 渲染器
    * @param x          X position of the slot
    *                   槽位左上角 X 坐标
    * @param y          Y position of the slot
    *                   槽位左上角 Y 坐标
    * @param size       Slot size (square)
    *                   槽位尺寸（正方形）
    * @param item       Item contained in this slot (nullable)
    *                   槽位中的物品（可为空）
    * @param isSelected Whether the slot is selected
    *                   槽位是否处于选中状态
    *
    * @param onDrop
    * Callback invoked when a dragged item is dropped onto this slot.
    * The actual item transfer / merge logic should be implemented here.
    *
    * 当有拖拽物品在此槽位上释放时触发的回调，
    * 实际的物品交换 / 合并逻辑应在此回调中实现。
    *
    * @param onDragStart
    * Callback invoked when dragging starts from this slot (optional).
    *
    * 当从该槽位开始拖拽时触发的回调（可选）。
    *
    * @param onClick
    * Callback invoked when the slot is clicked without dragging (optional).
    *
    * 当槽位被点击但未发生拖拽时触发的回调（可选）。
    */
        static void DrawSlot(const AppContext *appContext, SDL_Renderer *renderer,
                             float x, float y, float size,
                             const Item *item, bool isSelected,
                             const std::function<void(const DragState &)> &onDrop,
                             const std::function<void()> &onDragStart = nullptr,
                             const std::function<void()> &onClick = nullptr);

        /**
        * Render the dragged item icon at mouse position
         * 在鼠标位置呈现拖动的项目图标
         * @param renderer renderer SDL渲染器
         */
        static void RenderCombined(SDL_Renderer *renderer);
    };
}

#endif //GLIMMERWORKS_DRAGANDDROP_H
