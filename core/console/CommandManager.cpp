//
// Created by Cold-Mint on 2025/10/15.
//

#include "CommandManager.h"

#include "../log/LogCat.h"

void glimmer::CommandManager::RegisterCommand(std::unique_ptr<Command> command) {
    const std::string name = command->GetName();
    command->Initialize();
    commandMap[name] = std::move(command);
    LogCat::i("Command registered successfully: ", name);
}

glimmer::Command *glimmer::CommandManager::GetCommand(const std::string &name) const {
    if (const auto it = commandMap.find(name); it != commandMap.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::vector<std::string> glimmer::CommandManager::GetSuggestions(const CommandArgs &commandArgs,
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
        for (const auto &pair: commandMap) {
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
            for (const auto &child: children) {
                if (child == keyWord) {
                    continue;
                }
                if (child.find(keyWord) != std::string::npos) {
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
