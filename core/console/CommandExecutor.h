//
// Created by Cold-Mint on 2025/10/15.
//

#ifndef GLIMMERWORKS_COMMANDEXECUTOR_H
#define GLIMMERWORKS_COMMANDEXECUTOR_H
#include <functional>
#include <string>

#include "CommandManager.h"
#include "CommandResult.h"


namespace glimmer {
    class CommandExecutor {
    public:
        static void ExecuteAsync(const std::string &command, CommandManager *commandManager,
                                 const std::function<void(CommandResult result, const std::string &command)> &
                                 onFinished,
                                 const std::function<void(const std::string &text)> &onMessage = nullptr);
    };
}

#endif //GLIMMERWORKS_COMMANDEXECUTOR_H
