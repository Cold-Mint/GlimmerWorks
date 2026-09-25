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
#include "core/math/ScreenVector2D.h"
#include "core/world/ChunkTask.h"
#include "SDL3/SDL_rect.h"

namespace glimmer {
    class Transform2DComponent;
    class CameraComponent;
    class TileVector2D;
    class Config;

    class ChunkSystem final : public GameSystem {
        CameraComponent *cameraComponent_ = nullptr;
        Transform2DComponent *cameraTransform2DComponent_ = nullptr;
        WorldVector2D cameraPosition_;
        //Cached camera view parameters, written on the main thread
        //(OnWindowSizeChanged/OnConfigChanged) and read on the tick thread (OnTick).
        //缓存相机视图参数：在主线程（OnWindowSizeChanged/OnConfigChanged）写入，
        //在 tick 线程（OnTick）读取。
        ScreenVector2D cameraSize_{800.0F, 600.0F};
        float cameraZoom_ = 2.0F;
        mutable std::mutex cameraMutex_;
        //累加tick
        uint64_t accumTime_ = 0;
        //Cached world configuration parameters, written on the main thread
        //(OnConfigChanged) and read on the tick thread (OnTick).
        //缓存世界配置参数：在主线程（OnConfigChanged）写入，在 tick 线程（OnTick）读取。
        uint64_t chunkSpawnCleanInterval_ = 0;
        uint64_t loadTerrainInterval_ = 0;
        uint16_t loadTerrainBatch_ = 0;
        uint64_t loadChunkInterval_ = 0;
        uint16_t loadChunkBatch_ = 0;
        uint64_t unloadChunkInterval_ = 0;
        uint16_t unloadChunkBatch_ = 0;
        uint64_t unloadTerrainInterval_ = 0;
        uint16_t unloadTerrainBatch_ = 0;
        float preloadStructureRadius_ = 0.0F;
        float preloadChunkRadius_ = 0.0F;
        mutable std::mutex worldConfigMutex_;

        bool firstTime_ = true;
        std::vector<std::unique_ptr<ChunkTask> > loadTerrainTasks_;
        std::vector<std::unique_ptr<ChunkTask> > loadChunkTasks_;
        std::vector<std::unique_ptr<ChunkTask> > unloadChunkTasks_;
        std::vector<std::unique_ptr<ChunkTask> > unloadTerrainTasks_;
        std::unordered_set<uint64_t> taskFingerprintSet_;

        void ExecuteLoadTerrainTask(uint16_t loadTerrainBatch);

        void ExecuteLoadChunkTask(uint16_t loadChunkBatch);

        void ExecuteUnloadChunkTask(uint16_t unloadChunkBatch);

        void ExecuteUnloadTerrainTask(uint16_t unloadTerrainBatch);

        /**
        * PushTask
        * 推送任务到列表
        * @param taskList taskList 任务列表
        * @param chunkTask chunkTask 区块任务
        * @param fingerprint fingerprint 指纹
        */
        void PushTask(std::vector<std::unique_ptr<ChunkTask> > &taskList, std::unique_ptr<ChunkTask> chunkTask,
                      uint64_t fingerprint);

        /**
         * SetOriginAndSort
         * 设置原点和排序。
         * @param taskList taskList 任务列表
         * @param origin origin 原点
         * @param sortAscending sortAscending 是否升序（从小到大）
         */
        static void SetOriginAndSort(std::vector<std::unique_ptr<ChunkTask> > &taskList, const TileVector2D& origin,
                                     bool sortAscending);

        void UpdateChunkFadeAnimation(const SDL_FRect &viewportRect) const;

        template<typename Func>
        static void ExecuteTimedTask(const uint64_t tick, const uint64_t interval, uint16_t batch, Func &&executeFunc) {
            if (interval == 0) {
                executeFunc(batch);
                return;
            }
            if (tick % interval == 0) {
                executeFunc(batch);
            }
        }

        bool UpdateCameraPosition();

        void GenerateLoadTerrainTasks(const TileVector2D &startTerrain, const TileVector2D &endTerrain);

        void GenerateLoadChunkTasks(const TileVector2D &startChunk, const TileVector2D &endChunk);

        void GenerateUnloadChunkTasks(const TileVector2D &startChunk, const TileVector2D &endChunk);

        void GenerateUnloadTerrainTasks(const TileVector2D &startTerrain, const TileVector2D &endTerrain);

    public:
        explicit ChunkSystem(WorldContext *worldContext);

        void OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t number) override;

        void OnFrameStart() override;

        void OnTick(uint64_t tick) override;

        void OnWindowSizeChanged(const int &width, const int &height) override;

        void OnConfigChanged(const Config *config) override;

        [[nodiscard]] GameSystemType GetGameSystemType() const override;
    };
}
