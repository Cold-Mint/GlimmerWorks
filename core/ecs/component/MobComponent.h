//
// Created by Cold-Mint on 2026/3/10.
//

#ifndef GLIMMERWORKS_MOBCOMPONENT_H
#define GLIMMERWORKS_MOBCOMPONENT_H
#include <vector>

#include "core/ecs/GameComponent.h"
#include "core/ecs/GameEntity.h"

namespace glimmer {
    /**
     * MobComponent
     * 生物组件
     */
    class MobComponent : public GameComponent {
    public:
        //A list of physical IDs with radiation components for ground detection
        //用于地面检测的带有射线组件的实体ID列表
        std::vector<GameEntity::ID> groundCheckRayEntityIds;

        //Linear acceleration
        //移动加速度
        float movementAcceleration = 6.0F;
        float maxSpeed = 18.0F;
        // airControlFactor
        // 空中移动衰减
        // If it is set to 0, then the horizontal movement in the air will be prohibited. If it is set to 1, then there will be no restriction on the movement in the air. If it is set to 0.8, then the acceleration will be multiplied by 0.8.
        // 如果设置为0,那么禁止空中左右移动，如果设置为1,那么不限制空中移动，如果设置为0.8那么会将加速度乘以0.8。
        float airControlFactor = 1.0F;


        [[nodiscard]] uint32_t GetId() override;
    };
}

#endif //GLIMMERWORKS_MOBCOMPONENT_H
