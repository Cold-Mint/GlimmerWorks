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

#include "BiomeMatcher.h"
#include "ClimateSampler.h"
#include "TerrainResult.h"
#include "TerrainTileResult.h"
#include "core/math/TileVector2D.h"

namespace glimmer {
    struct DimensionResource;
    class BiomeRegistry;

    /**
     * TerrainGenerator
     * 地形生成器
     * Turns noise into terrain types by composing a climate sampler and a biome matcher.
     * 通过组合气候采样器与生物群系匹配器，将噪声转化为地形类型。
     */
    class TerrainGenerator {
        ClimateSampler climateSampler_;
        BiomeMatcher biomeMatcher_;

    public:
        /**
         * TerrainGenerator
         * 地形生成器构造
         * @param worldSeed worldSeed 世界种子
         * @param dimensionResource dimensionResource 维度资源
         * @param dimensionId dimensionId 当前维度Id（packId:resourceId）
         * @param biomeRegistry biomeRegistry 生物群系注册表
         */
        TerrainGenerator(int worldSeed, const DimensionResource *dimensionResource,
                         std::string dimensionId, BiomeRegistry *biomeRegistry);

        /**
         * GenerateTerrain
         * 生成地形
         * @param position position 区块位置
         * @return The generated terrain result 生成的地形结果
         */
        std::unique_ptr<TerrainResult> GenerateTerrain(const TileVector2D &position);

        /**
         * GetTerrainTileResult
         * 获取瓦片地形结果
         * @param world world 世界坐标
         * @param firstTileTerrainY firstTileTerrainY 地表第一格Y坐标
         * @return The terrain classification for the given coordinate 该坐标的地形分类
         */
        TerrainTileResult GetTerrainTileResult(const TileVector2D &world, int firstTileTerrainY);

        /**
         * GetFirstTileTerrainY
         * 获取地表第一格的Y坐标
         * @param x x 起点x坐标
         * @return The terrain surface Y for the given column 该列的地表Y坐标
         */
        int GetFirstTileTerrainY(int x);

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
