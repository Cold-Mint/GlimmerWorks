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
#include "Item.h"

namespace glimmer {
    class ItemFilter {
        //tagList
        //tag列表
        std::vector<uint64_t> tagList_;
        //Is it allowed to put in items?
        //是否允许放入物品
        bool input_ = true;
        //Is it allowed to remove the items?
        //是否允许移出物品
        bool output_ = true;
        //Is it on the whiteTagList?
        //是否为白Tag名单
        bool isWhiteTagList_ = false;

    public:
        /**
         * AllowInput
         * 允许输入
         */
        void AllowInput();

        /**
         * AddTag
         * 添加Tag
         * @param tag
         */
        void AddTag(uint64_t tag);

        /**
         * RemoveTag
         * 移除Tag
         * @param tag
         */
        void RemoveTag(uint64_t tag);

        void SetIsWhiteTagList(bool isWhiteTagList);

        /**
         * AllowOutput
         * 允许输出
         */
        void AllowOutput();

        /**
         * DisableInput
         * 禁止输入
         */
        void DisableInput();

        /**
         * DisableOutput
         * 禁止输出
         */
        void DisableOutput();

        [[nodiscard]] bool IsWhiteTagList() const;

        /**
         * CanInput
         * 是否可以输入
         * @param item
         * @return
         */
        [[nodiscard]] bool CanInput(const Item *item) const;

        /**
         * CanOutput
         * 是否可以输出
         * @return
         */
        [[nodiscard]] bool CanOutput() const;
    };
}
