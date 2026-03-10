//
// Created by Cold-Mint on 2025/10/29.
//

#ifndef GLIMMERWORKS_PLAYERCONTROLCOMPONENT_H
#define GLIMMERWORKS_PLAYERCONTROLCOMPONENT_H

#include "MobComponent.h"
#include "core/ecs/GameEntity.h"

namespace glimmer {
    /**
     * 玩家控制组件，用于处理玩家的输入控制
     */
    class PlayerComponent final : public MobComponent {
    public:
        bool moveLeft = false;
        bool moveRight = false;
        bool jump = false;
        bool mouseLeftDown = false;
        float dropTimer = 0.0F;
        bool dropPressed = false;

        [[nodiscard]] uint32_t GetId() override;
    };
}

#endif //GLIMMERWORKS_PLAYERCONTROLCOMPONENT_H
