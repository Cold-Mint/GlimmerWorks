//
// Created by Cold-Mint on 2026/3/8.
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
    class RayCast2DComponent : public GameComponent {
        WorldVector2D origin_;
        WorldVector2D translation_;
        b2QueryFilter filter_ = {};
        bool hit_ = false;
        //hitPoint
        //碰撞点
        WorldVector2D hitPoint_;
        //hitNormal
        //碰撞法线
        WorldVector2D hitNormal_;
        //Collision Shape ID
        //碰撞形状ID
        b2ShapeId hitShapeId_ = b2_nullShapeId;

        GameEntity::ID transform2DEntity_ = GAME_ENTITY_ID_INVALID;

    public:
        [[nodiscard]] const WorldVector2D &GetOrigin() const;

        [[nodiscard]] const WorldVector2D &GetTranslation() const;

        void SetTransform2DEntity(GameEntity::ID id);

        [[nodiscard]] GameEntity::ID GetTransform2DEntity() const;

        void SetTransform2D(const WorldVector2D &transform2D);

        void SetOrigin(const WorldVector2D &origin);

        [[nodiscard]] const b2QueryFilter &GetFilter() const;

        void SetFilter(const b2QueryFilter &filter);

        void SetTransform(const WorldVector2D &transform);

        [[nodiscard]] const WorldVector2D &GetHitPoint() const;

        void SetHitPoint(const WorldVector2D &hitPoint);

        [[nodiscard]] const b2ShapeId &GetHitShape() const;

        void SetHitShape(const b2ShapeId &hitShapeId);

        [[nodiscard]] const WorldVector2D &GetHitNormal() const;

        void SetHitNormal(const WorldVector2D &hitNormal);

        [[nodiscard]] bool IsHit() const;

        void SetHit(bool hit);

        [[nodiscard]] uint32_t GetId() override;
    };
}

#endif //GLIMMERWORKS_RAYCAST2DCOMPONENT_H
