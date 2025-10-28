//
// Created by Cold-Mint on 2025/10/24.
//

#include "GenerateChunkTask.h"

#include "Chunk.h"
#include "../Constants.h"

std::string glimmer::GenerateChunkTask::GetName() {
    return "GenerateChunkTask";
}

bool glimmer::GenerateChunkTask::Execute(Vector2D startPoint, AppContext *appContext, WorldContext *worldContext) {
    const auto chunkPtr = std::make_shared<Chunk>(startPoint, worldContext);
    const std::vector<int> heights = worldContext->GetHeightMap(startPoint.x);
    for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
        const int columnHeight = heights[localX];

        for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
            int worldY = startPoint.y + localY;
            if (worldY < columnHeight) {
                chunkPtr->Place(Vector2D(localX, localY), "dirt");
            } else {
                chunkPtr->Place(Vector2D(localX, localY), TILE_ID_AIR);
            }
        }
    }
    worldContext->RegisterChunk(chunkPtr);
    return true;
}
