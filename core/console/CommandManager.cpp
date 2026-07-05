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
    const std::string name = command->GetName();
    command->Initialize();
    commandMap_[name] = std::move(command);
}

glimmer::Command* glimmer::CommandManager::GetCommand(const std::string& name) const
{
    if (const auto it = commandMap_.find(name); it != commandMap_.end())
    {
        return it->second.get();
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
    if (worldContext_ == nullptr)
    {
        mouseCommandSender_.SetPosition({0, 0});
    }
    else
    {
        EntityShortCut* entityShortCut = worldContext_->GetEntityShortCut();
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
    worldContext_ = worldContext;
    entityManager_ = worldContext_->GetEntityManager();
    entityShortCut_ = worldContext_->GetEntityShortCut();
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
    worldContext_ = nullptr;
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

std::vector<std::string> glimmer::CommandManager::GetSuggestions(
    const DynamicSuggestionsManager* dynamicSuggestionsManager, const CommandArgs& commandArgs,
    const int cursorPos) const
{
    const int size = commandArgs.GetSize();
    if (size == 0)
    {
        return {};
    }

    int tokenIndex = commandArgs.GetTokenIndexAtCursor(cursorPos);
    if (tokenIndex == -1)
    {
        return {};
    }
    if (tokenIndex == 0)
    {
        std::vector<std::string> results;
        const std::string keyWord = commandArgs.AsString(0);
        for (const auto& commandStr : commandMap_ | std::views::keys)
        {
            if (commandStr == keyWord)
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
    std::string commandName = commandArgs.AsString(0);
    Command* command = GetCommand(commandName);
    if (command == nullptr)
    {
        return {};
    }
    std::vector<std::string> results;
    NodeTree<std::string>* nextNodeTree = command->GetSuggestionsTree(&commandArgs);
    for (int index = 1; index <= tokenIndex; index++)
    {
        std::string keyWord = commandArgs.AsString(index);
        if (index == tokenIndex)
        {
            if (nextNodeTree == nullptr)
            {
                return {};
            }
            std::vector<std::string> suggestions = ExtendSuggestions(dynamicSuggestionsManager, nextNodeTree);
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
        }
        else
        {
            if (nextNodeTree == nullptr)
            {
                return {};
            }
            NodeTree<std::string>* dynamicTree = nullptr;
            int nextNodeTreeSize = nextNodeTree->GetSize();
            bool perfectMatch = false;
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
                    nextNodeTree = node;
                    perfectMatch = true;
                    break;
                }
                if (dataValue.starts_with('&'))
                {
                    auto pos = dataValue.find(':');
                    std::string dynName = dataValue.substr(0, pos); // "&biome"
                    DynamicSuggestions* dynamicSuggestions = dynamicSuggestionsManager->GetSuggestions(dynName);
                    if (dynamicSuggestions != nullptr)
                    {
                        const std::string param = pos == std::string::npos ? "" : dataValue.substr(pos + 1); // "forest"
                        std::vector<std::string> dynList = dynamicSuggestions->GetSuggestions(param);
                        if (dynamicSuggestions->Match(keyWord, param))
                        {
                            dynamicTree = node;
                        }
                    }
                }
            }
            if (!perfectMatch)
            {
                if (dynamicTree == nullptr)
                {
                    nextNodeTree = nullptr;
                }
                else
                {
                    nextNodeTree = dynamicTree;
                }
            }
        }
    }
    return results;
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
    //Expanded dynamic suggestions.
    //展开过的动态建议。
    std::vector<std::string> unfoldList;
    std::vector<std::string> result;
    std::unordered_set<std::string> expandedSet;

    size_t i = 0;
    while (i < children.size())
    {
        const std::string& child = children[i];
        bool isExpanded = false;

        if (child.starts_with('&'))
        {
            if (auto [it, inserted] = expandedSet.insert(child); !inserted)
            {
                LogCat::e("Repeated dynamic suggestions:", child);
#if defined(NDEBUG)
                ++i; // 跳过重复项，既不展开也不加入结果
                continue;
#else
                assert(false && "Repeated dynamic suggestions");
#endif
            }
            auto pos = child.find(':');
            std::string dynName = child.substr(0, pos);
            if (auto* dyn = dynamicSuggestionsManager->GetSuggestions(dynName); dyn != nullptr)
            {
                isExpanded = true;
                std::optional<std::string> param = std::nullopt;
                if (pos != std::string::npos)
                {
                    param = child.substr(pos + 1);
                }
                auto dynList = dyn->GetSuggestions(param);
                children.insert(children.end(), dynList.begin(), dynList.end());
            }
        }

        if (!isExpanded)
        {
            result.emplace_back(child);
        }
        ++i;
    }
    return result;
}
