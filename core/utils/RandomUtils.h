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
#include <random>

namespace glimmer
{
    class RandomUtils
    {
        static std::mt19937& GetGenerator();

    public:
        template <typename T>
        static T Random();

        template <typename T>
        static T Random(T A, T B);

        template <typename T>
        static T Random(int32_t seed, T A, T B);
    };


    template <typename T>
    T RandomUtils::Random()
    {
        if constexpr (std::is_integral_v<T>)
        {
            return static_cast<T>(GetGenerator()());
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            std::uniform_real_distribution<T> dist(0.0F, 1.0F);
            return dist(GetGenerator());
        }
        else
        {
            static_assert(std::is_arithmetic_v<T>, "Only number types are supported");
        }
        return T{};
    }

    template <typename T>
    T RandomUtils::Random(T A, T B)
    {
        const T realMin = std::min(A, B);
        const T realMax = std::max(A, B);

        if constexpr (std::is_integral_v<T>)
        {
            std::uniform_int_distribution<T> dist(realMin, realMax);
            return dist(GetGenerator());
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            std::uniform_real_distribution<T> dist(realMin, realMax);
            return dist(GetGenerator());
        }
        else
        {
            static_assert(std::is_arithmetic_v<T>, "Only number types are supported");
        }
        return T{};
    }

    template <typename T>
    T RandomUtils::Random(const int32_t seed, T A, T B)
    {
        std::mt19937 tempRng(seed);
        const T realMin = std::min(A, B);
        const T realMax = std::max(A, B);

        if constexpr (std::is_integral_v<T>)
        {
            std::uniform_int_distribution<T> dist(realMin, realMax);
            return dist(tempRng);
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            std::uniform_real_distribution<T> dist(realMin, realMax);
            return dist(tempRng);
        }
        else
        {
            static_assert(std::is_arithmetic_v<T>, "Only number types are supported");
        }
        return T{};
    }
}
