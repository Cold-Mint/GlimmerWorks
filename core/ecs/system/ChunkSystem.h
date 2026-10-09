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

#include <mutex>

#include "core/ecs/GameSystem.h"
#include "core/math/TileVector2D.h"

namespace glimmer {
    class ChunkVertexVector2D;
    class ChunkManager;
    class CameraComponent;
    class Transform2DComponent;
    class ChunkTaskScheduler;
    class Config;

    class ChunkSystem final : public GameSystem {
        std::mutex worldConfigMutex_;
        CameraComponent *cameraComponent_ = nullptr;
        Transform2DComponent *cameraTransform2DComponent_ = nullptr;
        uint32_t cameraLastVersion_ = 0;
        uint32_t cameraTransformLastVersion_ = 0;
        uint8_t preloadChunkRadius_ = 1;
        uint8_t chunkScanTaskTickInterval_ = 1;
        //Has the block generation task been dispatched to the worker thread?
        //区块生成任务是否投递到了工作线程。
        std::atomic_bool chunkTaskInProgress_ = false;

        /**
         * Generate load tasks for chunks within [startChunk, endChunk].
         * 为 [startChunk, endChunk] 范围内的区块生成加载任务。
         */
        static void GenerateLoadTasks(const DimensionResource *dimensionResource, const ResourceRef &dimensionRef,
                                      const ChunkManager *chunkManager,
                                      ChunkTaskScheduler *scheduler,
                                      const ChunkVertexVector2D &startChunk,
                                      const ChunkVertexVector2D &endChunk);

        /**
         * Generate unload tasks for loaded chunks outside [startChunk, endChunk].
         * 为 [startChunk, endChunk] 范围之外的已加载区块生成卸载任务。
         */
        static void GenerateUnloadTasks(const ResourceRef &dimensionRef, const ChunkManager *chunkManager,
                                        ChunkTaskScheduler *scheduler,
                                        const ChunkVertexVector2D &startChunk,
                                        const ChunkVertexVector2D &endChunk);

        /**
         * Submit the task to the worker thread
         * 提交任务到工作线程
         */
        void PostTask();

    public:
        explicit ChunkSystem(WorldContext *worldContext);

        void OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) override;

        void OnTick(uint64_t tick) override;

        void OnConfigChanged(const Config *config) override;

        [[nodiscard]] GameSystemType GetGameSystemType() const override;
    };
}
