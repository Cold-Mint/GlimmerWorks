//
// Created by Cold-Mint on 2025/10/24.
//

#ifndef GLIMMERWORKS_WORLDGENERATORTASK_H
#define GLIMMERWORKS_WORLDGENERATORTASK_H
#include <string>

#include "WorldContext.h"
#include "../scene/AppContext.h"

namespace glimmer {
    /**
     * WorldGeneratorTask
     * 世界生成器内的一个任务
     */
    class WorldGeneratorTask {
    public:
        virtual ~WorldGeneratorTask() = default;

        /**
         * Get the name of the current task
         * 获取当前任务的名称
         * This task name will be displayed on the page.
         * 这个名称会显示在页面上。
         * @return
         */
        virtual std::string getName() = 0;

        /**
         * execute the task
         * 执行任务
         * @param startPoint 起点
         * @param appContext 应用上下文
         * @param worldContext 世界上下文
         * @return Whether the execution was successful 是否执行成功
         */
        virtual bool execute(Vector2D startPoint, AppContext *appContext, WorldContext *worldContext) = 0;
    };
}

#endif //GLIMMERWORKS_WORLDGENERATORTASK_H
