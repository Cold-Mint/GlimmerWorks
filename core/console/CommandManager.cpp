//
// Created by Cold-Mint on 2025/10/15.
//

#include "CommandManager.h"

#include <assert.h>

#include "../log/LogCat.h"
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
    NodeTree<std::string> nodeTree = command->GetSuggestionsTree();
    std::vector<std::string> results;
    NodeTree<std::string> *nextNodeTree = &nodeTree;
    for (int index = 1; index <= tokenIndex; index++) {
        std::string keyWord = commandArgs.AsString(index);
        if (index == tokenIndex) {
            std::vector<std::string> children = nextNodeTree->GetAllChildren();
            //Expanded dynamic suggestions.
            //展开过的动态建议。
            std::vector<std::string> unfoldList = {};
            for (size_t i = 0; i < children.size(); ++i) {
                const auto &child = children[i];
                if (child == keyWord) {
                    continue;
                }
                //Has dynamic suggestion been carried out
                //是否展开了动态建议
                bool unfold = false;
                if (child.starts_with('@')) {
                    if (std::ranges::find(unfoldList, child) != unfoldList.end()) {
                        LogCat::e("Repeated dynamic suggestions:", child);
#if  defined(NDEBUG)
                        continue;
#else
                        assert(false && "Repeated dynamic suggestions");
#endif
                    }
                    //If the suggestion starts with @, then it is a dynamic suggestion. Here we try to expand on it.
                    //如果建议以@开头，那么他是一个动态建议，我们在这里尝试展开他。
                    DynamicSuggestions *dynamicSuggestions = dynamicSuggestionsManager->GetSuggestions(child);
                    if (dynamicSuggestions != nullptr) {
                        unfold = true;
                        unfoldList.push_back(child);
                        std::vector<std::string> dynList = dynamicSuggestions->GetSuggestions();
                        children.insert(children.end(), dynList.begin(), dynList.end());
                    }
                }
                if (!unfold && child.find(keyWord) != std::string::npos) {
                    results.push_back(child);
                }
            }
        } else {
            nextNodeTree = nodeTree.GetChildByValue(keyWord);
            if (nextNodeTree == nullptr) {
                return {};
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
