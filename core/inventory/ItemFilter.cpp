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
#include "ItemFilter.h"

void glimmer::ItemFilter::AllowInput() {
    input_ = true;
}

void glimmer::ItemFilter::AddTag(const uint64_t tag) {
    if (std::ranges::find(tagList_, tag) == tagList_.end()) {
        tagList_.push_back(tag);
    }
}

void glimmer::ItemFilter::RemoveTag(uint64_t tag) {
    std::erase(tagList_, tag);
}

void glimmer::ItemFilter::SetIsWhiteTagList(const bool isWhiteTagList) {
    isWhiteTagList_ = isWhiteTagList;
}

void glimmer::ItemFilter::AllowOutput() {
    output_ = true;
}

void glimmer::ItemFilter::DisableInput() {
    input_ = false;
}

void glimmer::ItemFilter::DisableOutput() {
    output_ = false;
}

bool glimmer::ItemFilter::IsWhiteTagList() const {
    return isWhiteTagList_;
}

bool glimmer::ItemFilter::CanInput(const Item *item) const {
    //If the item is null, it indicates that the slot is cleared and is always allowed.
    //物品为null表示清空槽位，永远允许。
    if (item == nullptr) {
        return true;
    }
    if (!input_) {
        return false;
    }
    if (const size_t tagListSize = tagList_.size();
        tagListSize > 0) {
        const ItemTagModule *itemTagModule = item->GetTagModule();
        if (itemTagModule == nullptr) {
            return false;
        }

        if (isWhiteTagList_) {
            bool matched = false;
            for (const uint64_t tag: tagList_) {
                //Match any one of the items in the whitelist.
                //匹配白名单内的任意一个。
                if (itemTagModule->HasTag(tag)) {
                    matched = true;
                    break;
                }
            }
            if (!matched) {
                return false;
            }
        } else {
            for (const uint64_t tag: tagList_) {
                //The tag of the new item contains the tags from the blacklist.
                //新物品的tag内包含黑名单内的tag。
                if (itemTagModule->HasTag(tag)) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool glimmer::ItemFilter::CanOutput() const {
    return output_;
}
