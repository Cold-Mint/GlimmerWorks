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
        /**
         * Execute
         * 执行命令
         * @param commandSender commandSender 命令发送者
         * @param command command 命令
         * @param commandManager commandManager 命令管理器
         * @param onMessage onMessage 当输出消息时
         * @return
         */
        static CommandResult Execute(const CommandSender *commandSender, const std::string &command,
                                     const CommandManager *commandManager,
                                     const std::function<void(const std::string &text)> &onMessage);

    public:
        /**
         * ExecuteAsync
         * 异步执行多条命令
         * @param commandSender commandSender 命令发送者
         * @param commandList commandList 命令列表
         * @param commandManager commandManager 命令管理器
         * @param onFinished onFinished 当执行完毕时
         * @param onMessage onMessage 当输出消息时
         */
        static void ExecuteAsyncBatch(const CommandSender *commandSender, const std::vector<std::string> &commandList,
                                      const CommandManager *commandManager,
                                      const std::function<void(CommandResult result, const std::string &command)> &
                                      onFinished,
                                      const std::function<void(const std::string &text)> &onMessage = nullptr);

        /**
         * ExecuteAsync
         * 异步执行命令
        * @param commandSender commandSender 命令发送者
         * @param command command 命令
         * @param commandManager commandManager 命令管理器
         * @param onFinished onFinished 当执行完毕时
         * @param onMessage onMessage 当输出消息时
         */
        static void ExecuteAsyncSingle(const CommandSender *commandSender, const std::string &command,
                                       const CommandManager *commandManager,
                                       const std::function<void(
                                           CommandResult result, const std::string &command)> &onFinished,
                                       const std::function<void(const std::string &text)> &onMessage = nullptr);
    };
}

#endif //GLIMMERWORKS_COMMANDEXECUTOR_H
