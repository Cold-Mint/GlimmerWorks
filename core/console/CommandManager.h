//
// Created by Cold-Mint on 2025/10/15.
//

#ifndef GLIMMERWORKS_COMMANDMANAGER_H
#define GLIMMERWORKS_COMMANDMANAGER_H
#include <memory>
#include <string>
#include <unordered_map>

#include "Command.h"

namespace glimmer {
    class CommandManager {
        std::unordered_map<std::string, std::unique_ptr<Command> > commandMap{};

    public:
        void RegisterCommand(std::unique_ptr<Command> command);

        Command *GetCommand(const std::string &name);
    };
}

#endif //GLIMMERWORKS_COMMANDMANAGER_H
