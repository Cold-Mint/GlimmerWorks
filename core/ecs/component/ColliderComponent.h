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
#include "core/math/WorldVector2D.h"

namespace glimmer {
    /**
     * Axis-aligned bounding box collider used by the custom tile-grid physics.
     * This replaces the Box2D rigid body. The collider stores a half-extent box (in pixels), a velocity
     * (pixels/second) and a grounded flag that is refreshed by the physics system every tick.
     * 自定义瓦片网格物理使用的轴对齐包围盒碰撞体。它取代了 Box2D 刚体。
     * 碰撞体保存一个半宽/半高包围盒（像素）、速度（像素/秒）以及由物理系统每 tick 刷新的落地标记。
     */
    class ColliderComponent : public GameComponent {
        //Half extents of the AABB, in pixels.
        //AABB 的半宽/半高，单位像素。
        float halfWidth_ = 8.0F;
        float halfHeight_ = 8.0F;
        //Velocity in pixels/second.
        //速度（像素/秒）。
        WorldVector2D velocity_;
        //Whether the entity is standing on a solid tile. Refreshed by the physics system.
        //实体是否站在实心瓦片上。由物理系统刷新。
        bool grounded_ = false;
        //Whether the collider participates in physics.
        //碰撞体是否参与物理。
        bool enabled_ = false;

    public:
        /**
         * Set the half extents of the AABB (pixels).
         * 设置 AABB 的半宽/半高（像素）。
         */
        void SetHalfExtents(float halfWidth, float halfHeight);

        /**
         * Get the half width of the AABB (pixels).
         * 获取 AABB 半宽（像素）。
         */
        [[nodiscard]] float GetHalfWidth() const;

        /**
         * Get the half height of the AABB (pixels).
         * 获取 AABB 半高（像素）。
         */
        [[nodiscard]] float GetHalfHeight() const;

        /**
         * Set the velocity (pixels/second).
         * 设置速度（像素/秒）。
         */
        void SetVelocity(const WorldVector2D &velocity);

        /**
         * Get the velocity (pixels/second).
         * 获取速度（像素/秒）。
         */
        [[nodiscard]] const WorldVector2D &GetVelocity() const;

        /**
         * Set whether the entity is grounded.
         * 设置实体是否落地。
         */
        void SetGrounded(bool grounded);

        /**
         * Whether the entity is grounded.
         * 实体是否落地。
         */
        [[nodiscard]] bool IsGrounded() const;

        /**
         * Enable the collider.
         * 启用碰撞体。
         */
        void Enable();

        /**
         * Disable the collider.
         * 禁用碰撞体。
         */
        void Disable();

        /**
         * Whether the collider is enabled.
         * 碰撞体是否启用。
         */
        [[nodiscard]] bool IsEnabled() const;

        [[nodiscard]] static GameComponentTypeMessage GetComponentTypeStatic();

        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;
    };
}
