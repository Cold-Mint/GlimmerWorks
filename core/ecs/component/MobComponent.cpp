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
#include "MobComponent.h"

const std::vector<GameEntityID>& glimmer::MobComponent::GetGroundCheckRayEntityIds() const
{
    return groundCheckRayEntityIds_;
}

void glimmer::MobComponent::AddGroundCheckRayEntity(GameEntityID id)
{
    groundCheckRayEntityIds_.emplace_back(id);
}

void glimmer::MobComponent::SetMovementAcceleration(const float acceleration)
{
    movementAcceleration_ = acceleration;
}

float glimmer::MobComponent::GetMovementAcceleration() const
{
    return movementAcceleration_;
}

void glimmer::MobComponent::SetMaxSpeed(const float speed)
{
    maxSpeed_ = speed;
}

float glimmer::MobComponent::GetMaxSpeed() const
{
    return maxSpeed_;
}

void glimmer::MobComponent::SetAirControlFactor(const float factor)
{
    airControlFactor_ = factor;
}

float glimmer::MobComponent::GetAirControlFactor() const
{
    return airControlFactor_;
}

void glimmer::MobComponent::SetJumpForce(const float force)
{
    jumpForce_ = force;
}

float glimmer::MobComponent::GetJumpForce() const
{
    return jumpForce_;
}

void glimmer::MobComponent::SetFacingLeft(const bool facing)
{
    facingLeft_ = facing;
}

bool glimmer::MobComponent::GetFacingLeft() const
{
    return facingLeft_;
}

GameComponentTypeMessage glimmer::MobComponent::GetComponentTypeStatic()
{
    return COMPONENT_MOB;
}

GameComponentTypeMessage glimmer::MobComponent::GetComponentType()
{
    return GetComponentTypeStatic();
}
