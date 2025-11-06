//
// Created by Cold-Mint on 2025/11/6.
//

#ifndef GLIMMERWORKS_RIGIDBODY2DCOMPONENT_H
#define GLIMMERWORKS_RIGIDBODY2DCOMPONENT_H
#include "../GameComponent.h"
#include "../../math/Vector2D.h"
#include "box2d/types.h"

namespace glimmer
{
    class RigidBody2DComponent : public GameComponent
    {
        b2BodyId bodyId_ = b2_nullBodyId;
        bool init = false;

    public:
        /**
         * Create Body
         * 创建刚体
         * @param worldId worldId 世界ID
         * @param worldVector2d worldVector2d 刚体位置
         */
        void CreateBody(b2WorldId worldId, WorldVector2D worldVector2d);
        /**
         * GetBodyId
         * 获取刚体ID
         * @return 刚体ID
         */
        [[nodiscard]] b2BodyId GetBodyId() const;
    };
}

#endif //GLIMMERWORKS_RIGIDBODY2DCOMPONENT_H
