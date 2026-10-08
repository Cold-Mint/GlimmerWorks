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
#include <climits>
#include <memory>
#include <unordered_map>

#include "NoiseField.h"
#include "core/math/TileVector2D.h"
#include "core/math/Vector2DIHash.h"
#include "core/mod/Resource.h"

namespace glimmer {
    struct DimensionResource;

    /**
     * ClimateSampler
     * 气候采样器
     * Owns all terrain / climate noise generators and their per-coordinate caches.
     * 持有所有地形/气候噪声生成器以及它们按坐标缓存的结果。
     */
    class ClimateSampler {
        static constexpr uint8_t fieldCount_ = std::to_underlying(NoiseField::Count);
        std::unordered_map<const DimensionResource *, std::unordered_map<int, int> > heightMap_;
        std::unordered_map<const DimensionResource *, std::unordered_map<TileVector2D, float, Vector2DIHash> >
        humidityMap_;
        std::unordered_map<const DimensionResource *, std::unordered_map<TileVector2D, float, Vector2DIHash> >
        temperatureMap_;
        std::unordered_map<const DimensionResource *, std::unordered_map<TileVector2D, float, Vector2DIHash> >
        weirdnessMap_;
        std::unordered_map<const DimensionResource *, std::unordered_map<TileVector2D, float, Vector2DIHash> >
        erosionMap_;
        std::array<std::unique_ptr<FastNoiseLite>, fieldCount_> noises_;
        const DimensionResource *boundDimensionResource_ = nullptr;
        //The world seed and dimension the noises_ array is currently configured for. Used to lazily reconfigure.
        //noises_ 当前所绑定的世界种子与维度。用于惰性重新配置。
        int boundWorldSeed_ = INT_MIN;
        /**
         * GetNoise
         * 获取噪声生成器
         * @param field field 噪声字段
         * @return The noise generator for the given field 对应字段的噪声生成器
         */
        [[nodiscard]] FastNoiseLite *GetNoise(NoiseField field) const;

        /**
         * EnsureNoiseBound
         * 确保噪声生成器已按给定的世界种子与维度完成配置。
         * 仅当 (worldSeed, dimensionResource) 与上次绑定的不同时才重新配置。
         * @param worldSeed worldSeed 世界种子
         * @param dimensionResource dimensionResource 维度资源
         */
        void EnsureNoiseBound(int worldSeed, const DimensionResource *dimensionResource);

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
         * GetFirstTileTerrainY
         * 获取地表第一格的Y坐标
         * @param worldSeed worldSeed 世界种子
         * @param dimensionResource dimensionResource 维度资源
         * @param x x 起点x坐标
         * @return The terrain surface Y for the given column 该列的地表Y坐标
         */
        int GetFirstTileTerrainY(int worldSeed, const DimensionResource *dimensionResource, int x);

        /**
         * GetHumidity
         * 获取某个坐标的湿度值
         * @param worldSeed worldSeed 世界种子
         * @param dimensionResource dimensionResource 维度资源
         * @param pos pos 瓦片坐标
         * @return Humidity in [0,1] 湿度0-1
         */
        float GetHumidity(int worldSeed, const DimensionResource *dimensionResource, const TileVector2D &pos);

        /**
         * GetTemperature
         * 获取某个坐标的温度值
         * @param worldSeed worldSeed 世界种子
         * @param dimensionResource dimensionResource 维度资源
         * @param pos pos 瓦片坐标
         * @param elevation elevation 海拔
         * @return Temperature in [0,1] 温度0-1
         */
        float GetTemperature(int worldSeed, const DimensionResource *dimensionResource, const TileVector2D &pos,
                             float elevation);

        /**
         * GetWeirdness
         * 获取某个坐标的怪异值
         * @param worldSeed worldSeed 世界种子
         * @param dimensionResource dimensionResource 维度资源
         * @param pos pos 瓦片坐标
         * @return Weirdness in [0,1] 怪异0-1
         */
        float GetWeirdness(int worldSeed, const DimensionResource *dimensionResource, const TileVector2D &pos);

        /**
         * GetErosion
         * 获取某个坐标的侵蚀度
         * @param worldSeed worldSeed 世界种子
         * @param dimensionResource dimensionResource 维度资源
         * @param pos pos 瓦片坐标
         * @return Erosion in [0,1] 侵蚀0-1
         */
        float GetErosion(int worldSeed, const DimensionResource *dimensionResource, const TileVector2D &pos);
    };
}
