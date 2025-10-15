//
// Created by Cold-Mint on 2025/10/15.
//

#include "CommandManager.h"

#include "../log/LogCat.h"

void Glimmer::CommandManager::registerCommand(std::unique_ptr<Command> command) {
    const std::string name = command->getName();
    commandMap[name] = std::move(command);
    LogCat::i("Command registered successfully: ", name);
}

Glimmer::Command *Glimmer::CommandManager::getCommand(const std::string &name) {
    if (const auto it = commandMap.find(name); it != commandMap.end()) {
        return it->second.get();
    }
    return nullptr;
}
