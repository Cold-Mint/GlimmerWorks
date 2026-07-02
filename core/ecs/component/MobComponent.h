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
#include <vector>

#include "core/ecs/EcsTypes.h"
#include "core/ecs/GameComponent.h"
#include "core/inventory/Item.h"

namespace glimmer
{
    /**
     * MobComponent
     * 生物组件
     */
    class MobComponent : public GameComponent
    {
        //A list of physical IDs with radiation components for ground detection
        //用于地面检测的带有射线组件的实体ID列表
        std::vector<GameEntityID> groundCheckRayEntityIds_;

        //Linear acceleration
        //移动加速度
        float movementAcceleration_ = 6.0F;
        float maxSpeed_ = 18.0F;
        // airControlFactor
        // 空中移动衰减
        // If it is set to 0, then the horizontal movement in the air will be prohibited. If it is set to 1, then there will be no restriction on the movement in the air. If it is set to 0.8, then the acceleration will be multiplied by 0.8.
        // 如果设置为0,那么禁止空中左右移动，如果设置为1,那么不限制空中移动，如果设置为0.8那么会将加速度乘以0.8。
        float airControlFactor_ = 1.0F;

        float jumpForce_ = 7.5F;

        /**
         * Is it facing to the left?
         * 是否为面向左侧
         */
        bool facingLeft_ = false;

        std::unique_ptr<Item> emptyHandAutoUseItem_ = nullptr;

    public:
        [[nodiscard]] const std::vector<GameEntityID>& GetGroundCheckRayEntityIds() const;

        void SetEmptyHandAutoUseItem(std::unique_ptr<Item> emptyHandAutoUseItem);

        [[nodiscard]] Item* GetEmptyHandAutoUseItem() const;

        void AddGroundCheckRayEntity(GameEntityID id);

        void SetMovementAcceleration(float acceleration);

        [[nodiscard]] float GetMovementAcceleration() const;

        void SetMaxSpeed(float speed);

        [[nodiscard]] float GetMaxSpeed() const;

        void SetAirControlFactor(float factor);

        [[nodiscard]] float GetAirControlFactor() const;

        void SetJumpForce(float force);

        [[nodiscard]] float GetJumpForce() const;

        void SetFacingLeft(bool facing);

        [[nodiscard]] bool GetFacingLeft() const;

        [[nodiscard]] static GameComponentTypeMessage GetComponentTypeStatic();

        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;
    };
}
