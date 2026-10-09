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
#include "TaskWorker.h"

#include "core/log/LogCat.h"

void glimmer::TaskWorker::WorkLoop(std::stop_token stopToken) {
    LogCat::SetThreadName("TaskWorker");
    LogCat::i(LogLabel::DEFAULT, "task_worker_thread_started", "TaskWorker thread started");
    while (!stopToken.stop_requested()) {
        std::function<void()> task;
        {
            std::unique_lock lock(mutex_);
            conditionVariable_.wait(lock, [this, &stopToken] {
                return !tasks_.empty() || stopToken.stop_requested();
            });
            if (stopToken.stop_requested()) {
                break;
            }
            task = std::move(tasks_.front());
            tasks_.pop();
        }
        if (task) {
            task();
        }
    }
    LogCat::i(LogLabel::DEFAULT, "task_worker_thread_stopped", "TaskWorker thread stopped");
}

glimmer::TaskWorker::~TaskWorker() {
    thread_.request_stop();
    conditionVariable_.notify_all();
}

glimmer::TaskWorker::TaskWorker() {
    thread_ = std::jthread([this](const std::stop_token &stopToken) { this->WorkLoop(stopToken); });
}

void glimmer::TaskWorker::PostTask(std::function<void()> task) {
    if (task == nullptr) {
        return;
    }
    {
        std::lock_guard lock(mutex_);
        tasks_.push(std::move(task));
    }
    conditionVariable_.notify_one();
}
