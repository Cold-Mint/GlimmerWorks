//
// Created by Cold-Mint on 2025/10/29.
//

#ifndef GLIMMERWORKS_PLAYERCONTROLCOMPONENT_H
#define GLIMMERWORKS_PLAYERCONTROLCOMPONENT_H
#include <vector>

#include "../GameComponent.h"
#include "core/ecs/GameEntity.h"

namespace glimmer {
    /**
     * 玩家控制组件，用于处理玩家的输入控制
     */
    class PlayerControlComponent final : public GameComponent {
    public:
        bool moveLeft = false;
        bool moveRight = false;
        bool jump = false;
        bool mouseLeftDown = false;
        float dropTimer = 0.0F;
        bool dropPressed = false;
        //The list of entities used for radiographic testing.
        //用于射线检测的实体列表。
        std::vector<GameEntity::ID> rayCast2DList;

        [[nodiscard]] uint32_t GetId() override;
    };
}

#endif //GLIMMERWORKS_PLAYERCONTROLCOMPONENT_H
