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
#include "CraftPreviewSlotComponent.h"

#include "core/mod/ResourceLocator.h"
#include "core/world/WorldContext.h"


void glimmer::CraftPreviewSlotComponent::SetRecipeResource(WorldContext* worldContext, RecipeResource* recipeResource)
{
    recipeResource_ = recipeResource;
    const AppContext* appContext = worldContext->GetAppContext();
    if (appContext == nullptr)
    {
        return;
    }
    ResourceLocator* resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator == nullptr)
    {
        return;
    }
    std::unique_ptr<Item> item = resourceLocator->FindItem(worldContext, recipeResource->output);
    if (item == nullptr)
    {
        return;
    }
    item_ = std::move(item);
}

glimmer::RecipeResource* glimmer::CraftPreviewSlotComponent::GetRecipeResource() const
{
    return recipeResource_;
}

GameComponentTypeMessage glimmer::CraftPreviewSlotComponent::GetComponentTypeStatic()
{
    return COMPONENT_CRAFT_PREVIEW;
}

GameComponentTypeMessage glimmer::CraftPreviewSlotComponent::GetComponentType()
{
    return GetComponentTypeStatic();
}

const glimmer::DesignVector2D& glimmer::CraftPreviewSlotComponent::GetPosition() const
{
    return position_;
}

const glimmer::DesignVector2D& glimmer::CraftPreviewSlotComponent::GetSize() const
{
    return size_;
}

void glimmer::CraftPreviewSlotComponent::SetSize(const DesignVector2D& size)
{
    size_ = size;
}

void glimmer::CraftPreviewSlotComponent::SetPosition(const DesignVector2D& position)
{
    position_ = position;
}

bool glimmer::CraftPreviewSlotComponent::IsHovered() const
{
    return isHovered_;
}

DesignDimension glimmer::CraftPreviewSlotComponent::GetPadding() const
{
    return padding_;
}

void glimmer::CraftPreviewSlotComponent::SetPadding(DesignDimension padding)
{
    padding_ = padding;
}

void glimmer::CraftPreviewSlotComponent::SetHovered(bool hovered)
{
    isHovered_ = hovered;
}

glimmer::Item* glimmer::CraftPreviewSlotComponent::GetItem() const
{
    if (item_ == nullptr)
    {
        return nullptr;
    }
    return item_.get();
}
