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
#include "TechnologyCommand.h"

#include "core/LangsResources.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/scene/AppContext.h"
#include "core/world/WorldContext.h"
#include "fmt/xchar.h"

void glimmer::TechnologyCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
    if (suggestionsTree == nullptr)
    {
        return;
    }
    suggestionsTree->AddChild("list");
}

glimmer::TechnologyCommand::TechnologyCommand(AppContext* appContext)
    : Command(appContext)
{
}

const std::string& glimmer::TechnologyCommand::GetName() const
{
    return TECHNOLOGY_COMMAND_NAME;
}

bool glimmer::TechnologyCommand::RequiresWorldContext() const
{
    return true;
}

void glimmer::TechnologyCommand::PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
{
    if (strings == nullptr)
    {
        return;
    }
    strings->emplace_back("[list:string]");
}

bool glimmer::TechnologyCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
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
    if (int size = commandArgs->GetSize(); size < 2)
    {
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }
    const WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        onMessageRef(appContext->GetLangsResources()->worldContextIsNull);
        return false;
    }
    if (const std::string type = commandArgs->AsString(1); type == "list")
    {
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
        PlayerTechnologyHandler* playerTechnologyHandler = playerComponent->GetTechnologyHandler();
        if (playerTechnologyHandler == nullptr)
        {
            return false;
        }
        onMessageRef(playerTechnologyHandler->ListTechnology(langsResources->technologyItem));
        return true;
    }
    return false;
}
#endif
