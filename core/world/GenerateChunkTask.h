//
// Created by Cold-Mint on 2025/10/24.
//

#ifndef GLIMMERWORKS_GENERATECHUNKTASK_H
#define GLIMMERWORKS_GENERATECHUNKTASK_H
#include "WorldGeneratorTask.h"

namespace glimmer {
    /**
     * GenerateChunkTask
     * 生成区块任务
     */
    class GenerateChunkTask final : public WorldGeneratorTask {
    public:
        ~GenerateChunkTask() override = default;

        std::string getName() override;

        bool execute(Vector2D startPoint, AppContext *appContext, WorldContext *worldContext) override;
    };
}

#endif //GLIMMERWORKS_GENERATECHUNKTASK_H
