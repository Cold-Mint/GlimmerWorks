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
#include <memory>
#include <string>

#include "SDL3/SDL_render.h"

namespace glimmer
{
    class TagRuntimeData
    {
        /**
         * text
         * 显示的文本
         */
        std::string text_;
        /**
         * Required weight
         * 需要的权重
         * Expected value
         * 预期值
         */
        uint16_t requiredWeight_ = 1;
        /**
         * Actual value
         * 实际值
         */
        uint16_t actualValue_ = 0;

        /**
         * Exact Match
         * 精准匹配
         */
        bool exactMatch_ = true;

        std::shared_ptr<SDL_Texture> positiveTexture_ = nullptr;
        std::shared_ptr<SDL_Texture> negativeTexture_ = nullptr;

    public:
        [[nodiscard]] uint16_t GetRequiredWeight() const;

        void SetPositiveTexture(const std::shared_ptr<SDL_Texture>& positiveTexture);

        void SetNegativeTexture(const std::shared_ptr<SDL_Texture>& negativeTexture);

        void SetActualValue(uint16_t actualValue);

        void AddActualValue(uint16_t actualValue);

        bool IsExactMatch() const;

        void SetExactMatch(bool exactMatch);

        bool Matched() const;

        [[nodiscard]] uint16_t GetActualValue() const;

        SDL_Texture* GetPositiveTexture() const;

        SDL_Texture* GetNegativeTexture() const;

        void SetRequiredWeight(uint16_t requiredWeight);

        void SetText(const std::string& text);

        [[nodiscard]] const std::string& GetText() const;
    };
}
