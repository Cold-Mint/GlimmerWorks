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
#include <memory>
#include <string>

#include "Chunk.h"
#include "StructurePlacer.h"
#include "TerrainGenerator.h"

namespace glimmer {
    class WorldContext;
    struct DimensionResource;
    class BiomeRegistry;
    class TerrainResult;

    /**
     * ChunkGenerator
     * 区块生成器（门面）
     * Orchestrates the chunk generation pipeline by composing the terrain generator,
     * structure placer, decorator applier and tile populators.
     * 通过组合地形生成器、结构放置器、装饰器应用器与瓦片填充器，编排区块生成流水线。
     */
    class ChunkGenerator {
        WorldContext *worldContext_;
        std::string dimensionId_;
        //FixMe：在区块生成器内拆分处地形生成器，他们不再依赖。
        TerrainGenerator terrainGenerator_;
        StructurePlacer structurePlacer_;

        /**
         * ResolveDimensionId
         * 解析维度Id
         * @param dimensionResource dimensionResource 维度资源
         * @return The dimension id (packId:resourceId) 维度Id（packId:resourceId）
         */
        static std::string ResolveDimensionId(const DimensionResource *dimensionResource);

        /**
         * ResolveBiomeRegistry
         * 解析生物群系注册表
         * @param worldContext worldContext 世界上下文
         * @return The biome registry, or nullptr if unavailable 生物群系注册表，不可用时返回nullptr
         */
        static BiomeRegistry *ResolveBiomeRegistry(WorldContext *worldContext);

    public:
        /**
         * ChunkGenerator
         * 区块生成器构造
         * @param worldContext worldContext 世界上下文
         * @param worldSeed worldSeed 世界种子
         * @param dimensionResource dimensionResource 维度资源
         */
        ChunkGenerator(WorldContext *worldContext, int worldSeed, const DimensionResource *dimensionResource);

        /**
         * GenerateTerrain
         * 生成地形
         * @param position position 区块位置
         * @return The generated terrain result 生成的地形结果
         */
        std::shared_ptr<TerrainResult> GenerateTerrain(const TileVector2D &position);

        /**
         * GenerateStructure
         * 生成结构
         * @param position position 区块位置
         */
        void GenerateStructure(const TileVector2D &position) const;

        /**
         * GenerateChunkAt
         * 在指定位置生成区块
         * @param position position 区块位置
         * @return The generated chunk, or nullptr on failure 生成的区块，失败时返回nullptr
         */
        std::unique_ptr<Chunk> GenerateChunkAt(const TileVector2D &position) const;

        /**
         * GetFirstTileTerrainY
         * 获取地表第一格的Y坐标
         * @param x x 起点x坐标
         * @return The terrain surface Y for the given column 该列的地表Y坐标
         */
        int GetFirstTileTerrainY(int x);

        /**
         * GetDimensionId
         * 获取该生成器所属维度的Id
         * @return The dimension id 维度Id
         */
        [[nodiscard]] const std::string &GetDimensionId() const;

        /**
         * GetHumidity
         * 获取某个坐标的湿度值
         * @param pos pos 瓦片坐标
         * @return Humidity in [0,1] 湿度0-1
         */
        float GetHumidity(const TileVector2D &pos);

        /**
         * GetTemperature
         * 获取某个坐标的温度值
         * @param pos pos 瓦片坐标
         * @param elevation elevation 海拔
         * @return Temperature in [0,1] 温度0-1
         */
        float GetTemperature(const TileVector2D &pos, float elevation);

        /**
         * GetWeirdness
         * 获取某个坐标的怪异值
         * @param pos pos 瓦片坐标
         * @return Weirdness in [0,1] 怪异0-1
         */
        float GetWeirdness(const TileVector2D &pos);

        /**
         * GetErosion
         * 获取某个坐标的侵蚀度
         * @param pos pos 瓦片坐标
         * @return Erosion in [0,1] 侵蚀0-1
         */
        float GetErosion(const TileVector2D &pos);
    };
}
