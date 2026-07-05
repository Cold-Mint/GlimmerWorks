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
#if  !defined(NDEBUG)
#include "UnlockedRecipesCommand.h"

#include <sstream>

#include "core/LangsResources.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/mod/dataPack/RecipeGroup.h"
#include "core/mod/dataPack/RecipeManager.h"
#include "core/scene/AppContext.h"
#include "core/world/WorldContext.h"
#include "fmt/xchar.h"

void glimmer::UnlockedRecipesCommand::WriteRecipe(const std::string& recipesItem, std::stringstream& stringStream,
                                                  const RecipeResource* recipe)
{
    if (recipe == nullptr)
    {
        return;
    }

    const auto& output = recipe->output;
    std::string outputStr = fmt::format("{} x{}",
                                        Resource::GenerateId(output.item.GetPackageId(), output.item.GetResourceKey()),
                                        output.amount);

    std::string recipeGroupStr;
    switch (static_cast<RecipeGroup>(recipe->recipeGroup))
    {
    case RecipeGroup::None: recipeGroupStr = "None";
        break;
    case RecipeGroup::CraftTable: recipeGroupStr = "CraftTable";
        break;
    case RecipeGroup::Furnace: recipeGroupStr = "Furnace";
        break;
    default:
        if (recipe->recipeGroup >= 65)
        {
            recipeGroupStr = fmt::format("Custom{}", recipe->recipeGroup - 64);
        }
        else
        {
            recipeGroupStr = fmt::format("Unknown({})", static_cast<int>(recipe->recipeGroup));
        }
        break;
    }

    stringStream << fmt::format(fmt::runtime(recipesItem),
                                Resource::GenerateId(recipe->packId, recipe->resourceId),
                                outputStr,
                                recipe->duration, static_cast<int>(recipe->minTechnologyLevel),
                                recipeGroupStr);
}

glimmer::UnlockedRecipesCommand::UnlockedRecipesCommand(AppContext* appContext)
    : Command(appContext)
{
}

bool glimmer::UnlockedRecipesCommand::RequiresWorldContext() const
{
    return true;
}

const std::string& glimmer::UnlockedRecipesCommand::GetName() const
{
    return UNLOCKED_RECIPES_COMMAND_NAME;
}


bool glimmer::UnlockedRecipesCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                              const std::function<void(const std::string& text)>* onMessage)
{
    const AppContext* appContext = GetAppContext();
    if (appContext == nullptr || commandArgs == nullptr || onMessage == nullptr)
    {
        return false;
    }
    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    const LangsResources* langsResources = appContext->GetLangsResources();
    if (langsResources == nullptr)
    {
        return false;
    }
    const WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        onMessageRef(langsResources->worldContextIsNull);
        return false;
    }
    const RecipeManager* recipeManager = appContext->GetRecipeManager();
    if (recipeManager == nullptr)
    {
        return false;
    }
    const EntityShortCut* entityShortCut = worldContext->GetEntityShortCut();
    if (entityShortCut == nullptr)
    {
        return false;
    }
    EntityManager* entityManager = worldContext->GetEntityManager();
    if (entityManager == nullptr)
    {
        return false;
    }
    auto plyerEntity = entityShortCut->GetPlayer();
    if (WorldContext::IsEmptyEntityId(plyerEntity))
    {
        return false;
    }
    auto playerComponent = entityManager->GetComponent<PlayerComponent>(plyerEntity);
    if (playerComponent == nullptr)
    {
        return false;
    }
    const ItemContainerComponent* itemContainerComponent = entityShortCut->GetItemContainerComponent();
    if (itemContainerComponent == nullptr)
    {
        return false;
    }
    ItemContainer* itemContainer = itemContainerComponent->GetItemContainer();
    if (itemContainer == nullptr)
    {
        return false;
    }
    auto unlockedRecipes = recipeManager->FindUnlockedRecipes(playerComponent->GetTechnologyMap(),
                                                              itemContainer->GetTotalTags());
    if (unlockedRecipes.empty())
    {
        onMessageRef(langsResources->noUnlockedRecipes);
        return true;
    }

    std::stringstream stringStream;
    bool first = true;
    for (const auto& recipe : unlockedRecipes)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            stringStream << '\n';
        }
        WriteRecipe(langsResources->recipesItem, stringStream, recipe);
    }
    onMessageRef(stringStream.str());
    return true;
}
#endif
