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
        float slipTimer_ = 0.0F;
        /**
        * Check if the player is on the ground
        * 检查玩家是否在地面上
        * @return If the player is on the ground, return true; otherwise, return false 如果玩家在地面上则返回true，否则返回false
        */
        bool OnGround(const PlayerComponent *playerControlComponent) const;

    public:
        explicit PlayerControlSystem(WorldContext *worldContext);

        void Update(float delta) override;


        std::string GetName() override;

        bool HandleEvent(const SDL_Event &event) override;
    };
}

#endif //GLIMMERWORKS_PLAYERCONTROLSYSTEM_H
