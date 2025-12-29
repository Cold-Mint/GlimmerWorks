//
// Created by coldmint on 2025/12/28.
//

#ifndef GLIMMERWORKS_DRAGSOURCETYPE_H
#define GLIMMERWORKS_DRAGSOURCETYPE_H

namespace glimmer {
    /**
 * DragSourceType
 * Enumeration of possible drag source categories.
 *
 * DragSourceType
 * 拖拽来源类型枚举。
 */
    enum class DragSourceType {
        /**
   * No drag source.
   *
   * 无拖拽来源。
   */
        NONE,
        /**
   * Drag originated from an inventory container.
   *
   * 拖拽来源于背包容器。
   */
        INVENTORY
    };
}


#endif //GLIMMERWORKS_DRAGSOURCETYPE_H
