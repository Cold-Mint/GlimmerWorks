//
// Created by Cold-Mint on 2025/10/15.
//

#include "CommandExecutor.h"

#include <future>

#include "toml11/result.hpp"


glimmer::CommandResult glimmer::CommandExecutor::Execute(const CommandSender *commandSender, const std::string &command,
                                                         const CommandManager *commandManager,
                                                         const std::function<void(
                                                             const std::string &text)> &onMessage) {
    if (commandManager == nullptr) {
        return CommandResult::Failure;
    }
    std::string commandCopy = command;
    commandCopy.erase(0, commandCopy.find_first_not_of(" \t\n\r"));
    commandCopy.erase(commandCopy.find_last_not_of(" \t\n\r") + 1);
    CommandResult result;
    if (commandCopy.empty()) {
        result = CommandResult::EmptyArgs;
    } else {
        const CommandArgs args(commandCopy);
        if (Command *cmd = commandManager->GetCommand(args.AsString(0)); cmd == nullptr) {
            result = CommandResult::NotFound;
        } else {
            try {
                const bool execResult = cmd->Execute(commandSender, args, onMessage);
                result = execResult ? CommandResult::Success : CommandResult::Failure;
            } catch (const std::exception &_) {
                result = CommandResult::Failure;
            }
        }
    }
    return result;
}

void glimmer::CommandExecutor::ExecuteAsyncBatch(const CommandSender *commandSender,
                                                 const std::vector<std::string> &commandList,
                                                 const CommandManager *commandManager,
                                                 const std::function<void(
                                                     CommandResult result, const std::string &command)> &onFinished,
                                                 const std::function<void(const std::string &text)> &onMessage) {
    std::thread([commandList,commandManager,onMessage,onFinished, commandSender]() mutable {
        for (const auto &command: commandList) {
            const CommandResult result = Execute(commandSender, command, commandManager, onMessage);
            if (onFinished) {
                onFinished(result, command);
            }
        }
    }).detach();
}

void glimmer::CommandExecutor::ExecuteAsyncSingle(const CommandSender *commandSender, const std::string &command,
                                                  const CommandManager *commandManager,
                                                  const std::function<void(
                                                      CommandResult result, const std::string &command)> &onFinished,
                                                  const std::function<void(const std::string &text)> &onMessage) {
    ExecuteAsyncBatch(commandSender, std::vector{command}, commandManager, onFinished, onMessage);
}
