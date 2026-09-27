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

#include "ChunkTaskScheduler.h"

#include <algorithm>
#include <unordered_set>

void glimmer::ChunkTaskScheduler::PushPendingTask(const ChunkTask &chunkTask) {
    pendingTasks_.push_back(chunkTask);
}

void glimmer::ChunkTaskScheduler::SortTask(const TileVector2D &center) {
    std::ranges::sort(mainTask_,
                      [&center](const TileVector2D &lhs, const TileVector2D &rhs) {
                          return lhs.DistanceSquared(center) < rhs.DistanceSquared(center);
                      });
}

void glimmer::ChunkTaskScheduler::Commit() {
    if (pendingTasks_.empty()) {
        return;
    }
    for (auto &pendingTask: pendingTasks_) {
        const ChunkTaskType chunkTaskType = pendingTask.GetTaskType();
        if (chunkTaskType == ChunkTaskType::CANCELLED) {
            continue;
        }
        uint64_t fingerprint = pendingTask.GetPosition().GetFingerprint();
        auto iterator = chunkTaskMap_.find(fingerprint);
        if (iterator == chunkTaskMap_.end()) {
            //There are currently no tasks in this block.
            //当前区块没有任务。
            chunkTaskMap_[fingerprint] = pendingTask;
            mainTask_.push_back(pendingTask.GetPosition());
            continue;
        }
        ChunkTask &oldChunkTask = iterator->second;
        const ChunkTaskType oldTaskType = oldChunkTask.GetTaskType();
        if (oldTaskType == chunkTaskType) {
            //Re-requesting the task. The required additional task already exists.
            //重复请求任务，需要添加的任务已经存在。
            continue;
        }
        if (oldTaskType == ChunkTaskType::LOAD && chunkTaskType == ChunkTaskType::UNLOAD) {
            oldChunkTask.SetTaskType(ChunkTaskType::CANCELLED);
            continue;
        }
        if (oldTaskType == ChunkTaskType::UNLOAD && chunkTaskType == ChunkTaskType::LOAD) {
            oldChunkTask.SetTaskType(ChunkTaskType::CANCELLED);
            continue;
        }
        oldChunkTask.SetTaskType(pendingTask.GetTaskType());
    }
    pendingTasks_.clear();
}

void glimmer::ChunkTaskScheduler::Execute(uint8_t chunkTaskSize, uint8_t terrainTaskSize) {
    if (mainTask_.empty()) {
        return;
    }
    //The number of effective block generation tasks
    //有效的区块生成任务数量
    uint8_t effectiveTasksCount = 0;
    for (auto &tileVector2D: mainTask_) {
        if (effectiveTasksCount >= chunkTaskSize) {
            //Reaching the upper limit of the block tasks for each run.
            //抵达每次运行的区块任务上限。
            break;
        }
        auto fingerprint = tileVector2D.GetFingerprint();
        auto iterator = chunkTaskMap_.find(fingerprint);
        if (iterator == chunkTaskMap_.end()) {
            //The task located in tileVector2D does not exist.
            //位于tileVector2D的任务不存在。
            continue;
        }
        ChunkTask &chunkTask = iterator->second;
        const ChunkTaskType chunkTaskType = chunkTask.GetTaskType();
        if (chunkTaskType == ChunkTaskType::CANCELLED) {
            //The task located in tileVector2D has been cancelled.
            //位于tileVector2D的任务已取消。
            continue;
        }
        effectiveTasksCount++;
        if (chunkTaskType == ChunkTaskType::LOAD) {
            chunkManager_->LoadChunkAt(chunkTask.GetPosition());
            pendingDeleteTasks_.insert(fingerprint);
            chunkTaskMap_.erase(iterator);
        }
        if (chunkTaskType == ChunkTaskType::UNLOAD) {
            chunkManager_->UnloadChunkAt(chunkTask.GetPosition());
            pendingDeleteTasks_.insert(fingerprint);
            chunkTaskMap_.erase(iterator);
        }
    }
    //Delete invalid or executed tasks.
    //删除无效或者已执行的任务。
    std::erase_if(mainTask_,
                  [ this](const TileVector2D &task) {
                      return pendingDeleteTasks_.contains(task.GetFingerprint());
                  });
    pendingDeleteTasks_.clear();
}
