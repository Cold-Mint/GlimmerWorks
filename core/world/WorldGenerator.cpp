//
// Created by Cold-Mint on 2025/10/24.
//

#include "WorldGenerator.h"

#include <memory>

#include "../Constants.h"
#include "../log/LogCat.h"

void glimmer::WorldGenerator::Generate(Vector2D startPoint) const
{
    const int startX = static_cast<int>(std::round(startPoint.x));
    const int startY = static_cast<int>(std::round(startPoint.y));
    if (startX % CHUNK_SIZE != 0)
    {
        LogCat::e(
            "The starting point(X) must be the upper left corner of the chunk.It is divisible by .It is divisible by ",
            CHUNK_SIZE,
            ".");
        return;
    }
    if (startY % CHUNK_SIZE != 0)
    {
        LogCat::e("The starting point(Y) must be the upper left corner of the chunk.It is divisible by ", CHUNK_SIZE,
                  ".");
        return;
    }
    for (auto& task : worldGeneratorTasks)
    {
        LogCat::d("execute task", task->GetName());
        task->Execute(Vector2D(startX, startY), appContext, worldContext);
    }
}
