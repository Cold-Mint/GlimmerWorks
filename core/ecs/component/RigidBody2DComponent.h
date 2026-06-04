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
#include "core/ecs/GameComponent.h"
#include "box2d/types.h"
#include "core/Box2dFilter.h"
#include "core/math/WorldVector2D.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/ResourceRef.h"

namespace glimmer
{
    class RigidBody2DComponent : public GameComponent
    {
        b2BodyId bodyId_ = b2_nullBodyId;
        Box2dFilter filter_;
        bool ready_ = false;
        b2BodyType bodyType_ = b2_staticBody;
        bool allowBodySleep_ = true;
        bool fixedRotation_ = false;
        bool enabled_ = true;
        float friction_ = 0.0F;
        float restitution_ = 0.0F;
        //Density
        //密度
        float density_ = 0.01F;
        ResourceRef shapeRef_;

    public:
        ~RigidBody2DComponent() override;

        void SetFilter(Box2dFilter filter);

        void SetShapeRef(const ResourceRef& shapeRef);

        void Enable();

        void Disable();

        [[nodiscard]] bool IsEnabled() const;

        void SetDensity(float density);

        [[nodiscard]] float GetDensity() const;

        /**
         * SetFriction
         * 设置摩擦力
         * @param friction friction 摩擦力
         */
        void SetFriction(float friction);

        /**
         * SetRestitution
         * 设置摩擦返还系数
         * @param restitution
         */
        void SetRestitution(float restitution);


        /**
         * GetFriction
         * 获取摩擦力
         * @return
         */
        [[nodiscard]] float GetFriction() const;


        /**
         * Create Body
         * 创建刚体
         * @param resourceLocator
         * @param worldId worldId 世界ID
         * @param vector2d vector2d 位置
         */
        void CreateBody(const ResourceLocator* resourceLocator, b2WorldId worldId, WorldVector2D vector2d);

        /**
         * GetBodyId
         * 获取刚体ID
         * @return 刚体ID
         */
        [[nodiscard]] b2BodyId GetBodyId() const;

        /**
         * Is the body ready
         * 刚体是否就绪
         * @return
         */
        [[nodiscard]] bool IsReady() const;

        /**
         * SetBodyType
         * 设置刚体类型
         * @param bodyType 刚体类型
         */
        void SetBodyType(b2BodyType bodyType);

        /**
         * Set whether to allow sleep mode
         * 设置是否允许休眠
         * @param enable 是否启用休眠
         */
        void SetAllowBodySleep(bool enable);

        /**
         * Obtain the permission for sleep mode
         * 获取是否允许休眠
         * @return 是否启用休眠
         */
        [[nodiscard]] bool AllowBodySleep() const;

        /**
         * SetFixedRotation
         * 设置是否固定旋转
         * @param fixedRotation 是否固定旋转
         */
        void SetFixedRotation(bool fixedRotation);

        /**
         * GetFixedRotation
         * 获取是否固定旋转
         * @return 是否固定旋转
         */
        [[nodiscard]] bool GetFixedRotation() const;

        /**
         * Check if the body is a dynamic body.
         * 动态刚体可以被物理力影响，位置会根据力自动更新。
         * Dynamic bodies are affected by forces and their positions are automatically updated by the physics engine.
         * 动态物体受到力的影响，它们的位置由物理引擎自动更新。
         * @return true if the body is dynamic
         */
        [[nodiscard]] bool IsDynamicBody() const;

        /**
         * Check if the body is a kinematic body.
         * 运动学刚体不受力影响，但可以通过设置速度或位置移动。
         * Kinematic bodies are not affected by forces but can be moved by setting velocity or position.
         * 运动体不受力的影响，但可以通过设定速度或位置来移动。
         * @return true if the body is kinematic
         */
        [[nodiscard]] bool IsKinematicBody() const;

        /**
         * Check if the body is a static body.
         * 静态刚体不受力影响，位置只能手动设置，通常用于地面或墙壁。
         * Static bodies are unaffected by forces and can only be moved manually, typically used for ground or walls.
         * 静态物体不受外力影响，只能手动移动，通常用于地面或墙壁。
         * @return true if the body is static
         */
        [[nodiscard]] bool IsStaticBody() const;

        [[nodiscard]] static GameComponentTypeMessage GetComponentTypeStatic();

        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;
    };
}
