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

void glimmer::ChunkTaskScheduler::PushPendingTask(std::unique_ptr<ChunkTask> chunkTask) {
    std::lock_guard lock(chunkTaskMutex_);
    pendingTasks_.push_back(std::move(chunkTask));
}

void glimmer::ChunkTaskScheduler::SortTask(const TileVector2D &center) {
    std::lock_guard lock(chunkTaskMutex_);
    std::ranges::sort(mainTask_,
                      [&center](const TileVector2D &lhs, const TileVector2D &rhs) {
                          return lhs.DistanceSquared(center) < rhs.DistanceSquared(center);
                      });
}

void glimmer::ChunkTaskScheduler::Commit() {
    std::lock_guard lock(chunkTaskMutex_);
    if (pendingTasks_.empty()) {
        return;
    }
    for (auto &pendingTask: pendingTasks_) {
        if (pendingTask == nullptr) {
            continue;
        }
        const ChunkTaskType chunkTaskType = pendingTask->GetTaskType();
        if (chunkTaskType == ChunkTaskType::CANCELLED) {
            continue;
        }
        const TileVector2D &position = pendingTask->GetPosition();
        uint64_t fingerprint = position.GetFingerprint();
        auto iterator = chunkTaskMap_.find(fingerprint);
        if (iterator == chunkTaskMap_.end()) {
            //There are currently no tasks in this block.
            //当前区块没有任务。
            mainTask_.push_back(position);
            chunkTaskMap_[fingerprint] = std::move(pendingTask);
            continue;
        }
        std::unique_ptr<ChunkTask> &oldChunkTask = iterator->second;
        if (oldChunkTask == nullptr) {
            //The task has become invalid.
            //任务已失效
            mainTask_.push_back(position);
            chunkTaskMap_[fingerprint] = std::move(pendingTask);
            continue;
        }
        const ChunkTaskType oldTaskType = oldChunkTask->GetTaskType();
        if (oldTaskType == chunkTaskType) {
            //Re-requesting the task. The required additional task already exists.
            //重复请求任务，需要添加的任务已经存在。
            continue;
        }
        if (oldTaskType == ChunkTaskType::LOAD && chunkTaskType == ChunkTaskType::UNLOAD) {
            oldChunkTask->SetTaskType(ChunkTaskType::CANCELLED);
            continue;
        }
        if (oldTaskType == ChunkTaskType::UNLOAD && chunkTaskType == ChunkTaskType::LOAD) {
            oldChunkTask->SetTaskType(ChunkTaskType::CANCELLED);
            continue;
        }
        oldChunkTask->SetTaskType(pendingTask->GetTaskType());
    }
    pendingTasks_.clear();
}

size_t glimmer::ChunkTaskScheduler::GetMainTaskCount() {
    std::lock_guard lock(chunkTaskMutex_);
    return mainTask_.size();
}

std::unique_ptr<glimmer::ChunkTask> glimmer::ChunkTaskScheduler::PopFrontTask() {
    std::lock_guard lock(chunkTaskMutex_);
    if (mainTask_.empty()) {
        return nullptr;
    }
    const TileVector2D &position = mainTask_.front();
    mainTask_.pop_front();
    const uint64_t fingerprint = position.GetFingerprint();
    const auto iterator = chunkTaskMap_.find(fingerprint);
    if (iterator == chunkTaskMap_.end()) {
        return nullptr;
    }
    const auto node = chunkTaskMap_.extract(iterator);
    return std::move(node.mapped());
}
