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
#include <functional>
#include <mutex>
#include <vector>

namespace glimmer {
    /**
     * PhysicsTaskQueue
     * A thread-safe queue of physics tasks.
     *
     * Box2D's world is not thread-safe, so all mutations to the physics world must
     * happen on a single thread (the TickWorker running the physics step). Code on
     * other threads (main thread, console worker, etc.) enqueues tasks here instead
     * of touching Box2D directly. The physics system drains the queue before each
     * step, so every mutation is applied in a deterministic order.
     *
     * PhysicsTaskQueue
     * 一个线程安全的物理任务队列。
     *
     * Box2D 的世界并非线程安全，因此所有对物理世界的修改都必须发生在单一线程
     * （执行物理步进的 TickWorker）上。其他线程（主线程、控制台工作线程等）不再直接
     * 触碰 Box2D，而是将任务投递到此处。物理系统在每次步进前排空队列，从而以确定的
     * 顺序应用所有修改。
     */
    class PhysicsTaskQueue {
        std::mutex mutex_;
        std::vector<std::function<void()> > tasks_;

    public:
        /**
         * Enqueue a task to be executed on the physics thread.
         * 将一个任务投递到物理线程执行。
         * @param task task 要执行的任务
         */
        void Enqueue(std::function<void()> task);

        /**
         * Drain and execute all pending tasks.
         * Must be called on the physics thread.
         * 排空并执行所有待处理任务。必须在物理线程上调用。
         */
        void Drain();
    };
}
