//
// Created by Cold-Mint on 2025/10/29.
//

#ifndef GLIMMERWORKS_PLAYERCONTROLSYSTEM_H
#define GLIMMERWORKS_PLAYERCONTROLSYSTEM_H
#include "../GameSystem.h"
#include "core/ecs/component/PlayerComponent.h"

namespace glimmer {
    class RigidBody2DComponent;
    /**
     * The player control system processes the player's input control and realizes the WASD mobile camera function
     * 玩家控制系统，处理玩家的输入控制，实现WASD移动相机功能
     */
    class PlayerControlSystem final : public GameSystem {
        //Hand slip determination interval.
        //手滑判定间隔。
        float slipTimer = 0.0F;

        // 新增：地面检测射线参数
        const float GROUND_CHECK_DISTANCE = 0.1f;
        // 新增：移动缓动系数（优化手感）
        const float MOVE_ACCELERATION = 15.0f;
        const float MOVE_DECELERATION = 20.0f;

    public:
        explicit PlayerControlSystem(WorldContext *worldContext);

        void Update(float delta) override;

        /**
         * Check if the player is on the ground
         * 检查玩家是否在地面上
         * @return If the player is on the ground, return true; otherwise, return false 如果玩家在地面上则返回true，否则返回false
         */
        bool OnGround(const PlayerComponent* playerControlComponent) const;

        std::string GetName() override;

        bool HandleEvent(const SDL_Event &event) override;

    private:
        // 新增：平滑移动计算函数
        float CalculateSmoothMoveSpeed(float targetSpeed, float currentSpeed, float delta, bool isGrounded);
    };
}

#endif //GLIMMERWORKS_PLAYERCONTROLSYSTEM_H