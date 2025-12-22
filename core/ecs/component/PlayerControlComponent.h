//
// Created by Cold-Mint on 2025/10/29.
//

#ifndef GLIMMERWORKS_PLAYERCONTROLCOMPONENT_H
#define GLIMMERWORKS_PLAYERCONTROLCOMPONENT_H
#include "../GameComponent.h"

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
    };
}

#endif //GLIMMERWORKS_PLAYERCONTROLCOMPONENT_H
