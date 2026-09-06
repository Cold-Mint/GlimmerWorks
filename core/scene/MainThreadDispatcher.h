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

#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <functional>

namespace glimmer {
    /**
     * MainThreadDispatcher
     * 主线程任务调度器
     */
    class MainThreadDispatcher {
        std::mutex mainThreadMutex_;
        std::queue<std::function<void()> > mainThreadTasks_;
        std::thread::id mainThreadId_;

    public:
        MainThreadDispatcher();

        ~MainThreadDispatcher();

        [[nodiscard]] bool IsMainThread() const;

        void ProcessMainThreadTasks();

        /**
         * Send a task to the main thread. It can be waited for.
         * 投递一个任务到主线程，可等待。
         * @tparam Func
         * @param func
         * @return Call the `.get()` method at the return value to wait for the execution to complete. 在返回值处调用.get()等待执行完毕。
         */
        template<typename Func>
        std::future<std::invoke_result_t<Func> > AddMainThreadTaskAwait(Func &&func) {
            using Result = std::invoke_result_t<Func>;
            if (IsMainThread()) {
                std::promise<Result> p;
                if constexpr (std::is_void_v<Result>) {
                    func();
                    p.set_value();
                } else {
                    p.set_value(func());
                }
                return p.get_future();
            }
            auto promise = std::make_shared<std::promise<Result> >();
            auto future = promise->get_future();
            {
                std::lock_guard lock(mainThreadMutex_);
                mainThreadTasks_.push(
                    [func = std::forward<Func>(func), promise]() mutable {
                        if constexpr (std::is_void_v<Result>) {
                            func();
                            promise->set_value();
                        } else {
                            promise->set_value(func());
                        }
                    }
                );
            }

            return future;
        }

        /**
         * RunOnMainThread
         * 在主线程运行
         *
         * If the currently executing thread is the main thread, then call it immediately. Otherwise, schedule it for the next frame.
         * 如果当前执行线程为主线程，那么立刻调用。否则，投递到下一帧。
         * @param task
         */
        void RunOnMainThread(std::function<void()> task);

        /**
         * Post to the next frame
         * 投递到下一帧
         * @param task
         */
        void PostToNextMainFrame(std::function<void()> task);
    };
}
