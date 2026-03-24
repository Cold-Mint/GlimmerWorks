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

        //Movement speed, unit (tile)
        //移动速度，单位（格数)
        float movementSpeed = 0.5F;

        [[nodiscard]] uint32_t GetId() override;
    };
}

#endif //GLIMMERWORKS_MOBCOMPONENT_H
