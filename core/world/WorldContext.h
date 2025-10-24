//
// Created by Cold-Mint on 2025/10/24.
//

#ifndef GLIMMERWORKS_WORLDCONTEXT_H
#define GLIMMERWORKS_WORLDCONTEXT_H

#include <vector>

#include "../Constants.h"
#include "../math/FastNoiseLite.h"
#include "../math/Vector2D.h"

namespace Glimmer {
    class Saves;
    struct Chunk;

    class WorldContext {
    public:
        /**
         * World Seed
         * 世界种子
         */
        int seed;

        /**
         * The player's position
         * 玩家的位置
         */
        Vector2D playerPosition;

        /**
         * Height map
         * 高度图
         */
        std::vector<float> heightmap;

        /**
         * Game saves
         * 游戏存档
         */
        Saves *saves;

        /**
         * 用于生成高度的噪声生成器
         */
        FastNoiseLite *heightMapNoise;

        explicit WorldContext(int seed, Vector2D playerPosition, Saves *saves) : seed(seed),
            playerPosition(playerPosition), saves(saves) {
            heightmap.resize(CHUNK_SIZE);
            heightMapNoise = new FastNoiseLite();
            heightMapNoise->SetSeed(seed);
            heightMapNoise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
            heightMapNoise->SetFrequency(0.05f);
        }
    };
}


#endif //GLIMMERWORKS_WORLDCONTEXT_H
