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

    if (size == 1) {
        std::vector<std::string> results;
        const std::string keyWord = commandArgs.AsString(0);
        for (const auto &pair: commandMap) {
            const std::string &cmd = pair.first;

            if (cmd.find(keyWord) != std::string::npos) {
                results.push_back(cmd);
            }
        }
        return results;
    }

    // TODO: 以后做参数提示

    return {
        "help","hello","hey","halt","heal","heap","hear","heart","heat","height",
        "run","jump","walk","open","close","start","stop","reset","remove","rename",
        "begin","end","finish","find","insert","include","increase","info","input","inspect"
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