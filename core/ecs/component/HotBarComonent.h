//
// Created by Cold-Mint on 2025/12/18.
//

#ifndef GLIMMERWORKS_HOTBARCOMONENT_H
#define GLIMMERWORKS_HOTBARCOMONENT_H
#include "../GameComponent.h"
#include "../../math/Vector2D.h"
#include <vector>

namespace glimmer {
    class GameEntity;
    class HotBarComponent : public GameComponent {
        Vector2D position_;
        int selectedSlot_ = 0;
        int maxSlot_ = 0;
        std::vector<GameEntity*> slotEntities_;

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

        void AddSlotEntity(GameEntity* entity) {
            slotEntities_.push_back(entity);
        }

        [[nodiscard]] const std::vector<GameEntity*>& GetSlotEntities() const {
            return slotEntities_;
        }

        [[nodiscard]] u_int32_t GetId() override;
    };
}

#endif //GLIMMERWORKS_HOTBARCOMONENT_H
