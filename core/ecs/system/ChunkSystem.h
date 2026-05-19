//
// Created by Cold-Mint on 2025/11/4.
//

#pragma once

#include "core/ecs/GameSystem.h"
#include "core/math/Vector2D.h"
#include "core/world/ChunkTask.h"

namespace glimmer {
    class ChunkSystem final : public GameSystem {
        WorldVector2D cameraPosition_;
        float accumTime_ = 0.0F;
        float loadTerrainAccumTime_ = 0.0F;
        float loadChunkAccumTime_ = 0.0F;
        float unloadChunkAccumTime_ = 0.0F;
        float unloadTerrainAccumTime_ = 0.0F;
        bool firstTime_ = true;
        std::vector<std::unique_ptr<ChunkTask> > loadTerrainTasks_;
        std::vector<std::unique_ptr<ChunkTask> > loadChunkTasks_;
        std::vector<std::unique_ptr<ChunkTask> > unloadChunkTasks_;
        std::vector<std::unique_ptr<ChunkTask> > unloadTerrainTasks_;
        std::unordered_set<uint64_t> taskSignatureSet_;

        void ExecuteLoadTerrainTask(uint16_t loadTerrainBatch);

        void ExecuteLoadChunkTask(uint16_t loadChunkBatch);

        void ExecuteUnloadChunkTask(uint16_t unloadChunkBatch);

        void ExecuteUnloadTerrainTask(uint16_t unloadTerrainBatch);

        /**
         * PushTask
         * 推送任务到列表
         * @param taskList taskList 任务列表
         * @param chunkTask chunkTask 区块任务
         * @param signature signature 签名
         */
        void PushTask(std::vector<std::unique_ptr<ChunkTask> > &taskList, std::unique_ptr<ChunkTask> chunkTask,
                      uint64_t signature);

        /**
         * SetOriginAndSort
         * 设置原点和排序。
         * @param taskList taskList 任务列表
         * @param origin origin 原点
         * @param sortAscending sortAscending 是否升序（从小到大）
         */
        void SetOriginAndSort(std::vector<std::unique_ptr<ChunkTask> > &taskList, TileVector2D origin,
                              bool sortAscending);

    public:
        explicit ChunkSystem(WorldContext *worldContext);

        void Update(float delta) override;

        std::string GetName() override;
    };
}
