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
#include "ItemUseContext.h"

void glimmer::ItemUseContext::SetMouseLeftDown(const bool mouseLeftDown) {
    if (mouseLeftDown_ == mouseLeftDown) {
        if (mouseLeftDown_) {
            //It is regarded as a repeated press if the value of the left mouse button press this time is the same as the previous one.
            //和上次鼠标左键按下的值一致，被看作重复按下中。
            firstMouseLeftDown_ = false;
        }
        return;
    }
    mouseLeftDown_ = mouseLeftDown;
    //If mouseLeftDown is true, it is regarded as the first press; otherwise, it is considered as the release. The release sets firstMouseLeftDown to false.
    //如果mouseLeftDown == true，那么被看作首次按下，反之则被看作抬起，抬起设置firstMouseLeftDown == false。
    firstMouseLeftDown_ = mouseLeftDown;
}

bool glimmer::ItemUseContext::IsFirstMouseLeftDown() const {
    return firstMouseLeftDown_;
}

bool glimmer::ItemUseContext::IsFirstMouseRightDown() const {
    return firstMouseRightDown_;
}

bool glimmer::ItemUseContext::IsMouseLeftDown() const {
    return mouseLeftDown_;
}

void glimmer::ItemUseContext::SetMouseRightDown(const bool mouseRightDown) {
    if (mouseRightDown_ == mouseRightDown) {
        if (mouseRightDown_) {
            firstMouseRightDown_ = false;
        }
        return;
    }
    mouseRightDown_ = mouseRightDown;
    firstMouseRightDown_ = mouseRightDown;
}

bool glimmer::ItemUseContext::IsMouseRightDown() const {
    return mouseRightDown_;
}
