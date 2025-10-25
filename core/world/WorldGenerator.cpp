//
// Created by Cold-Mint on 2025/10/24.
//

#include "WorldGenerator.h"

#include <memory>

#include "../Constants.h"
#include "../log/LogCat.h"

void Glimmer::WorldGenerator::generate(Vector2D startPoint) const {
    if (startPoint.x % CHUNK_SIZE != 0) {
        LogCat::e(
            "The starting point(X) must be the upper left corner of the chunk.It is divisible by .It is divisible by ",
            CHUNK_SIZE,
            ".");
        return;
    }
    if (startPoint.y % CHUNK_SIZE != 0) {
        LogCat::e("The starting point(Y) must be the upper left corner of the chunk.It is divisible by ", CHUNK_SIZE,
                  ".");
        return;
    }
    for (auto &task: worldGeneratorTasks) {
        LogCat::d("execute task", task->getName());
        task->execute(startPoint, appContext, worldContext);
    }
}
