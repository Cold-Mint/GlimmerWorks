//
// Created by Cold-Mint on 2026/5/13.
//

#include "ConsoleWorker.h"

#include "CommandResponse.h"
#include "CommandResult.h"
#include "core/console/CommandRequest.h"


void glimmer::ConsoleWorker::WorkLoop(std::stop_token stopToken) {
    while (!stopToken.stop_requested()) {
        std::unique_lock lock(commandMutex_);
        cv_.wait(lock, [this, &stopToken] {
            return !taskCommandRequestQueue_.empty() || stopToken.stop_requested();
        });
        if (stopToken.stop_requested()) {
            break;
        }
        if (onMessageStack_.empty()) {
            continue;
        }
        if (taskCommandRequestQueue_.empty()) {
            continue;
        }

        auto commandRequest = std::move(taskCommandRequestQueue_.front());
        taskCommandRequestQueue_.pop();
        const std::function<void(const std::string &text)> *currentCallback = onMessageStack_.top().get();
        lock.unlock();

        if (!commandRequest || commandManager_ == nullptr || !currentCallback) {
            continue;
        }

        auto commandResponse = std::make_unique<CommandResponse>();
        const std::string &command = commandRequest->GetCommand();
        const CommandArgs args(command);

        try {
            if (Command *cmd = commandManager_->GetCommand(args.AsString(0)); cmd == nullptr) {
                commandResponse->SetCommandResult(CommandResult::NotFound, command);
            } else {
                const bool success = cmd->Execute(
                    commandRequest->GetCommandSender(),
                    &args,
                    currentCallback
                );
                commandResponse->SetCommandResult(
                    success ? CommandResult::Success : CommandResult::Failure, command
                );
            }
        } catch (...) {
            commandResponse->SetCommandResult(CommandResult::Failure, command);
        }
        std::lock_guard writeLock(commandMutex_);
        responseMap_[commandRequest->GetId()] = std::move(commandResponse);
    }
}

glimmer::ConsoleWorker::ConsoleWorker(CommandManager *commandManager)
    : commandManager_(commandManager) {
    thread_ = std::jthread(&ConsoleWorker::WorkLoop, this);
}

std::unique_ptr<glimmer::CommandResponse> glimmer::ConsoleWorker::TakeCommandResponse(const uint32_t id) {
    std::lock_guard lock(commandMutex_);
    const auto iterator = responseMap_.find(id);
    if (iterator == responseMap_.end()) {
        return nullptr;
    }
    std::unique_ptr<CommandResponse> result = std::move(iterator->second);
    responseMap_.erase(iterator);
    return result;
}

uint32_t glimmer::ConsoleWorker::CreateRequest(std::string command, CommandSender *commandSender) {
    if (command.empty() || commandSender == nullptr) {
        return 0;
    }
    std::lock_guard lock(commandMutex_);
    commandId_++;
    const uint32_t id = commandId_;
    taskCommandRequestQueue_.push(
        std::make_unique<CommandRequest>(id, std::move(command), commandSender)
    );
    cv_.notify_one();
    return id;
}

void glimmer::ConsoleWorker::PopOnMessage() {
    std::lock_guard lock(commandMutex_);
    if (!onMessageStack_.empty()) {
        onMessageStack_.pop();
    }
    cv_.notify_one();
}

void glimmer::ConsoleWorker::Stop() {
    thread_.request_stop();
    cv_.notify_one();
}

void glimmer::ConsoleWorker::PushOnMessage(std::unique_ptr<std::function<void(const std::string &text)> > onMessage) {
    std::lock_guard lock(commandMutex_);
    onMessageStack_.push(std::move(onMessage));
    cv_.notify_one();
}
