//
// Created by coldmint on 2026/3/8.
//

#ifndef GLIMMERWORKS_RAYCAST2DCOMPONENT_H
#define GLIMMERWORKS_RAYCAST2DCOMPONENT_H
#include "box2d/id.h"
#include "core/Box2dFilter.h"
#include "core/Constants.h"
#include "core/ecs/GameComponent.h"
#include "core/ecs/GameEntity.h"
#include "core/math/Vector2D.h"

namespace glimmer {
    /**
     * RayCast2D component
     * 射线组件
     */
    struct RayCast2DComponent : GameComponent {
        WorldVector2D origin;
        WorldVector2D translation;
        Box2dFilter filter;
        bool hit = false;
        WorldVector2D hitPoint;
        WorldVector2D hitNormal;
        b2ShapeId hitShapeId;
        GameEntity::ID hitEntity = GAME_ENTITY_ID_INVALID;
        //Detect once for each frame
        //每帧检测一次
        bool enableContinuous = true;
        GameEntity::ID transform2DEntity = GAME_ENTITY_ID_INVALID;

        [[nodiscard]] uint32_t GetId() override;


    };
}

#endif //GLIMMERWORKS_RAYCAST2DCOMPONENT_H
