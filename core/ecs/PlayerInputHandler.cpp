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
#include "PlayerInputHandler.h"

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
