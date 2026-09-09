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
#include <array>
#include <cstdint>
#include <memory>
#include <unordered_map>

#include "core/math/TileVector2D.h"
#include "core/math/Vector2DIHash.h"

namespace glimmer {
    struct NoiseConfig;
    struct DimensionResource;

    /**
     * NoiseField
     * 噪声字段
     * Identifiers for every noise generator owned by the climate sampler.
     * 气候采样器所拥有的每一个噪声生成器的标识。
     */
    enum class NoiseField : uint8_t {
        /**
         * ContinentHeight
         * 大陆高度噪声
         */
        ContinentHeight,
        /**
         * Humidity
         * 湿度噪声
         */
        Humidity,
        /**
         * Temperature
         * 温度噪声
         */
        Temperature,
        /**
         * Weirdness
         * 怪异度噪声
         */
        Weirdness,
        /**
         * Erosion
         * 侵蚀度噪声
         */
        Erosion
    };

    /**
     * The total number of noise fields.
     * 噪声字段的总数。
     */
    constexpr uint8_t kNoiseFieldCount = 5;

    /**
     * ClimateSampler
     * 气候采样器
     * Owns all terrain / climate noise generators and their per-coordinate caches.
     * 持有所有地形/气候噪声生成器以及它们按坐标缓存的结果。
     */
    class ClimateSampler {
        std::array<std::unique_ptr<FastNoiseLite>, kNoiseFieldCount> noises_;
        std::unordered_map<int, int> heightMap_;
        std::unordered_map<TileVector2D, float, Vector2DIHash> humidityMap_;
        std::unordered_map<TileVector2D, float, Vector2DIHash> temperatureMap_;
        std::unordered_map<TileVector2D, float, Vector2DIHash> weirdnessMap_;
        std::unordered_map<TileVector2D, float, Vector2DIHash> erosionMap_;

        /**
         * GetNoise
         * 获取噪声生成器
         * @param field field 噪声字段
         * @return The noise generator for the given field 对应字段的噪声生成器
         */
        [[nodiscard]] FastNoiseLite *GetNoise(NoiseField field);

        /**
         * GetNoiseConfig
         * 获取噪声配置
         * @param dimensionResource dimensionResource 维度资源
         * @param field field 噪声字段
         * @return The noise configuration for the given field 对应字段的噪声配置
         */
        static const NoiseConfig &GetNoiseConfig(const DimensionResource *dimensionResource, NoiseField field);

        /**
         * ApplyNoiseConfig
         * 应用噪声配置
         * @param noise noise 目标噪声生成器
         * @param config config 噪声配置
         * @param baseSeed baseSeed 基准种子（世界种子）
         */
        static void ApplyNoiseConfig(FastNoiseLite *noise, const NoiseConfig &config, int baseSeed);

    public:
        /**
         * ClimateSampler
         * 气候采样器构造
         * @param worldSeed worldSeed 世界种子
         * @param dimensionResource dimensionResource 维度资源
         */
        explicit ClimateSampler(int worldSeed, const DimensionResource *dimensionResource);

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
