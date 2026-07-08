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
#include <string>
#include <unordered_map>
#include <functional>
#include "core/mod/Resource.h"
#include "core/ecs/component/TileLayerComponent.h"


namespace glimmer
{
    struct BiomeResource;

    class BiomesManager
    {
        std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<BiomeResource>,
                                                           TransparentStringHash, std::equal_to<>>,
                           TransparentStringHash, std::equal_to<>> biomeMap_{};
        std::vector<BiomeResource*> biomeVector_{};

    public:
        BiomeResource* AddResource(std::unique_ptr<BiomeResource> biomeResource);

        [[nodiscard]] BiomeResource* Find(std::string_view packId, std::string_view resourceId) const;


        /**
         * calculateBiomeScoreDelta
         * 计算群系单项差异得分
         * @param targetValue targetValue 目标值
         * @param actualValue actualValue 实际值
         * @param strictness strictness 严格度系数
         * @return
         */
        static float CalculateBiomeScoreDelta(float targetValue, float actualValue, float strictness);

        [[nodiscard]] std::span<BiomeResource*> GetBiomeVector();

        /**
         * Find Best Biome
         * 查找最合的生物群系
         * @param humidity humidity 湿度
         * @param temperature temperature 温度
         * @param weirdness weirdness 奇异度
         * @param erosion erosion 侵蚀度
         * @param elevation elevation 海拔
         * @param surfaceProximity SurfaceProximity 地表贴近度
         * @return
         */
        BiomeResource* FindBestBiome(float humidity, float temperature, float weirdness, float erosion,
                                     float elevation, float surfaceProximity) const;

        std::vector<std::string> GetBiomeList() const;

        std::string ListBiomes() const;
    };
}
