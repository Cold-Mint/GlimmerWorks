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
#pragma once
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

namespace glimmer {
    /**
     * TaskWorker
     * 耗时任务 Worker
     *
     * Responsible for executing long-running tasks, such as chunk generation and save storage.
     * 负责执行长时间运行的任务，例如区块生成、存档保存等通用耗时操作。
     *
     * It uses a single-threaded task queue model: all submitted tasks are executed sequentially
     * on a dedicated worker thread in the order they were submitted.
     * 采用单线程任务队列模型：所有提交的任务按提交顺序在专用工作线程上依次执行。
     */
    class TaskWorker {
        std::queue<std::function<void()> > tasks_;
        std::condition_variable conditionVariable_;
        std::mutex mutex_;
        std::jthread thread_;

        void WorkLoop(std::stop_token stopToken);

    public:
        ~TaskWorker();

        TaskWorker();

        /**
         * Add a task that can be waited on.
         * 添加一个可等待的任务。
         * @tparam Func
         * @param func
         * @return Call `.get()` on the returned future to wait for the execution to complete.
         * 在返回值处调用 `.get()` 等待执行完毕。
         */
        template<typename Func>
        std::future<std::invoke_result_t<Func> > AddTaskAwait(Func &&func) {
            using Result = std::invoke_result_t<Func>;
            auto promise = std::make_shared<std::promise<Result> >();
            auto future = promise->get_future();
            {
                std::lock_guard lock(mutex_);
                tasks_.push(
                    [func = std::forward<Func>(func), promise]() mutable {
                        try {
                            if constexpr (std::is_void_v<Result>) {
                                func();
                                promise->set_value();
                            } else {
                                promise->set_value(func());
                            }
                        } catch (...) {
                            promise->set_exception(std::current_exception());
                        }
                    }
                );
            }
            conditionVariable_.notify_one();
            return future;
        }

        /**
         * Post a task without waiting for the result.
         * 投递一个任务，不等待结果。
         * @param task
         */
        void PostTask(std::function<void()> task);
    };
}
