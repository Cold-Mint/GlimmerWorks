//
// Created by coldmint on 2026/2/4.
//

#ifndef GLIMMERWORKS_CHUNKGENERATOR_H
#define GLIMMERWORKS_CHUNKGENERATOR_H
#include <FastNoiseLite.h>
#include <memory>

#include "Chunk.h"
#include "TerrainResult.h"
#include "core/math/Vector2DI.h"

namespace glimmer {
    class ChunkGenerator {
        WorldContext *worldContext_;
        /**
        * Height map
        * 高度图
        */
        std::unordered_map<int, int> heightMap_;

        /**
        * A noise generator used for generating noise on the continent
        * 用于生成大陆的噪声生成器
        */
        std::unique_ptr<FastNoiseLite> continentHeightMapNoise;
        /**
         * A noise generator used for generating mountains
         * 用于生成山脉的噪声生成器
         */
        std::unique_ptr<FastNoiseLite> mountainHeightMapNoise;
        /**
         * Noise generator for hills
         * 用于丘陵的噪声生成器
         */
        std::unique_ptr<FastNoiseLite> hillsNoiseHeightMapNoise;

        /**
        * Humidity chart
        * 湿度图
        * The starting x-coordinate of the block and the starting y-coordinate of the block
        * 区块的起点X坐标和区块的起点Y坐标
        */
        std::unordered_map<TileVector2D, float, Vector2DIHash> humidityMap;
        /**
         * temperature chart
         * 温度图
         */
        std::unordered_map<TileVector2D, float, Vector2DIHash> temperatureMap;
        std::unordered_map<TileVector2D, float, Vector2DIHash> weirdnessMap;
        std::unordered_map<TileVector2D, float, Vector2DIHash> erosionMap;

        /**
        * 湿度噪声生成器
        */
        std::unique_ptr<FastNoiseLite> humidityMapNoise;

        /**
         * Odd value noise generator
         * 怪异值噪声生成器
         */
        std::unique_ptr<FastNoiseLite> weirdnessMapNoise;

        /**
         * Erosion noise generator
         * 侵蚀噪声生成器
         */
        std::unique_ptr<FastNoiseLite> erosionMapNoise;

        /**
        * 温度噪声生成器
        */
        std::unique_ptr<FastNoiseLite> temperatureMapNoise;

    public:
        explicit ChunkGenerator(WorldContext *worldContext, int worldSeed);

        /**
        * get Height
        * 获取高度
        * @param x Starting point x coordinate 起点x坐标
        * @return The height array of this block (length = CHUNK_SIZE)
        */
        int GetHeight(int x);

        /**
         * GenerateTerrain
         * 生成地形
         * During the terrain generation stage, the focus is on differentiating the sky, the ocean, the land, and dividing the biomes.
         * 在地形生成阶段，专注于区分天空，海洋，陆地。以及划分生物群系。
         * @param position
         * @return
         */
        std::unique_ptr<TerrainResult> GenerateTerrain(TileVector2D position);

        TerrainTileResult GetTerrainTileResult(TileVector2D world, int height);

        std::unique_ptr<Chunk> GenerateChunkAt(TileVector2D position);


        /**
       * GetElevation
       * 获取海拔
       * @param height 高度
       * @return
       */
        [[nodiscard]] static float GetElevation(int height);

        /**
        * Obtain the humidity value of a certain coordinate
        * 获取某个坐标的湿度值
        * @param tileVector2d tileVector2d 瓦片坐标
        * @return 湿度0-1
        */
        float GetHumidity(TileVector2D tileVector2d);

        /**
         * Obtain the temperature value of a certain coordinate
         * 获取某个坐标的温度值
         * @param tileVector2d tileVector2d 瓦片坐标
         * @param elevation elevation 海拔
         * @return 温度0-1
         */
        float GetTemperature(TileVector2D tileVector2d, float elevation);

        /**
         * Obtain the strange value of a certain coordinate
         * 获取某个坐标的怪异值
         * @param tileVector2d tileVector2d 瓦片坐标
         * @return 怪异0-1
         */
        float GetWeirdness(TileVector2D tileVector2d);


        /**
         *Obtain the degree of erosion at a certain coordinate
         * 获取某个坐标的侵蚀度
         * @param tileVector2d tileVector2d 瓦片坐标
         * @return 侵蚀0-1
         */
        float GetErosion(TileVector2D tileVector2d);
    };
}


#endif //GLIMMERWORKS_CHUNKGENERATOR_H
