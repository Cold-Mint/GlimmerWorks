/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#pragma once
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

        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;
    };
}
