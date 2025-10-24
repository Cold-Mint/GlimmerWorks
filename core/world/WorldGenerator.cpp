//
// Created by Cold-Mint on 2025/10/24.
//

#include "WorldGenerator.h"

#include <memory>

#include "../log/LogCat.h"

void Glimmer::WorldGenerator::generate(Vector2D startPoint) const {
    for (auto &task: worldGeneratorTasks) {
        LogCat::d("execute task", task->getName());
        task->execute(startPoint, appContext, worldContext);
    }
}
