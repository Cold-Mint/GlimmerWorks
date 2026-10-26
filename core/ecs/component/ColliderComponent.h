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
#include "core/math/WorldAABB.h"
#include "core/math/WorldVector2D.h"

namespace glimmer {
    /**
     * Physical collision body component
     * 物理碰撞体组件
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
        //Gravity scale of the collider affects gravitational pull. A value of 0 disables gravity; negative values produce anti‑gravity floatation; positive values result in downward falling.
        //碰撞体的重力缩放，影响重力。（0为不启用重力，负数为反重力漂浮，正数为下落。）
        float gravityScale_ = 1.0F;

    public:
        /**
         * Set the half extents of the AABB (pixels).
         * 设置 AABB 的半宽/半高（像素）。
         */
        void SetHalfExtents(float halfWidth, float halfHeight);

        void SetGravityScale(float gravityScale);

        [[nodiscard]] float GetGravityScale() const;

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
         * Build an AABB from a center position and this collider's half extents.
         * 用中心位置与本碰撞体的半宽半高组装一个 AABB。
         * @param center center 盒子中心（世界坐标）。
         * @return The AABB. AABB。
         */
        [[nodiscard]] WorldAABB GetAABB(const WorldVector2D &center) const;

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
