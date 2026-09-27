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
#include <vector>

#include "ChunkTask.h"
#include "core/world/ChunkManager.h"

namespace glimmer {
    class ChunkTaskScheduler {
        /**
         * pendingTasks
         * 等待队列
         */
        std::vector<ChunkTask> pendingTasks_;

        /**
         * The coordinate key for the block task.
         * 区块任务的坐标Key。
         */
        std::vector<TileVector2D> mainTask_;

        /**
         * The task queue waiting to be deleted within the main task
         * 等待在主任务内删除的任务队列
         */
        std::unordered_set<uint64_t> pendingDeleteTasks_;

        /**
         * The status of the chunk task
         * 区块任务的Map
         */
        std::unordered_map<uint64_t, ChunkTask> chunkTaskMap_;

        ChunkManager *chunkManager_ = nullptr;

    public:
        /**
         * Add a task to the waiting queue
         * 向等待队列添加一个任务
         * @param chunkTask
         */
        void PushPendingTask(const ChunkTask &chunkTask);

        /**
         * Sorting main task
         * 排序主任务
         *
         * Those closer to the center are placed at the front.
         * 距离中心近的排在前面。
         * @param center 中心
         */
        void SortTask(const TileVector2D &center);

        /**
         * Submit the tasks in the waiting queue to the main queue
         * 将等待队列内的任务提交到主队列
         */
        void Commit();

        /**
         *
         * @param chunkTaskSize 执行多少个区块任务
         * @param terrainTaskSize 执行多少个地形任务
         */
        void Execute(uint8_t chunkTaskSize, uint8_t terrainTaskSize);
    };
}
