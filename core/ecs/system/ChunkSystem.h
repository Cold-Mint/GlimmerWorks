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

#include "core/ecs/GameSystem.h"
#include "core/world/ChunkTask.h"
#include <functional>

namespace glimmer
{
    class Transform2DComponent;
    class CameraComponent;
    class TileVector2D;

    class ChunkSystem final : public GameSystem
    {
        CameraComponent* cameraComponent_ = nullptr;
        Transform2DComponent* cameraTransform2DComponent_ = nullptr;
        WorldVector2D cameraPosition_;
        float accumTime_ = 0.0F;
        float loadTerrainAccumTime_ = 0.0F;
        float loadChunkAccumTime_ = 0.0F;
        float unloadChunkAccumTime_ = 0.0F;
        float unloadTerrainAccumTime_ = 0.0F;
        bool firstTime_ = true;
        std::vector<std::unique_ptr<ChunkTask>> loadTerrainTasks_;
        std::vector<std::unique_ptr<ChunkTask>> loadChunkTasks_;
        std::vector<std::unique_ptr<ChunkTask>> unloadChunkTasks_;
        std::vector<std::unique_ptr<ChunkTask>> unloadTerrainTasks_;
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
        void PushTask(std::vector<std::unique_ptr<ChunkTask>>& taskList, std::unique_ptr<ChunkTask> chunkTask,
                      uint64_t fingerprint);

        /**
         * SetOriginAndSort
         * 设置原点和排序。
         * @param taskList taskList 任务列表
         * @param origin origin 原点
         * @param sortAscending sortAscending 是否升序（从小到大）
         */
        static void SetOriginAndSort(std::vector<std::unique_ptr<ChunkTask>>& taskList, TileVector2D origin,
                                     bool sortAscending);

        void UpdateChunkFadeAnimation(float delta, const SDL_FRect& viewportRect) const;

        void ExecuteTimedTask(float delta, float interval, float& accumTime, uint16_t batch,
                              const std::function<void(uint16_t)>& executeFunc);

        bool UpdateCameraPosition();

        void GenerateLoadTerrainTasks(const TileVector2D& startTerrain, const TileVector2D& endTerrain);

        void GenerateLoadChunkTasks(const TileVector2D& startChunk, const TileVector2D& endChunk);

        void GenerateUnloadChunkTasks(const TileVector2D& startChunk, const TileVector2D& endChunk);

        void GenerateUnloadTerrainTasks(const TileVector2D& startTerrain, const TileVector2D& endTerrain);

    public:
        explicit ChunkSystem(WorldContext* worldContext);

        void OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t number) override;

        void Update(float delta) override;

        [[nodiscard]] GameSystemType GetGameSystemType() const override;
    };
}
