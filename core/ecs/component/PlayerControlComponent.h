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
        // 是否启用WASD移动
        bool enableWASD = true;
        
        // 上移按键状态
        bool moveUp = false;
        // 下移按键状态
        bool moveDown = false;
        // 左移按键状态
        bool moveLeft = false;
        // 右移按键状态
        bool moveRight = false;
        
        /**
         * 重置所有移动按键状态
         */
        void ResetMovement() {
            moveUp = false;
            moveDown = false;
            moveLeft = false;
            moveRight = false;
        }
        
        /**
         * 检查是否有任何移动输入
         * @return bool 是否有移动输入
         */
        [[nodiscard]] bool HasMovementInput() const {
            return moveUp || moveDown || moveLeft || moveRight;
        }
    };
} 

#endif //GLIMMERWORKS_PLAYERCONTROLCOMPONENT_H