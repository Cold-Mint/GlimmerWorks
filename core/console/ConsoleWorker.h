//
// Created by coldmint on 2026/5/13.
//

#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>
#include <stack>
#include <thread>

#include "CommandManager.h"
#include "CommandRequest.h"
#include "CommandResponse.h"

namespace glimmer {
    class ConsoleWorker {
        std::jthread thread_;
        std::mutex commandMutex_;
        std::condition_variable cv_;
        CommandManager *commandManager_ = nullptr;
        std::queue<std::unique_ptr<CommandRequest> > taskCommandRequestQueue_;
        std::stack<std::unique_ptr<std::function<void(const std::string &text)> > > onMessageStack_;
        std::unordered_map<uint32_t, std::unique_ptr<CommandResponse> > responseMap_;
        uint32_t commandId_ = 0;

        void WorkLoop(std::stop_token stopToken);

    public:
        explicit ConsoleWorker(CommandManager *commandManager);

        [[nodiscard]] std::unique_ptr<CommandResponse> TakeCommandResponse(uint32_t id);


        /**
         * Create Command Request
         * 创建命令请求
         * @param command command 命令
         * @param commandSender commandSender 命令发送者
         * @return Return 0. Creation failed. 返回0创建失败
         */
        uint32_t CreateRequest(std::string command, CommandSender *commandSender);

        void PopOnMessage();

        void Stop();

        void PushOnMessage(std::unique_ptr<std::function<void(const std::string &text)> > onMessage);
    };
}
