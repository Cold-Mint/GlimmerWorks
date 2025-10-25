//
// Created by Cold-Mint on 2025/10/24.
//

#include "GenerateChunkTask.h"

#include "Chunk.h"
#include "../Constants.h"

std::string Glimmer::GenerateChunkTask::getName() {
    return "GenerateChunkTask";
}

bool Glimmer::GenerateChunkTask::execute(Vector2D startPoint, AppContext *appContext, WorldContext *worldContext) {
    auto chunkPtr = std::make_shared<Chunk>(startPoint, worldContext);
    const std::vector<int> heights = worldContext->getHeightMap(startPoint.x);
    for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
        const int columnHeight = heights[localX];

        for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
            int worldY = startPoint.y + localY;
            if (worldY < columnHeight) {
                chunkPtr->place(Vector2D(localX, localY), "dirt");
            } else {
                chunkPtr->place(Vector2D(localX, localY), TILE_ID_AIR);
            }
        }
    }
    worldContext->chunks.push_back(chunkPtr);
    return true;
}
