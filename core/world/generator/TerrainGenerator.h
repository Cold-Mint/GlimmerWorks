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
        std::unordered_map<TileVector2D, std::weak_ptr<TerrainResult>, Vector2DIHash> terrainResults_;
        std::unique_ptr<ClimateSampler> climateSampler_ = std::make_unique<ClimateSampler>();

        /**
         * GenerateTerrain
         * 生成地形
         * @param biomeRegistry biomeRegistry 生物群系注册表
         * @param worldSeed worldSeed 世界种子
         * @param dimensionResource dimensionResource 维度资源
         * @param dimension dimension 维度
         * @param position position 区块位置
         * @return The generated terrain result 生成的地形结果
         */
        std::shared_ptr<TerrainResult>
        GenerateTerrain(const BiomeRegistry *biomeRegistry, int worldSeed, const DimensionResource *dimensionResource,
                        const ResourceRef &dimension, const TileVector2D &position) const;

    public:
        /**
         * GenerateOrGetTerrain
         * 生成或者获取地形
         * @param biomeRegistry
         * @param worldSeed
         * @param dimensionResource
         * @param dimension
         * @param position
         * @return 可能返回null!
         */
        std::shared_ptr<TerrainResult> GenerateOrGetTerrain(const BiomeRegistry *biomeRegistry, int worldSeed,
                                                            const DimensionResource *dimensionResource,
                                                            const ResourceRef &dimension,
                                                            const TileVector2D &position);


        /**
         * WriteTerrainTileResult
         * 写瓦片地形结果
         * @param biomeRegistry biomeRegistry 生物群系注册表
         * @param worldSeed worldSeed 世界种子
         * @param dimensionResource dimensionResource 维度资源
         * @param dimension dimension 维度
         * @param world world 世界坐标
         * @param firstTileTerrainY firstTileTerrainY 地表第一格Y坐标
         * @param terrainTileResult terrainTileResult 地形瓦片结果
         */
        void WriteTerrainTileResult(const BiomeRegistry *biomeRegistry, int worldSeed,
                                    const DimensionResource *dimensionResource, const ResourceRef &dimension,
                                    const TileVector2D &world, int firstTileTerrainY,
                                    TerrainTileResult &terrainTileResult) const;

        ClimateSampler *GetMutableClimateSampler() const;
    };
}
