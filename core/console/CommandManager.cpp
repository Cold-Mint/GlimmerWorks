//
// Created by Cold-Mint on 2025/10/15.
//

#include "CommandManager.h"

#include <cassert>

#include "../log/LogCat.h"
#include "core/world/WorldContext.h"
#include "fmt/xchar.h"
#include "suggestion/DynamicSuggestionsManager.h"

void glimmer::CommandManager::RegisterCommand(std::unique_ptr<Command> command) {
    const std::string name = command->GetName();
    command->Initialize();
    commandMap_[name] = std::move(command);
    LogCat::i("Command registered successfully: ", name);
}

glimmer::Command *glimmer::CommandManager::GetCommand(const std::string &name) const {
    if (const auto it = commandMap_.find(name); it != commandMap_.end()) {
        return it->second.get();
    }
    return nullptr;
}

glimmer::CommandSender *glimmer::CommandManager::GetDefaultCommandSender() {
    if (worldContext_ == nullptr) {
        defaultCommandSender_.SetPosition({0, 0});
    } else {
        const GameEntity::ID player = worldContext_->GetPlayerEntity();
        if (!WorldContext::IsEmptyEntityId(player)) {
            auto transform2dComponent = worldContext_->GetComponent<Transform2DComponent>(player);
            if (transform2dComponent != nullptr) {
                defaultCommandSender_.SetPosition(transform2dComponent->GetPosition());
            }
        }
    }
    return &defaultCommandSender_;
}

glimmer::CommandSender *glimmer::CommandManager::GetMouseCommandSender() {
    if (worldContext_ == nullptr) {
        mouseCommandSender_.SetPosition({0, 0});
    } else {
        const CameraComponent *cameraComponent = worldContext_->GetCameraComponent();
        if (cameraComponent != nullptr) {
            const Transform2DComponent *transform2DComponent = worldContext_->GetCameraTransform2D();
            if (transform2DComponent != nullptr) {
                float mouseX = 0;
                float mouseY = 0;
                SDL_GetMouseState(&mouseX, &mouseY);
                const WorldVector2D worldPosition = cameraComponent->GetWorldPosition(transform2DComponent->GetPosition(),
                    {mouseX, mouseY});
                mouseCommandSender_.SetPosition(worldPosition);
            }
        }
    }
    return &mouseCommandSender_;
}

void glimmer::CommandManager::BindWorldContext(WorldContext *worldContext) {
    worldContext_ = worldContext;
    for (const auto &command: commandMap_ | std::views::values) {
        if (command->RequiresWorldContext()) {
            command->BindWorldContext(worldContext);
        }
    }
}

void glimmer::CommandManager::UnbindWorldContext() {
    worldContext_ = nullptr;
    for (const auto &command: commandMap_ | std::views::values) {
        if (command->RequiresWorldContext()) {
            command->UnBindWorldContext();
        }
    }
}

std::string glimmer::CommandManager::GetHelpText(const LangsResources *langsResources) {
    std::stringstream stringStream;
    stringStream << fmt::format(
        fmt::runtime(langsResources->commandInfo),
        commandMap_.size());
    for (const auto &[name, command]: commandMap_) {
        stringStream << '\n';
        stringStream << name;
    }
    return stringStream.str();
}

std::vector<std::string> glimmer::CommandManager::GetSuggestions(
    const DynamicSuggestionsManager *dynamicSuggestionsManager, const CommandArgs &commandArgs,
    const int cursorPos) const {
    const int size = commandArgs.GetSize();
    if (size == 0) {
        return {};
    }

    int tokenIndex = commandArgs.GetTokenIndexAtCursor(cursorPos);
    if (tokenIndex == -1) {
        return {};
    }
    if (tokenIndex == 0) {
        std::vector<std::string> results;
        const std::string keyWord = commandArgs.AsString(0);
        for (const auto &pair: commandMap_) {
            const std::string &cmd = pair.first;
            if (cmd == keyWord) {
                continue;
            }
            if (cmd.find(keyWord) != std::string::npos) {
                results.push_back(cmd);
            }
        }
        return results;
    }
    std::string commandName = commandArgs.AsString(0);
    Command *command = GetCommand(commandName);
    if (command == nullptr) {
        return {};
    }
    NodeTree<std::string> nodeTree = command->GetSuggestionsTree(commandArgs);
    std::vector<std::string> results;
    NodeTree<std::string> *nextNodeTree = &nodeTree;
    for (int index = 1; index <= tokenIndex; index++) {
        std::string keyWord = commandArgs.AsString(index);
        if (index == tokenIndex) {
            if (nextNodeTree == nullptr) {
                return {};
            }
            std::vector<std::string> suggestions = ExtendSuggestions(dynamicSuggestionsManager, nextNodeTree);
            for (const auto &suggestion: suggestions) {
                if (suggestion == keyWord) {
                    continue;
                }
                if (suggestion.find(keyWord) != std::string::npos) {
                    results.push_back(suggestion);
                }
            }
        } else {
            if (nextNodeTree == nullptr) {
                return {};
            }
            NodeTree<std::string> *dynamicTree = nullptr;
            int nextNodeTreeSize = nextNodeTree->GetSize();
            bool perfectMatch = false;
            for (int c = 0; c < nextNodeTreeSize; c++) {
                NodeTree<std::string> *node = nextNodeTree->GetChild(c);
                if (node == nullptr) {
                    continue;
                }
                auto data = node->Data();
                if (!data.has_value()) {
                    continue;
                }
                const std::string &dataValue = data.value();
                if (dataValue == keyWord) {
                    nextNodeTree = node;
                    perfectMatch = true;
                    break;
                }
                if (dataValue.starts_with('&')) {
                    auto pos = dataValue.find(':');
                    std::string dynName = dataValue.substr(0, pos); // "&biome"
                    DynamicSuggestions *dynamicSuggestions = dynamicSuggestionsManager->GetSuggestions(dynName);
                    if (dynamicSuggestions != nullptr) {
                        const std::string param = pos == std::string::npos ? "" : dataValue.substr(pos + 1); // "forest"
                        std::vector<std::string> dynList = dynamicSuggestions->GetSuggestions(param);
                        if (dynamicSuggestions->Match(keyWord, param)) {
                            dynamicTree = node;
                        }
                    }
                }
            }
            if (!perfectMatch) {
                if (dynamicTree == nullptr) {
                    nextNodeTree = nullptr;
                } else {
                    nextNodeTree = dynamicTree;
                }
            }
        }
    }
    return results;
}

std::vector<std::string> glimmer::CommandManager::GetCommandStructure(const CommandArgs &commandArgs) const {
    const int size = commandArgs.GetSize();
    if (size == 0) {
        return {"[command name:string]"};
    }
    std::string commandName = commandArgs.AsString(0);
    Command *command = GetCommand(commandName);
    if (command == nullptr) {
        return {commandName};
    }
    std::vector<std::string> results;
    results.emplace_back(commandName);
    command->PutCommandStructure(commandArgs, results);
    return results;
}

std::vector<std::string> glimmer::CommandManager::ExtendSuggestions(
    const DynamicSuggestionsManager *dynamicSuggestionsManager, NodeTree<std::string> *nextNodeTree) {
    std::vector<std::string> children = nextNodeTree->GetAllChildren();
    //Expanded dynamic suggestions.
    //展开过的动态建议。
    std::vector<std::string> unfoldList = {};
    std::vector<std::string> result = {};
    for (size_t i = 0; i < children.size(); ++i) {
        const auto &child = children[i];
        //Has dynamic suggestion been carried out
        //是否展开了动态建议
        bool unfold = false;
        if (child.starts_with('&')) {
            if (std::find(unfoldList.begin(), unfoldList.end(), child) != unfoldList.end()) {
                LogCat::e("Repeated dynamic suggestions:", child);
#if  defined(NDEBUG)
                continue;
#else
                assert(false && "Repeated dynamic suggestions");
#endif
            }
            //If the suggestion starts with &, then it is a dynamic suggestion. Here we try to expand on it.
            //如果建议以&开头，那么他是一个动态建议，我们在这里尝试展开他。
            //Extract parameters
            //提取参数
            auto pos = child.find(':');
            std::string dynName = child.substr(0, pos); // "@biome"
            DynamicSuggestions *dynamicSuggestions = dynamicSuggestionsManager->GetSuggestions(dynName);
            if (dynamicSuggestions != nullptr) {
                unfold = true;
                unfoldList.push_back(child);
                const std::string param = pos == std::string::npos ? "" : child.substr(pos + 1); // "forest"
                std::vector<std::string> dynList = dynamicSuggestions->GetSuggestions(param);
                children.insert(children.end(), dynList.begin(), dynList.end());
            }
        }
        if (!unfold) {
            result.emplace_back(child);
        }
    }
    return result;
}
