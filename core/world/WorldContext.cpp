//
// Created by Cold-Mint on 2025/10/24.
//

#include "WorldContext.h"

#include "../Constants.h"
#include "../log/LogCat.h"
#include "../saves/Saves.h"

std::vector<int> Glimmer::WorldContext::getHeightMap(int x) {
    const int chunkX = (x / CHUNK_SIZE) * CHUNK_SIZE;
    LogCat::d("getHeightMap called for x=", x, " aligned to chunkX=", chunkX);

    const auto it = heightMap.find(chunkX);
    if (it != heightMap.end()) {
        LogCat::d("HeightMap cache hit for chunkX=", chunkX);
        return it->second;
    }

    LogCat::d("HeightMap cache miss, generating new chunk at chunkX=", chunkX);
    std::vector<int> heights(CHUNK_SIZE);
    for (int i = 0; i < CHUNK_SIZE; ++i) {
        const auto worldX = static_cast<float>(chunkX + i);
        const float noiseValue = heightMapNoise->GetNoise(worldX, 0.0f);
        const int height = static_cast<int>((noiseValue + 1.0f) * 0.5f * (WORLD_HEIGHT - 1));
        heights[i] = height;
    }

    heightMap[chunkX] = heights;
    LogCat::d("Generated and cached heights for chunkX=", chunkX);
    return heights;
}
