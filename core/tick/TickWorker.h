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
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

#include "ITickListener.h"

namespace glimmer {
    class TickWorker {
        std::jthread thread_;
        std::mutex mutex_;
        std::condition_variable conditionVariable_;
        std::vector<ITickListener *> listeners_;
        uint64_t tickCount_ = 0;

        void TickLoop(std::stop_token stopToken);

    public:
        /**
         * Create a TickWorker that ticks at a fixed rate.
         * 创建一个以固定速率 tick 的 TickWorker。
         */
        explicit TickWorker();

        /**
         * Add a listener that is invoked on every tick.
         * 添加一个在每个 tick 都会被调用的监听器。
         * @param listener The listener receives the cumulative tick count. 监听器接收累计 tick 次数。
         */
        void AddCallback(ITickListener *listener);

        /**
         * Remove a previously added listener.
         * 移除一个之前添加的监听器。
         * @param listener The listener to remove. 要移除的监听器。
         */
        void RemoveCallback(ITickListener *listener);

        void Stop();
    };
}
