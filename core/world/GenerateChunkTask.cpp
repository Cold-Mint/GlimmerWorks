//
// Created by Cold-Mint on 2025/10/24.
//

#include "GenerateChunkTask.h"

#include "Chunk.h"

std::string Glimmer::GenerateChunkTask::getName() {
    return "GenerateChunkTask";
}

bool Glimmer::GenerateChunkTask::execute(Vector2D startPoint, AppContext *appContext, WorldContext *worldContext) {
    Chunk chunk(startPoint, worldContext);
    chunk.save();
    return false;
}
