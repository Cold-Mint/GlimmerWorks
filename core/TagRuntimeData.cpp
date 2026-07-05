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
#include "TagRuntimeData.h"

uint16_t glimmer::TagRuntimeData::GetRequiredWeight() const
{
    return requiredWeight_;
}

void glimmer::TagRuntimeData::SetPositiveTexture(const std::shared_ptr<SDL_Texture>& positiveTexture)
{
    positiveTexture_ = positiveTexture;
}

void glimmer::TagRuntimeData::SetNegativeTexture(const std::shared_ptr<SDL_Texture>& negativeTexture)
{
    negativeTexture_ = negativeTexture;
}

void glimmer::TagRuntimeData::SetActualValue(const uint16_t actualValue)
{
    actualValue_ = actualValue;
}

void glimmer::TagRuntimeData::AddActualValue(const uint16_t actualValue)
{
    actualValue_ += actualValue;
}

bool glimmer::TagRuntimeData::IsExactMatch() const
{
    return exactMatch_;
}

void glimmer::TagRuntimeData::SetExactMatch(bool exactMatch)
{
    exactMatch_ = exactMatch;
}

bool glimmer::TagRuntimeData::Matched() const
{
    if (exactMatch_)
    {
        return actualValue_ == requiredWeight_;
    }
    return actualValue_ >= requiredWeight_;
}

uint16_t glimmer::TagRuntimeData::GetActualValue() const
{
    return actualValue_;
}

SDL_Texture* glimmer::TagRuntimeData::GetPositiveTexture() const
{
    if (positiveTexture_ == nullptr)
    {
        return nullptr;
    }
    return positiveTexture_.get();
}

SDL_Texture* glimmer::TagRuntimeData::GetNegativeTexture() const
{
    if (negativeTexture_ == nullptr)
    {
        return nullptr;
    }
    return negativeTexture_.get();
}

void glimmer::TagRuntimeData::SetRequiredWeight(const uint16_t requiredWeight)
{
    requiredWeight_ = requiredWeight;
}

void glimmer::TagRuntimeData::SetText(const std::string_view text)
{
    text_ = text;
}

const std::string& glimmer::TagRuntimeData::GetText() const
{
    return text_;
}
