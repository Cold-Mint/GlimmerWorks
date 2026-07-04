/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#pragma once
#include <FastNoiseLite.h>
#include <memory>

#include "Chunk.h"
#include "TerrainResult.h"
#include "core/math/Vector2DIHash.h"

namespace glimmer
{
    class ChunkGenerator
    {
        WorldContext* worldContext_;
        /**
        * Height map
        * 高度图
        */
        std::unordered_map<int, int> heightMap_;

        /**
        * A noise generator used for generating noise on the continent
        * 用于生成大陆的噪声生成器
        */
        std::unique_ptr<FastNoiseLite> continentHeightMapNoise_;

        ResourceRef waterTileRef_;
        ResourceRef bedrockTileRef_;

        /**
         * A noise generator used for generating mountains
         * 用于生成山脉的噪声生成器
         */
        std::unique_ptr<FastNoiseLite> mountainHeightMapNoise_;
        /**
         * Noise generator for hills
         * 用于丘陵的噪声生成器
         */
        std::unique_ptr<FastNoiseLite> hillsNoiseHeightMapNoise_;

        /**
        * Humidity chart
        * 湿度图
        * The starting x-coordinate of the block and the starting y-coordinate of the block
        * 区块的起点X坐标和区块的起点Y坐标
        */
        std::unordered_map<TileVector2D, float, Vector2DIHash> humidityMap_;
        /**
         * temperature chart
         * 温度图
         */
        std::unordered_map<TileVector2D, float, Vector2DIHash> temperatureMap_;
        std::unordered_map<TileVector2D, float, Vector2DIHash> weirdnessMap_;
        std::unordered_map<TileVector2D, float, Vector2DIHash> erosionMap_;

        /**
        * 湿度噪声生成器
        */
        std::unique_ptr<FastNoiseLite> humidityMapNoise_;

        /**
         * Odd value noise generator
         * 怪异值噪声生成器
         */
        std::unique_ptr<FastNoiseLite> weirdnessMapNoise_;

        /**
         * Erosion noise generator
         * 侵蚀噪声生成器
         */
        std::unique_ptr<FastNoiseLite> erosionMapNoise_;

        /**
        * 温度噪声生成器
        */
        std::unique_ptr<FastNoiseLite> temperatureMapNoise_;

    public:
        explicit ChunkGenerator(WorldContext* worldContext, int worldSeed);

        /**
        * get Height
        * 获取高度
        * @param x Starting point x coordinate 起点x坐标
        * @return The height array of this block (length = CHUNK_SIZE)
        */
        int GetFirstTileTerrainY(int x);

        /**
         * GenerateTerrain
         * 生成地形
         * During the terrain generation stage, the focus is on differentiating the sky, the ocean, the land, and dividing the biomes.
         * 在地形生成阶段，专注于区分天空，海洋，陆地。以及划分生物群系。
         * @param position
         * @return
         */
        std::unique_ptr<TerrainResult> GenerateTerrain(const TileVector2D& position);

        void GenerateStructure(const TileVector2D& position) const;

        TerrainTileResult GetTerrainTileResult(TileVector2D world, int firstTileTerrainY);

        std::unique_ptr<Chunk> GenerateChunkAt(const TileVector2D& position) const;


        /**
       * GetElevation
       * 获取海拔
       * @param y y
       * @return
       */
        [[nodiscard]] static float GetElevation(int y);

        /**
        * Obtain the humidity value of a certain coordinate
        * 获取某个坐标的湿度值
        * @param tileVector2d tileVector2d 瓦片坐标
        * @return 湿度0-1
        */
        float GetHumidity(const TileVector2D& tileVector2d);

        /**
         * Obtain the temperature value of a certain coordinate
         * 获取某个坐标的温度值
         * @param tileVector2d tileVector2d 瓦片坐标
         * @param elevation elevation 海拔
         * @return 温度0-1
         */
        float GetTemperature(const TileVector2D& tileVector2d, float elevation);

        /**
         * Obtain the strange value of a certain coordinate
         * 获取某个坐标的怪异值
         * @param tileVector2d tileVector2d 瓦片坐标
         * @return 怪异0-1
         */
        float GetWeirdness(const TileVector2D& tileVector2d);


        /**
         *Obtain the degree of erosion at a certain coordinate
         * 获取某个坐标的侵蚀度
         * @param tileVector2d tileVector2d 瓦片坐标
         * @return 侵蚀0-1
         */
        float GetErosion(const TileVector2D& tileVector2d);

        /**
         * GetSurfaceProximity
         * 获取地表贴近度
         * @param firstTileTerrainY
         * @param worldY
         * @return
         */
        static float GetSurfaceProximity(int firstTileTerrainY, int worldY);
    };
}
