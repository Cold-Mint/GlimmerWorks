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

#include <string>
#include <vector>
#include <optional>

#include "core/mod/ResourceRef.h"
#include "core/BoolOrToggle.h"

namespace glimmer
{
    class CommandArgs
    {
        std::vector<std::string> tokens_;
        std::string command_;

    public:
        explicit CommandArgs(std::string_view command);

        /**
         * Obtain the Token position corresponding to the cursor position
         * 获取光标位置对应的Token位置
         * @param cursorPos cursorPos 光标位置
         * @return
         */
        [[nodiscard]] int GetTokenIndexAtCursor(int cursorPos) const;

        /**
         * Get the keyword corresponding to the cursor position
         * 获取光标位置对应的关键字
         * @param cursorPos cursorPos 光标位置
         * @return
         */
        [[nodiscard]] std::string GetKeywordAtCursor(int cursorPos) const;

        [[nodiscard]] int GetSize() const;

        [[nodiscard]] bool AsBool(int index) const;

        /**
         *
         * 转为bool值或者toggle
         * @param index
         * @return
         */
        [[nodiscard]] BoolOrToggle AsBoolOrToggle(int index) const;

        [[nodiscard]] int AsInt(int index) const;

        /**
         * Convert to coordinates
         * 转换为坐标
         * @param index index 位置
         * @param origin origin(Used to replace the tilde symbol ~) 原点坐标(用于替代~号)
         * @return  Return to world coordinates 返回世界坐标
         */
        [[nodiscard]] float AsCoordinate(int index, float origin) const;

        [[nodiscard]] float AsFloat(int index) const;

        [[nodiscard]] double AsDouble(int index) const;

        [[nodiscard]] std::string AsString(int index) const;

        [[nodiscard]] std::optional<ResourceRef> AsResourceRef(int index, ResourceTypeMessage resourceType) const;
    };
}
