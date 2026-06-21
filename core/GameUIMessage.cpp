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
#include "GameUIMessage.h"

#include <utility>

#include "utils/StringUtils.h"

glimmer::GameUIMessage::GameUIMessage(ResourcePackManager* resourcePackManager, std::string text, const uint64_t now,
                                      const Color* color) : text_(std::move(text)),
                                                            createTime_(now),
                                                            expireTime_(now + 2500),
                                                            tween_(tweeny::from(0.0F)
                                                                   .to(1.0F).during(200)
                                                                   .to(1.0F).during(2000)
                                                                   .to(0.0F).during(300))
{
    texture_ = resourcePackManager->CreateStringTexture(text_, color);
}

uint64_t glimmer::GameUIMessage::GetCreateTime() const
{
    return createTime_;
}

void glimmer::GameUIMessage::SetAlpha(const float alpha)
{
    alpha_ = alpha;
}

std::string glimmer::GameUIMessage::GetText() const
{
    return text_;
}

float glimmer::GameUIMessage::GetAlpha() const
{
    return alpha_;
}

SDL_Texture* glimmer::GameUIMessage::GetTexture() const
{
    if (texture_ == nullptr)
    {
        return nullptr;
    }
    return texture_.get();
}

tweeny::tween<float>& glimmer::GameUIMessage::GetTween()
{
    return tween_;
}

uint64_t glimmer::GameUIMessage::GetExpireTime() const
{
    return expireTime_;
}

uint64_t glimmer::GameUIMessage::GetFingerprint() const
{
    return StringUtils::StringToUint64(text_);
}
