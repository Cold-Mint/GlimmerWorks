//
// Created by Cold-Mint on 2025/10/15.
//

#include "CommandExecutor.h"

#include <thread>

#include "../log/LogCat.h"

void Glimmer::CommandExecutor::executeAsync(std::string command, CommandManager *commandManager,
                                            const std::function<void(CommandResult result, const std::string &command)>
                                            &
                                            onFinished,
                                            const std::function<void(const std::string &text)> &onOutput) {
    std::string cmdStr = std::move(command);
    std::thread([cmdStr, commandManager,onOutput, onFinished]() mutable {
        cmdStr.erase(0, cmdStr.find_first_not_of(" \t\n\r"));
        cmdStr.erase(cmdStr.find_last_not_of(" \t\n\r") + 1);

        CommandResult result;

        if (cmdStr.empty()) {
            LogCat::w("Command is empty");
            result = CommandResult::EmptyArgs;
        } else {
            const CommandArgs args(cmdStr);
            if (Command *cmd = commandManager->getCommand(args.asString(0)); cmd == nullptr) {
                LogCat::e("Command not found: ", args.asString(0));
                result = CommandResult::NotFound;
            } else {
                try {
                    const bool execResult = cmd->execute(args, onOutput);
                    result = execResult ? CommandResult::Success : CommandResult::Failure;
                } catch (const std::exception &e) {
                    LogCat::e("Command execution exception: ", e.what());
                    result = CommandResult::Failure;
                }
            }
        }
        if (onFinished) onFinished(result, cmdStr);
    }).detach();
}
