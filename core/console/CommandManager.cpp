//
// Created by Cold-Mint on 2025/10/15.
//

#include "CommandManager.h"

#include "../log/LogCat.h"

void glimmer::CommandManager::RegisterCommand(std::unique_ptr<Command> command) {
    const std::string name = command->GetName();
    commandMap[name] = std::move(command);
    LogCat::i("Command registered successfully: ", name);
}

glimmer::Command *glimmer::CommandManager::GetCommand(const std::string &name) {
    if (const auto it = commandMap.find(name); it != commandMap.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::vector<std::string> glimmer::CommandManager::GetSuggestions(const CommandArgs &commandArgs,
                                                                 const int cursorPos) const {
    int size = commandArgs.GetSize();
    if (size == 0) {
        return {};
    }
    std::vector<std::string> results;

    if (size == 1) {
        std::string keyWord = commandArgs.AsString(0);
        for (const auto &pair: commandMap) {
            const std::string &cmd = pair.first;

            if (cmd.find(keyWord) != std::string::npos) {
                results.push_back(cmd);
            }
        }

        std::sort(results.begin(), results.end(),
                  [&](const std::string &a, const std::string &b) {
                      bool aStarts = a.rfind(keyWord, 0) == 0;
                      bool bStarts = b.rfind(keyWord, 0) == 0;

                      if (aStarts != bStarts)
                          return aStarts > bStarts; // 开头匹配排前

                      return a < b; // 字典序
                  });

        return results;
    }

    // TODO: 以后做参数提示

    return {
        "uud1","uud2","uud3","uud4","uud5","uud6","uud7","uud8","uud9","uud10",
        "uud11","uud12","uud13","uud14","uud15","uud16","uud17","uud18","uud19","uud20",
        "uud21","uud22","uud23","uud24","uud25","uud26","uud27","uud28","uud29","uud30"
    };
}

std::vector<std::string> glimmer::CommandManager::GetCommandStructure(const CommandArgs &commandArgs) {
    const int size = commandArgs.GetSize();
    if (size == 0 || size == 1) {
        return {"[command name]"};
    }
    std::vector<std::string> results;
    results.emplace_back("[command name]");
    for (int i = 1; i < size; i++) {
        std::string keyWord = commandArgs.AsString(i);
        results.push_back(keyWord);
    }
    return results;
}