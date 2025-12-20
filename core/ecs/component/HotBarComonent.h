//
// Created by Cold-Mint on 2025/12/18.
//

#ifndef GLIMMERWORKS_HOTBARCOMONENT_H
#define GLIMMERWORKS_HOTBARCOMONENT_H
#include "../GameComponent.h"
#include "../../math/Vector2D.h"

namespace glimmer {
    class HotBarComponent : public GameComponent {
        Vector2D position_;
        int selectedSlot_ = 0;
        int maxSlot_ = 0;

    public:
        explicit HotBarComponent(const Vector2D position, const int maxSlot) : position_(position), maxSlot_(maxSlot) {
        }

        /**
         * Get Position
         * 获取位置
         * @return
         */
        [[nodiscard]] Vector2D GetPosition() const;

        void SetPosition(Vector2D position);

        [[nodiscard]] int GetSelectedSlot() const;

        void SetSelectedSlot(int selectedSlot);

        [[nodiscard]] int GetMaxSlot() const;
    };
}

#endif //GLIMMERWORKS_HOTBARCOMONENT_H
