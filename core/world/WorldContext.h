//
// Created by Cold-Mint on 2025/10/24.
//

#ifndef GLIMMERWORKS_WORLDCONTEXT_H
#define GLIMMERWORKS_WORLDCONTEXT_H

#include <memory>
#include <unordered_map>
#include <vector>

#include "Chunk.h"
#include "../math/FastNoiseLite.h"
#include "../math/Vector2D.h"

namespace Glimmer {
    class Saves;

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
         * All loaded blocks
         * 所有已加载的区块
         */
        std::vector<std::shared_ptr<Chunk> > chunks;

        /**
         * Height map(Divided by blocks)
         * 高度图（按区块划分）
         * key：The x-coordinate of the block's starting point 区块起点X坐标
         * value：The height array of this block (length = CHUNK_SIZE)
         */
        std::unordered_map<int, std::vector<int> > heightMap;


        /**
         * get Height Map
         * 获取高度图
         * @param x Starting point x coordinate 起点x坐标
         * @return The height array of this block (length = CHUNK_SIZE)
         */
        std::vector<int> getHeightMap(int x);

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
            heightMapNoise = new FastNoiseLite();
            heightMapNoise->SetSeed(seed);
            heightMapNoise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
            heightMapNoise->SetFrequency(0.05f);
        }
    };
}


#endif //GLIMMERWORKS_WORLDCONTEXT_H
