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
#include "PlayerComponent.h"
#include <utility>
#include "fmt/xchar.h"


void glimmer::PlayerComponent::RemoveHorizontalInput(const float increment)
{
    horizontalInput_ -= increment;
}

void glimmer::PlayerComponent::AddHorizontalInput(const float increment)
{
    horizontalInput_ += increment;
}

void glimmer::PlayerComponent::ResetHorizontalInput()
{
    horizontalInput_ = 0.0F;
}

void glimmer::PlayerComponent::RemoveVerticalInput(const float increment)
{
    verticalInput_ -= increment;
}

void glimmer::PlayerComponent::AddVerticalInput(const float increment)
{
    verticalInput_ += increment;
}

void glimmer::PlayerComponent::ResetVerticalInput()
{
    verticalInput_ = 0.0F;
}


float glimmer::PlayerComponent::GetHorizontalInput() const
{
    return horizontalInput_;
}

void glimmer::PlayerComponent::SetVerticalInput(const float verticalInput)
{
    verticalInput_ = verticalInput;
}

float glimmer::PlayerComponent::GetVerticalInput() const
{
    return verticalInput_;
}

void glimmer::PlayerComponent::SetPressedW(const bool pressedW)
{
    pressedW_ = pressedW;
}

void glimmer::PlayerComponent::SetPressedA(const bool pressedA)
{
    pressedA_ = pressedA;
}

void glimmer::PlayerComponent::SetPressedS(const bool pressedS)
{
    pressedS_ = pressedS;
}

void glimmer::PlayerComponent::SetPressedD(const bool pressedD)
{
    pressedD_ = pressedD;
}

bool glimmer::PlayerComponent::IsPressedW() const
{
    return pressedW_;
}

bool glimmer::PlayerComponent::IsPressedA() const
{
    return pressedA_;
}

bool glimmer::PlayerComponent::IsPressedS() const
{
    return pressedS_;
}

bool glimmer::PlayerComponent::IsPressedD() const
{
    return pressedD_;
}

void glimmer::PlayerComponent::ResetDropTimer()
{
    dropTimer_ = 0;
}

void glimmer::PlayerComponent::AddDropTimer(const float delta)
{
    dropTimer_ += delta;
}

void glimmer::PlayerComponent::RemoveDropTimer(const float delta)
{
    dropTimer_ -= delta;
}

void glimmer::PlayerComponent::SetJump(const bool jump)
{
    jump_ = jump;
}

bool glimmer::PlayerComponent::IsJump() const
{
    return jump_;
}

void glimmer::PlayerComponent::SetJumpBuffer(const float jumpBuffer)
{
    jumpBuffer_ = jumpBuffer;
}

float glimmer::PlayerComponent::GetJumpBuffer() const
{
    return jumpBuffer_;
}

void glimmer::PlayerComponent::SetMouseLeftDown(const bool mouseLeftDown)
{
    mouseLeftDown_ = mouseLeftDown;
}

bool glimmer::PlayerComponent::IsMouseLeftDown() const
{
    return mouseLeftDown_;
}

float glimmer::PlayerComponent::GetDropTimer() const
{
    return dropTimer_;
}

void glimmer::PlayerComponent::SetDropPressed(const bool dropPressed)
{
    dropPressed_ = dropPressed;
}

bool glimmer::PlayerComponent::IsDropPressed() const
{
    return dropPressed_;
}

void glimmer::PlayerComponent::SetFlying(const bool flying)
{
    isFlying_ = flying;
}

bool glimmer::PlayerComponent::IsFlying() const
{
    return isFlying_;
}

void glimmer::PlayerComponent::SetItem(Item* item)
{
    item_ = item;
}

glimmer::Item* glimmer::PlayerComponent::GetItem() const
{
    return item_;
}

void glimmer::PlayerComponent::ResetTechnologyMap()
{
    technologyMap_.clear();
    technologyMap_[RecipeGroup::None] = 1;
}

void glimmer::PlayerComponent::SetTechnology(RecipeGroup recipeGroup, uint8_t technologyLevel)
{
    auto iterator = technologyMap_.find(recipeGroup);
    if (iterator == technologyMap_.end())
    {
        technologyMap_[recipeGroup] = technologyLevel;
    }
    else
    {
        if (technologyLevel > iterator->second)
        {
            //If the current technological level set is higher than the previous one.
            //如果当前设置的科技等级大于上次设置的科技等级。
            technologyMap_[recipeGroup] = technologyLevel;
        }
    }
}

const std::unordered_map<glimmer::RecipeGroup, uint8_t>& glimmer::PlayerComponent::GetTechnologyMap() const
{
    return technologyMap_;
}

std::string glimmer::PlayerComponent::ListTechnology(const std::string& technologyItem) const
{
    std::stringstream ss;
    for (auto& [recipeGroup, techLevel] : technologyMap_)
    {
        ss << fmt::format(
            fmt::runtime(technologyItem),
            std::to_underlying(recipeGroup), static_cast<int>(techLevel));
        ss << '\n';
    }
    return ss.str();
}

GameComponentTypeMessage glimmer::PlayerComponent::GetComponentTypeStatic()
{
    return COMPONENT_PLAYER;
}

GameComponentTypeMessage glimmer::PlayerComponent::GetComponentType()
{
    return GetComponentTypeStatic();
}
