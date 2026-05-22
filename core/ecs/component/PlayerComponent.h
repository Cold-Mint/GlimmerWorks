//
// Created by Cold-Mint on 2025/10/29.
//

#pragma once

#include "MobComponent.h"
#include "core/inventory/Item.h"

namespace glimmer {
    /**
     * 玩家控制组件，用于处理玩家的输入控制
     */
    class PlayerComponent final : public MobComponent {
    public:
        //If the player does not press the left or right key, then this value is 0. Pressing A is -1, and pressing D is 1.0.
        //如果玩家没有按下左键或右键那么此值为0,按下A为-1,按下D为1.0
        float horizontalInput = 0.0F;
        float verticalInput = 0.0F;
        bool pressedW = false;
        bool pressedA = false;
        bool pressedS = false;
        bool pressedD = false;
        bool jump = false;
        int jumpBuffer = 0;
        bool mouseLeftDown = false;
        float dropTimer = 0.0F;
        bool dropPressed = false;
        bool isFlying = false;
        //The current item being held.
        //当前手持的物品。
        Item *item = nullptr;

        [[nodiscard]] uint32_t GetId() override;
    };
}
