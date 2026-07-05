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
#include "EcsCommand.h"

#include "fmt/base.h"
#include "fmt/compile.h"
#include "core/scene/AppContext.h"
#include "core/scene/WorldScene.h"

void glimmer::EcsCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
    if (suggestionsTree == nullptr)
    {
        return;
    }
    suggestionsTree->AddChild("showEntityList");
    suggestionsTree->AddChild("displayDetailedInformation");
    suggestionsTree->AddChild("activeSystems");
}

glimmer::EcsCommand::EcsCommand(AppContext* appContext) : Command(appContext)
{
}

std::optional<std::string> glimmer::EcsCommand::EntityToString(GameEntityID gameEntityId) const
{
    const WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        return std::nullopt;
    }
    EntityManager* entityManager = worldContext->GetEntityManager();
    const std::vector<GameComponent*> components = entityManager->GetAllComponent(gameEntityId);
    std::stringstream stringStream;
    for (int i = 0; i < components.size(); ++i)
    {
        auto const& component = components[i];
        stringStream << component->GetComponentType();
        if (i != components.size() - 1)
        {
            stringStream << ',';
        }
    }
    return fmt::format("id={} isPersistable={} \ncomponents = [{}]\n", gameEntityId,
                       entityManager->IsPersistable(gameEntityId),
                       stringStream.str());
}

const std::string& glimmer::EcsCommand::GetName() const
{
    return ECS_COMMAND_NAME;
}

void glimmer::EcsCommand::PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
{
    if (commandArgs == nullptr || strings == nullptr)
    {
        return;
    }
    strings->emplace_back("[type:string]");
    if (commandArgs->GetSize() > 1 && commandArgs->AsString(1) == "displayDetailedInformation")
    {
        strings->emplace_back("[id:uint]");
    }
}

bool glimmer::EcsCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                  const std::function<void(const std::string& text)>* onMessage)
{
    AppContext* appContext = GetAppContext();
    WorldContext* worldContext = GetWorldContext();
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
    if (worldContext == nullptr)
    {
        onMessageRef(langsResources->worldContextIsNull);
        return false;
    }
    int size = commandArgs->GetSize();
    if (commandArgs->GetSize() < 2)
    {
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }
    std::string arg = commandArgs->AsString(1);
    if (arg == "showEntityList")
    {
        for (const std::vector<uint32_t> allGameEntities = worldContext->GetEntityManager()->GetAllEntityIDs(); const
             auto& e :
             allGameEntities)
        {
            auto string = EntityToString(e);
            if (string.has_value())
            {
                onMessageRef(string.value());
            }
        }
        return true;
    }
    if (arg == "displayDetailedInformation")
    {
        if (commandArgs->GetSize() < 3)
        {
            onMessageRef(fmt::format(
                fmt::runtime(langsResources->insufficientParameterLength),
                3, size));
            return false;
        }
        const uint32_t id = commandArgs->AsInt(2);
        if (WorldContext::IsEmptyEntityId(id))
        {
            onMessageRef(langsResources->cantFindObject);
            return false;
        }
        auto string = EntityToString(id);
        if (string.has_value())
        {
            onMessageRef(string.value());
        }
        return true;
    }
    if (arg == "activeSystems")
    {
        for (std::vector<GameSystemType> allGameSystems = worldContext->GetAllActiveSystemType(); auto& type :
             allGameSystems)
        {
            onMessageRef(fmt::format("{}\n", static_cast<uint8_t>(type)));
        }
        return true;
    }
    return false;
}


bool glimmer::EcsCommand::RequiresWorldContext() const
{
    return true;
}
#endif
