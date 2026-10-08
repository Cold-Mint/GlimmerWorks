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
#include "ComposableItemCreateParams.h"
#include "core/mod/Resource.h"

void glimmer::ComposableItemCreateParams::SetUnbreakable(const bool unbreakable)
{
    unbreakable_ = unbreakable;
}

void glimmer::ComposableItemCreateParams::SetTags(const std::vector<ItemTagResource>& tags)
{
    tags_ = tags;
}

void glimmer::ComposableItemCreateParams::SetResourceRef(const ResourceRef& resourceRef)
{
    resourceRef_ = resourceRef;
}

void glimmer::ComposableItemCreateParams::SetIconResourceRef(const ResourceRef& iconResourceRef)
{
    iconResultRef_ = iconResourceRef;
}

void glimmer::ComposableItemCreateParams::SetMaxDurability(const uint32_t maxDurability)
{
    maxDurability_ = maxDurability;
}

void glimmer::ComposableItemCreateParams::SetMaxSize(const uint8_t maxSize)
{
    maxSize_ = maxSize;
}

uint8_t glimmer::ComposableItemCreateParams::GetMaxSize() const
{
    return maxSize_;
}

void glimmer::ComposableItemCreateParams::SetIconResult(const std::shared_ptr<TextureResourceResult>& iconResult)
{
    iconResult_ = iconResult;
}

const std::shared_ptr<glimmer::TextureResourceResult>& glimmer::ComposableItemCreateParams::GetIconResult() const
{
    return iconResult_;
}

uint32_t glimmer::ComposableItemCreateParams::GetMaxDurability() const
{
    return maxDurability_;
}

void glimmer::ComposableItemCreateParams::SetDescription(const std::optional<std::string>& description)
{
    description_ = description;
}

void glimmer::ComposableItemCreateParams::SetName(std::string_view name)
{
    name_ = name;
}

void glimmer::ComposableItemCreateParams::SetId(std::string_view id)
{
    id_ = id;
}

const std::optional<std::string>& glimmer::ComposableItemCreateParams::GetDescription() const
{
    return description_;
}


bool glimmer::ComposableItemCreateParams::IsUnbreakable() const
{
    return unbreakable_;
}

const std::vector<glimmer::ItemTagResource>& glimmer::ComposableItemCreateParams::GetTags() const
{
    return tags_;
}

const glimmer::ResourceRef& glimmer::ComposableItemCreateParams::GetResourceRef() const
{
    return resourceRef_;
}

const glimmer::ResourceRef& glimmer::ComposableItemCreateParams::GetIconResourceRef() const
{
    return iconResultRef_;
}

const std::string& glimmer::ComposableItemCreateParams::GetName() const
{
    return name_;
}

const std::string& glimmer::ComposableItemCreateParams::GetId() const
{
    return id_;
}
