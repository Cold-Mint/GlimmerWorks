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
#include "CommandManager.h"

#include <cassert>

#include "core/log/LogCat.h"
#include "core/math/CoordinateTransformer.h"
#include "core/world/WorldContext.h"
#include "fmt/xchar.h"
#include "suggestion/DynamicSuggestionsManager.h"

void glimmer::CommandManager::RegisterCommand(std::unique_ptr<Command> command)
{
    const std::string& name = command->GetName();
    command->Initialize();
    commandMap_[name] = std::move(command);
}

glimmer::Command* glimmer::CommandManager::GetCommand(const std::string& name) const
{
    if (const auto it = commandMap_.find(name); it != commandMap_.end())
    {
        Command* command = it->second.get();
        if (!CanExecuteCommand(command))
        {
            return nullptr;
        }
        return command;
    }
    return nullptr;
}

glimmer::CommandSender* glimmer::CommandManager::GetDefaultCommandSender()
{
    if (entityShortCut_ == nullptr || entityManager_ == nullptr)
    {
        defaultCommandSender_.SetPosition({0, 0});
    }
    else
    {
        const GameEntityID player = entityShortCut_->GetPlayer();
        if (!WorldContext::IsEmptyEntityId(player))
        {
            auto transform2dComponent = entityManager_->GetComponent<Transform2DComponent>(player);
            if (transform2dComponent != nullptr)
            {
                defaultCommandSender_.SetPosition(transform2dComponent->GetPosition());
            }
        }
    }
    return &defaultCommandSender_;
}

glimmer::CommandSender* glimmer::CommandManager::GetMouseCommandSender()
{
    if (commandEnvironment_.worldContext == nullptr)
    {
        mouseCommandSender_.SetPosition({0, 0});
    }
    else
    {
        const EntityShortCut* entityShortCut = commandEnvironment_.worldContext->GetEntityShortCut();
        const CameraComponent* cameraComponent = entityShortCut->GetCameraComponent();
        if (cameraComponent != nullptr)
        {
            const Transform2DComponent* transform2DComponent = entityShortCut->GetCameraTransform2DComponent();
            if (transform2DComponent != nullptr)
            {
                float mouseX = 0;
                float mouseY = 0;
                SDL_GetMouseState(&mouseX, &mouseY);
                const WorldVector2D worldPosition = CoordinateTransformer::ScreenToWorld(
                    transform2DComponent->GetPosition(),
                    {mouseX, mouseY}, cameraComponent->GetSize(), cameraComponent->GetZoom());
                mouseCommandSender_.SetPosition(worldPosition);
            }
        }
    }
    return &mouseCommandSender_;
}

void glimmer::CommandManager::BindWorldContext(WorldContext* worldContext)
{
    commandEnvironment_.worldContext = worldContext;
    entityManager_ = worldContext->GetEntityManager();
    entityShortCut_ = worldContext->GetEntityShortCut();
    for (const auto& command : commandMap_ | std::views::values)
    {
        if (command->RequiresWorldContext())
        {
            command->BindWorldContext(worldContext);
        }
    }
}

void glimmer::CommandManager::UnbindWorldContext()
{
    commandEnvironment_.Reset();
    entityManager_ = nullptr;
    entityShortCut_ = nullptr;
    for (const auto& command : commandMap_ | std::views::values)
    {
        if (command->RequiresWorldContext())
        {
            command->UnBindWorldContext();
        }
    }
}

void glimmer::CommandManager::SetAllowCheats(const bool allowCheats)
{
    commandEnvironment_.allowCheats = allowCheats;
}

const glimmer::CommandEnvironment& glimmer::CommandManager::GetCommandEnvironment() const
{
    return commandEnvironment_;
}

bool glimmer::CommandManager::CanExecuteCommand(const Command* command) const
{
    if (command == nullptr)
    {
        return false;
    }
    if (command->RequiresWorldContext() && commandEnvironment_.worldContext == nullptr)
    {
        return false;
    }
    if (command->RequiresCheatEnabled() && !commandEnvironment_.allowCheats)
    {
        return false;
    }
    return true;
}

std::string glimmer::CommandManager::GetHelpText(const LangsResources* langsResources)
{
    std::stringstream stringStream;
    stringStream << fmt::format(
        fmt::runtime(langsResources->commandInfo),
        commandMap_.size());
    for (const auto& [name, command] : commandMap_)
    {
        stringStream << '\n';
        stringStream << name;
    }
    return stringStream.str();
}

std::vector<std::string> glimmer::CommandManager::GetCommandNameSuggestions(const std::string& keyWord) const
{
    std::vector<std::string> results;
    for (const auto& [commandStr, command] : commandMap_)
    {
        if (commandStr == keyWord)
        {
            continue;
        }
        if (!CanExecuteCommand(command.get()))
        {
            continue;
        }
        if (commandStr.contains(keyWord))
        {
            results.push_back(commandStr);
        }
    }
    return results;
}

std::vector<std::string> glimmer::CommandManager::CollectMatchingSuggestions(
    const std::vector<std::string>& suggestions,
    const std::string& keyWord)
{
    std::vector<std::string> results;
    for (const auto& suggestion : suggestions)
    {
        if (suggestion == keyWord)
        {
            continue;
        }
        if (suggestion.contains(keyWord))
        {
            results.push_back(suggestion);
        }
    }
    return results;
}

glimmer::NodeTree<std::string>* glimmer::CommandManager::FindNextNodeTree(
    const DynamicSuggestionsManager* dynamicSuggestionsManager,
    NodeTree<std::string>* nextNodeTree, const std::string& keyWord)
{
    if (nextNodeTree == nullptr)
    {
        return nullptr;
    }

    const int nextNodeTreeSize = nextNodeTree->GetSize();
    NodeTree<std::string>* dynamicTree = nullptr;

    for (int c = 0; c < nextNodeTreeSize; c++)
    {
        NodeTree<std::string>* node = nextNodeTree->GetChild(c);
        if (node == nullptr)
        {
            continue;
        }

        auto data = node->Data();
        if (!data.has_value())
        {
            continue;
        }

        const std::string& dataValue = data.value();
        if (dataValue == keyWord)
        {
            return node;
        }

        if (!dataValue.starts_with('&'))
        {
            continue;
        }

        auto pos = dataValue.find(':');
        std::string dynName = dataValue.substr(0, pos);
        DynamicSuggestions* dynamicSuggestions = dynamicSuggestionsManager->GetSuggestions(dynName);
        if (dynamicSuggestions == nullptr)
        {
            continue;
        }

        const std::string param = pos == std::string::npos ? "" : dataValue.substr(pos + 1);
        if (dynamicSuggestions->Match(keyWord, param))
        {
            dynamicTree = node;
        }
    }

    return dynamicTree;
}

bool glimmer::CommandManager::TryExpandDynamicSuggestion(
    const DynamicSuggestionsManager* dynamicSuggestionsManager, const std::string& child,
    std::vector<std::string>& children,
    std::unordered_set<std::string, TransparentStringHash, std::equal_to<>>& expandedSet)
{
    if (!child.starts_with('&'))
    {
        return false;
    }

    auto [it, inserted] = expandedSet.insert(child);
    if (!inserted)
    {
#if defined(NDEBUG)
        return true;
#else
        assert(false && "Repeated dynamic suggestions");
#endif
    }

    auto pos = child.find(':');
    std::string dynName = child.substr(0, pos);
    auto dyn = dynamicSuggestionsManager->GetSuggestions(dynName);
    if (dyn == nullptr)
    {
        return false;
    }

    std::optional<std::string> param = std::nullopt;
    if (pos != std::string::npos)
    {
        param = child.substr(pos + 1);
    }

    auto dynList = dyn->GetSuggestions(param);
    children.insert(children.end(), dynList.begin(), dynList.end());
    return true;
}

std::vector<std::string> glimmer::CommandManager::GetSuggestions(
    const DynamicSuggestionsManager* dynamicSuggestionsManager, const CommandArgs& commandArgs,
    const int tokenIndex) const
{
    if (tokenIndex == -1 || tokenIndex == 0)
    {
        return GetCommandNameSuggestions(commandArgs.AsString(0));
    }

    const std::string commandName = commandArgs.AsString(0);
    Command* command = GetCommand(commandName);
    if (command == nullptr)
    {
        return {};
    }

    NodeTree<std::string>* nextNodeTree = command->GetSuggestionsTree(&commandArgs);
    for (int index = 1; index < tokenIndex; index++)
    {
        nextNodeTree = FindNextNodeTree(dynamicSuggestionsManager, nextNodeTree, commandArgs.AsString(index));
        if (nextNodeTree == nullptr)
        {
            return {};
        }
    }
    if (nextNodeTree == nullptr)
    {
        return {};
    }
    const std::vector<std::string> suggestions = ExtendSuggestions(dynamicSuggestionsManager, nextNodeTree);
    return CollectMatchingSuggestions(suggestions, commandArgs.AsString(tokenIndex));
}

std::vector<std::string> glimmer::CommandManager::GetCommandStructure(const CommandArgs* commandArgs) const
{
    if (commandArgs == nullptr)
    {
        return {};
    }
    if (const int size = commandArgs->GetSize(); size == 0)
    {
        return {"[command name:string]"};
    }
    std::string commandName = commandArgs->AsString(0);
    Command* command = GetCommand(commandName);
    if (command == nullptr)
    {
        return {commandName};
    }
    std::vector<std::string> results;
    results.emplace_back(commandName);
    command->PutCommandStructure(commandArgs, &results);
    return results;
}

std::vector<std::string> glimmer::CommandManager::ExtendSuggestions(
    const DynamicSuggestionsManager* dynamicSuggestionsManager, const NodeTree<std::string>* nextNodeTree)
{
    std::vector<std::string> children = nextNodeTree->GetAllChildren();
    std::vector<std::string> result;
    std::unordered_set<std::string, TransparentStringHash, std::equal_to<>> expandedSet;

    size_t i = 0;
    while (i < children.size())
    {
        const std::string& child = children[i];
        const bool isExpanded = TryExpandDynamicSuggestion(dynamicSuggestionsManager, child, children, expandedSet);
        if (!isExpanded)
        {
            result.emplace_back(child);
        }
        ++i;
    }
    return result;
}
