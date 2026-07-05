/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "ConsoleWorker.h"

#include "CommandResponse.h"
#include "CommandResult.h"
#include "core/console/CommandRequest.h"


void glimmer::ConsoleWorker::WorkLoop(std::stop_token stopToken)
{
    while (!stopToken.stop_requested())
    {
        std::unique_lock lock(commandMutex_);
        cv_.wait(lock, [this, &stopToken]
        {
            return !taskCommandRequestQueue_.empty() || stopToken.stop_requested();
        });
        lock.unlock();
        if (stopToken.stop_requested())
        {
            break;
        }
        if (onMessageStack_.empty())
        {
            continue;
        }
        if (taskCommandRequestQueue_.empty())
        {
            continue;
        }

        auto commandRequest = std::move(taskCommandRequestQueue_.front());
        taskCommandRequestQueue_.pop();
        const std::function<void(const std::string& text)>* currentCallback = onMessageStack_.top().get();
        if (commandRequest == nullptr || commandManager_ == nullptr || currentCallback == nullptr)
        {
            continue;
        }

        auto commandResponse = std::make_unique<CommandResponse>();
        const std::string_view& command = commandRequest->GetCommand();
        const CommandArgs args(command);
        if (Command* cmd = commandManager_->GetCommand(args.AsString(0)); cmd == nullptr)
        {
            commandResponse->SetCommandResult(CommandResult::NotFound, command);
        }
        else
        {
            const bool success = cmd->Execute(
                commandRequest->GetCommandSender(),
                &args,
                currentCallback
            );
            commandResponse->SetCommandResult(
                success ? CommandResult::Success : CommandResult::Failure, command
            );
        }
        std::lock_guard writeLock(commandMutex_);
        responseMap_[commandRequest->GetId()] = std::move(commandResponse);
    }
}

glimmer::ConsoleWorker::ConsoleWorker(CommandManager* commandManager)
    : commandManager_(commandManager)
{
    thread_ = std::jthread([this](std::stop_token stopToken) { this->WorkLoop(stopToken); });
}

std::unique_ptr<glimmer::CommandResponse> glimmer::ConsoleWorker::TakeCommandResponse(const uint32_t id)
{
    std::lock_guard lock(commandMutex_);
    const auto iterator = responseMap_.find(id);
    if (iterator == responseMap_.end())
    {
        return nullptr;
    }
    std::unique_ptr<CommandResponse> result = std::move(iterator->second);
    responseMap_.erase(iterator);
    return result;
}

uint32_t glimmer::ConsoleWorker::CreateRequest(const std::string& command, CommandSender* commandSender)
{
    if (command.empty() || commandSender == nullptr)
    {
        return 0;
    }
    std::lock_guard lock(commandMutex_);
    commandId_++;
    const uint32_t id = commandId_;
    taskCommandRequestQueue_.push(
        std::make_unique<CommandRequest>(id, command, commandSender)
    );
    cv_.notify_one();
    return id;
}

void glimmer::ConsoleWorker::PopOnMessage()
{
    std::lock_guard lock(commandMutex_);
    if (!onMessageStack_.empty())
    {
        onMessageStack_.pop();
    }
    cv_.notify_one();
}

void glimmer::ConsoleWorker::Stop()
{
    thread_.request_stop();
    cv_.notify_one();
}

void glimmer::ConsoleWorker::PushOnMessage(std::unique_ptr<std::function<void(const std::string& text)>> onMessage)
{
    std::lock_guard lock(commandMutex_);
    onMessageStack_.push(std::move(onMessage));
    cv_.notify_one();
}
