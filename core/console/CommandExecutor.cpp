//
// Created by Cold-Mint on 2025/10/15.
//

#include "CommandExecutor.h"

#include <thread>

#include "../log/LogCat.h"

void glimmer::CommandExecutor::ExecuteAsync(const std::string &command, CommandManager *commandManager,
                                            const std::function<void(CommandResult result, const std::string &command)>
                                            &
                                            onFinished,
                                            const std::function<void(const std::string &text)> &onMessage) {
    std::string cmdStr = command; // skipcq: CXX-P2005
    std::thread([cmdStr = std::move(cmdStr), commandManager,onMessage, onFinished]() mutable {
        cmdStr.erase(0, cmdStr.find_first_not_of(" \t\n\r"));
        cmdStr.erase(cmdStr.find_last_not_of(" \t\n\r") + 1);

        CommandResult result;

        if (cmdStr.empty()) {
            LogCat::w("Command is empty");
            result = CommandResult::EmptyArgs;
        } else {
            const CommandArgs args(cmdStr);
            if (Command *cmd = commandManager->GetCommand(args.AsString(0)); cmd == nullptr) {
                LogCat::e("Command not found: ", args.AsString(0));
                result = CommandResult::NotFound;
            } else {
                try {
                    const bool execResult = cmd->Execute(args, onMessage);
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
