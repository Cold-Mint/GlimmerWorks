//
// Created by Cold-Mint on 2026/5/28.
//

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
