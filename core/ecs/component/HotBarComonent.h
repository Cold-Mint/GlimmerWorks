//
// Created by coldmint on 2025/12/18.
//

#ifndef GLIMMERWORKS_HOTBARCOMONENT_H
#define GLIMMERWORKS_HOTBARCOMONENT_H
#include "../../math/Vector2D.h"

namespace glimmer {
    class HotBarComponent {
        Vector2D position_;

    public:
        HotBarComponent(Vector2D position) : position_(position) {
        }

        /**
         * Get Position
         * 获取位置
         * @return
         */
        [[nodiscard]] Vector2D GetPosition() const;

        void SetPosition(Vector2D position) ;
    };
}

#endif //GLIMMERWORKS_HOTBARCOMONENT_H
