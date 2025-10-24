//
// Created by Cold-Mint on 2025/10/24.
//

#ifndef GLIMMERWORKS_WORLDGENERATOR_H
#define GLIMMERWORKS_WORLDGENERATOR_H

#include "GenerateChunkTask.h"
#include "WorldContext.h"
#include "WorldGeneratorTask.h"
#include "../scene/AppContext.h"

namespace Glimmer {
    /**
     * World Generator
     * 世界生成器
     */
    class WorldGenerator {
        /**
        * World context
        * 世界上下文
        */
        WorldContext *worldContext;

        /**
         * App context
         * 应用上下文
         */
        AppContext *appContext;

        /**
         * World generator tasks
         * 世界生成任务
         */
        std::vector<std::unique_ptr<WorldGeneratorTask> > worldGeneratorTasks;

    public:
        explicit WorldGenerator(WorldContext *worldContext, AppContext *appContext) : worldContext(worldContext),
            appContext(appContext) {
            worldGeneratorTasks.push_back(std::make_unique<GenerateChunkTask>());
        }

        /**
         * Generate the world
         * 生成世界
         * @param startPoint Starting point coordinate 起点坐标
         */
        void generate(Vector2D startPoint) const;

        ~WorldGenerator() {
            delete worldContext;
        }
    };
}


#endif //GLIMMERWORKS_WORLDGENERATOR_H
