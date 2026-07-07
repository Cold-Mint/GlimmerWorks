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

#include "MobComponent.h"
#include "core/inventory/Item.h"
#include "core/mod/dataPack/RecipeGroup.h"
#include "src/core/game_component_type.pb.h"

namespace glimmer
{
    class PlayerInputHandler
    {
        float horizontalInput_ = 0.0F;
        float verticalInput_ = 0.0F;
        bool pressedW_ = false;
        bool pressedA_ = false;
        bool pressedS_ = false;
        bool pressedD_ = false;
        bool jump_ = false;
        int jumpBuffer_ = 0;
        bool mouseLeftDown_ = false;
        float dropTimer_ = 0.0F;
        bool dropPressed_ = false;

    public:
        void RemoveHorizontalInput(float increment);
        void AddHorizontalInput(float increment);
        void ResetHorizontalInput();
        [[nodiscard]] float GetHorizontalInput() const;

        void RemoveVerticalInput(float increment);
        void AddVerticalInput(float increment);
        void ResetVerticalInput();
        void SetVerticalInput(float verticalInput);
        [[nodiscard]] float GetVerticalInput() const;

        void SetPressedW(bool pressedW);
        void SetPressedA(bool pressedA);
        void SetPressedS(bool pressedS);
        void SetPressedD(bool pressedD);
        [[nodiscard]] bool IsPressedW() const;
        [[nodiscard]] bool IsPressedA() const;
        [[nodiscard]] bool IsPressedS() const;
        [[nodiscard]] bool IsPressedD() const;

        void SetJump(bool jump);
        [[nodiscard]] bool IsJump() const;
        void SetJumpBuffer(float jumpBuffer);
        [[nodiscard]] float GetJumpBuffer() const;

        void SetMouseLeftDown(bool mouseLeftDown);
        [[nodiscard]] bool IsMouseLeftDown() const;

        void ResetDropTimer();
        void AddDropTimer(float delta);
        void RemoveDropTimer(float delta);
        [[nodiscard]] float GetDropTimer() const;
        void SetDropPressed(bool dropPressed);
        [[nodiscard]] bool IsDropPressed() const;
    };

    class PlayerCapabilityHandler
    {
        bool isFlying_ = false;
        Item* item_ = nullptr;

    public:
        void SetFlying(bool flying);
        [[nodiscard]] bool IsFlying() const;
        void SetItem(Item* item);
        [[nodiscard]] Item* GetItem() const;
    };

    class PlayerTechnologyHandler
    {
        std::unordered_map<RecipeGroup, uint8_t> technologyMap_ = {
            {RecipeGroup::None, 1}
        };

    public:
        void ResetTechnologyMap();
        void SetTechnology(RecipeGroup recipeGroup, uint8_t technologyLevel);
        [[nodiscard]] const std::unordered_map<RecipeGroup, uint8_t>& GetTechnologyMap() const;
        std::string ListTechnology(const std::string& technologyItem) const;
    };

    class PlayerComponent final : public MobComponent
    {
        PlayerInputHandler inputHandler_;
        PlayerCapabilityHandler capabilityHandler_;
        PlayerTechnologyHandler technologyHandler_;

    public:
        void RemoveHorizontalInput(float increment);
        void AddHorizontalInput(float increment);
        void ResetHorizontalInput();
        [[nodiscard]] float GetHorizontalInput() const;

        void RemoveVerticalInput(float increment);
        void AddVerticalInput(float increment);
        void ResetVerticalInput();
        void SetVerticalInput(float verticalInput);
        [[nodiscard]] float GetVerticalInput() const;

        void SetPressedW(bool pressedW);
        void SetPressedA(bool pressedA);
        void SetPressedS(bool pressedS);
        void SetPressedD(bool pressedD);
        [[nodiscard]] bool IsPressedW() const;
        [[nodiscard]] bool IsPressedA() const;
        [[nodiscard]] bool IsPressedS() const;
        [[nodiscard]] bool IsPressedD() const;

        void ResetDropTimer();
        void AddDropTimer(float delta);
        void RemoveDropTimer(float delta);
        void SetJump(bool jump);
        [[nodiscard]] bool IsJump() const;
        void SetJumpBuffer(float jumpBuffer);
        [[nodiscard]] float GetJumpBuffer() const;
        void SetMouseLeftDown(bool mouseLeftDown);
        [[nodiscard]] bool IsMouseLeftDown() const;
        [[nodiscard]] float GetDropTimer() const;
        void SetDropPressed(bool dropPressed);
        [[nodiscard]] bool IsDropPressed() const;

        void SetFlying(bool flying);
        [[nodiscard]] bool IsFlying() const;
        void SetItem(Item* item);
        [[nodiscard]] Item* GetItem() const;

        void ResetTechnologyMap();
        void SetTechnology(RecipeGroup recipeGroup, uint8_t technologyLevel);
        [[nodiscard]] const std::unordered_map<RecipeGroup, uint8_t>& GetTechnologyMap() const;
        std::string ListTechnology(const std::string& technologyItem) const;

        [[nodiscard]] static GameComponentTypeMessage GetComponentTypeStatic();
        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;
    };
}
