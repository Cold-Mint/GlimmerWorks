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

std::vector<std::string> glimmer::CommandManager::GetSuggestions(const CommandArgs &commandArgs) const {
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

    return {};
}
