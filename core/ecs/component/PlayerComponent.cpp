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

void glimmer::PlayerInputHandler::RemoveHorizontalInput(const float increment)
{
    horizontalInput_ -= increment;
}

void glimmer::PlayerInputHandler::AddHorizontalInput(const float increment)
{
    horizontalInput_ += increment;
}

void glimmer::PlayerInputHandler::ResetHorizontalInput()
{
    horizontalInput_ = 0.0F;
}

float glimmer::PlayerInputHandler::GetHorizontalInput() const
{
    return horizontalInput_;
}

void glimmer::PlayerInputHandler::RemoveVerticalInput(const float increment)
{
    verticalInput_ -= increment;
}

void glimmer::PlayerInputHandler::AddVerticalInput(const float increment)
{
    verticalInput_ += increment;
}

void glimmer::PlayerInputHandler::ResetVerticalInput()
{
    verticalInput_ = 0.0F;
}

void glimmer::PlayerInputHandler::SetVerticalInput(const float verticalInput)
{
    verticalInput_ = verticalInput;
}

float glimmer::PlayerInputHandler::GetVerticalInput() const
{
    return verticalInput_;
}

void glimmer::PlayerInputHandler::SetPressedW(const bool pressedW)
{
    pressedW_ = pressedW;
}

void glimmer::PlayerInputHandler::SetPressedA(const bool pressedA)
{
    pressedA_ = pressedA;
}

void glimmer::PlayerInputHandler::SetPressedS(const bool pressedS)
{
    pressedS_ = pressedS;
}

void glimmer::PlayerInputHandler::SetPressedD(const bool pressedD)
{
    pressedD_ = pressedD;
}

bool glimmer::PlayerInputHandler::IsPressedW() const
{
    return pressedW_;
}

bool glimmer::PlayerInputHandler::IsPressedA() const
{
    return pressedA_;
}

bool glimmer::PlayerInputHandler::IsPressedS() const
{
    return pressedS_;
}

bool glimmer::PlayerInputHandler::IsPressedD() const
{
    return pressedD_;
}

void glimmer::PlayerInputHandler::ResetDropTimer()
{
    dropTimer_ = 0;
}

void glimmer::PlayerInputHandler::AddDropTimer(const float delta)
{
    dropTimer_ += delta;
}

void glimmer::PlayerInputHandler::RemoveDropTimer(const float delta)
{
    dropTimer_ -= delta;
}

void glimmer::PlayerInputHandler::SetJump(const bool jump)
{
    jump_ = jump;
}

bool glimmer::PlayerInputHandler::IsJump() const
{
    return jump_;
}

void glimmer::PlayerInputHandler::SetJumpBuffer(const float jumpBuffer)
{
    jumpBuffer_ = jumpBuffer;
}

float glimmer::PlayerInputHandler::GetJumpBuffer() const
{
    return jumpBuffer_;
}

void glimmer::PlayerInputHandler::SetMouseLeftDown(const bool mouseLeftDown)
{
    mouseLeftDown_ = mouseLeftDown;
}

bool glimmer::PlayerInputHandler::IsMouseLeftDown() const
{
    return mouseLeftDown_;
}

float glimmer::PlayerInputHandler::GetDropTimer() const
{
    return dropTimer_;
}

void glimmer::PlayerInputHandler::SetDropPressed(const bool dropPressed)
{
    dropPressed_ = dropPressed;
}

bool glimmer::PlayerInputHandler::IsDropPressed() const
{
    return dropPressed_;
}

void glimmer::PlayerCapabilityHandler::SetFlying(const bool flying)
{
    isFlying_ = flying;
}

bool glimmer::PlayerCapabilityHandler::IsFlying() const
{
    return isFlying_;
}

void glimmer::PlayerCapabilityHandler::SetItem(Item* item)
{
    item_ = item;
}

glimmer::Item* glimmer::PlayerCapabilityHandler::GetItem() const
{
    return item_;
}

void glimmer::PlayerTechnologyHandler::ResetTechnologyMap()
{
    technologyMap_.clear();
    technologyMap_[RecipeGroup::None] = 1;
}

void glimmer::PlayerTechnologyHandler::SetTechnology(RecipeGroup recipeGroup, uint8_t technologyLevel)
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
            technologyMap_[recipeGroup] = technologyLevel;
        }
    }
}

const std::unordered_map<glimmer::RecipeGroup, uint8_t>& glimmer::PlayerTechnologyHandler::GetTechnologyMap() const
{
    return technologyMap_;
}

std::string glimmer::PlayerTechnologyHandler::ListTechnology(const std::string& technologyItem) const
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

void glimmer::PlayerComponent::RemoveHorizontalInput(const float increment)
{
    inputHandler_.RemoveHorizontalInput(increment);
}

void glimmer::PlayerComponent::AddHorizontalInput(const float increment)
{
    inputHandler_.AddHorizontalInput(increment);
}

void glimmer::PlayerComponent::ResetHorizontalInput()
{
    inputHandler_.ResetHorizontalInput();
}

float glimmer::PlayerComponent::GetHorizontalInput() const
{
    return inputHandler_.GetHorizontalInput();
}

void glimmer::PlayerComponent::RemoveVerticalInput(const float increment)
{
    inputHandler_.RemoveVerticalInput(increment);
}

void glimmer::PlayerComponent::AddVerticalInput(const float increment)
{
    inputHandler_.AddVerticalInput(increment);
}

void glimmer::PlayerComponent::ResetVerticalInput()
{
    inputHandler_.ResetVerticalInput();
}

void glimmer::PlayerComponent::SetVerticalInput(const float verticalInput)
{
    inputHandler_.SetVerticalInput(verticalInput);
}

float glimmer::PlayerComponent::GetVerticalInput() const
{
    return inputHandler_.GetVerticalInput();
}

void glimmer::PlayerComponent::SetPressedW(const bool pressedW)
{
    inputHandler_.SetPressedW(pressedW);
}

void glimmer::PlayerComponent::SetPressedA(const bool pressedA)
{
    inputHandler_.SetPressedA(pressedA);
}

void glimmer::PlayerComponent::SetPressedS(const bool pressedS)
{
    inputHandler_.SetPressedS(pressedS);
}

void glimmer::PlayerComponent::SetPressedD(const bool pressedD)
{
    inputHandler_.SetPressedD(pressedD);
}

bool glimmer::PlayerComponent::IsPressedW() const
{
    return inputHandler_.IsPressedW();
}

bool glimmer::PlayerComponent::IsPressedA() const
{
    return inputHandler_.IsPressedA();
}

bool glimmer::PlayerComponent::IsPressedS() const
{
    return inputHandler_.IsPressedS();
}

bool glimmer::PlayerComponent::IsPressedD() const
{
    return inputHandler_.IsPressedD();
}

void glimmer::PlayerComponent::ResetDropTimer()
{
    inputHandler_.ResetDropTimer();
}

void glimmer::PlayerComponent::AddDropTimer(const float delta)
{
    inputHandler_.AddDropTimer(delta);
}

void glimmer::PlayerComponent::RemoveDropTimer(const float delta)
{
    inputHandler_.RemoveDropTimer(delta);
}

void glimmer::PlayerComponent::SetJump(const bool jump)
{
    inputHandler_.SetJump(jump);
}

bool glimmer::PlayerComponent::IsJump() const
{
    return inputHandler_.IsJump();
}

void glimmer::PlayerComponent::SetJumpBuffer(const float jumpBuffer)
{
    inputHandler_.SetJumpBuffer(jumpBuffer);
}

float glimmer::PlayerComponent::GetJumpBuffer() const
{
    return inputHandler_.GetJumpBuffer();
}

void glimmer::PlayerComponent::SetMouseLeftDown(const bool mouseLeftDown)
{
    inputHandler_.SetMouseLeftDown(mouseLeftDown);
}

bool glimmer::PlayerComponent::IsMouseLeftDown() const
{
    return inputHandler_.IsMouseLeftDown();
}

float glimmer::PlayerComponent::GetDropTimer() const
{
    return inputHandler_.GetDropTimer();
}

void glimmer::PlayerComponent::SetDropPressed(const bool dropPressed)
{
    inputHandler_.SetDropPressed(dropPressed);
}

bool glimmer::PlayerComponent::IsDropPressed() const
{
    return inputHandler_.IsDropPressed();
}

void glimmer::PlayerComponent::SetFlying(const bool flying)
{
    capabilityHandler_.SetFlying(flying);
}

bool glimmer::PlayerComponent::IsFlying() const
{
    return capabilityHandler_.IsFlying();
}

void glimmer::PlayerComponent::SetItem(Item* item)
{
    capabilityHandler_.SetItem(item);
}

glimmer::Item* glimmer::PlayerComponent::GetItem() const
{
    return capabilityHandler_.GetItem();
}

void glimmer::PlayerComponent::ResetTechnologyMap()
{
    technologyHandler_.ResetTechnologyMap();
}

void glimmer::PlayerComponent::SetTechnology(RecipeGroup recipeGroup, uint8_t technologyLevel)
{
    technologyHandler_.SetTechnology(recipeGroup, technologyLevel);
}

const std::unordered_map<glimmer::RecipeGroup, uint8_t>& glimmer::PlayerComponent::GetTechnologyMap() const
{
    return technologyHandler_.GetTechnologyMap();
}

std::string glimmer::PlayerComponent::ListTechnology(const std::string& technologyItem) const
{
    return technologyHandler_.ListTechnology(technologyItem);
}

GameComponentTypeMessage glimmer::PlayerComponent::GetComponentTypeStatic()
{
    return COMPONENT_PLAYER;
}

GameComponentTypeMessage glimmer::PlayerComponent::GetComponentType()
{
    return GetComponentTypeStatic();
}
