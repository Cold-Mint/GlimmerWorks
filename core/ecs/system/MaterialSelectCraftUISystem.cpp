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
#include "MaterialSelectCraftUISystem.h"

#include "core/layout/GridLayoutStepper.h"
#include "core/layout/VerticalLayoutStepper.h"
#include "core/world/WorldContext.h"

glimmer::MaterialSelectCraftUISystem::MaterialSelectCraftUISystem(WorldContext* worldContext)
    : GUISystem(worldContext)
{
    AppContext* appContext = worldContext_->GetAppContext();
    if (appContext == nullptr)
    {
        return;
    }
    stringManager_ = appContext->GetStringManager();
}

void glimmer::MaterialSelectCraftUISystem::OnActivationChanged(bool activeStatus)
{
    if (activeStatus)
    {
        CraftPreviewSlotComponent* slotComponent = entityShortCut_->GetSelectedCraftPreviewSlotComponent();
        if (slotComponent == nullptr)
        {
            worldContext_->PopGuiSystemType();
            return;
        }
        RecipeResource* recipeResource = slotComponent->GetRecipeResource();
        if (recipeResource == nullptr)
        {
            worldContext_->PopGuiSystemType();
            return;
        }
        const size_t inputResourceSize = recipeResource_->input.size();
        if (inputResourceSize == 0)
        {
            worldContext_->PopGuiSystemType();
        }
        recipeResource_ = recipeResource;
        std::vector<RequiredTag>& input = recipeResource_->input;
        recipeStringMap_.clear();
        if (stringManager_ == nullptr)
        {
            return;
        }
        for (auto& requiredTag : input)
        {
            std::stringstream stringStream;
            uint64_t cachedTagId = requiredTag.GetCachedTagId();
            std::optional<std::string> tagTranslateOptional = stringManager_->GetTagTranslate(cachedTagId);
            if (tagTranslateOptional.has_value())
            {
                stringStream << tagTranslateOptional.value();
            }
            else
            {
                stringStream << requiredTag.requiredTag;
            }
            stringStream << '*';
            stringStream << static_cast<int>(requiredTag.requiredWeight);
            recipeStringMap_[cachedTagId] = stringStream.str();
        }
    }
}

glimmer::GameSystemType glimmer::MaterialSelectCraftUISystem::GetGameSystemType() const
{
    return GameSystemType::MaterialSelectCraftUISystem;
}
