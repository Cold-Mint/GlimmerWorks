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
#include <bit>
#include <cmath>
#include <numbers>

#include "core/IFingerprintable.h"

namespace glimmer
{
    using Vector2DFingerprint = uint64_t;

    template <typename Derived>
    class Vector2DBase : public IFingerprintable
    {
    public:
        float x = 0.0F;
        float y = 0.0F;
        Vector2DBase() = default;

        Vector2DBase(float tempX, float tempY);

        /**
         * Vector addition
         * 向量加法
         * @param lhs
         * @param rhs
         * @return
         */
        friend Derived operator+(const Derived& lhs, const Derived& rhs)
        {
            Derived result(lhs);
            result.x += rhs.x;
            result.y += rhs.y;
            return result;
        }

        /**
         * Vector subtraction
         * 向量减法
         * @param lhs
         * @param rhs
         * @return
         */
        friend Derived operator-(const Derived& lhs, const Derived& rhs)
        {
            Derived result(lhs);
            result.x -= rhs.x;
            result.y -= rhs.y;
            return result;
        }

        /**
         * Vector multiplication
         * 向量乘法
         * @param lhs
         * @param scalar scalar 标量
         * @return
         */
        friend Derived operator*(const Derived& lhs, float scalar)
        {
            Derived result(lhs);
            result.x *= scalar;
            result.y *= scalar;
            return result;
        }

        friend Derived operator*(float scalar, const Derived& rhs)
        {
            return rhs * scalar;
        }

        /**
         * Vector division
         * 向量除法
         * @param lhs
         * @param scalar scalar 标量
         * @return
         */
        friend Derived operator/(const Derived& lhs, float scalar)
        {
            Derived result(lhs);
            result.x /= scalar;
            result.y /= scalar;
            return result;
        }

        /**
         * Vector plus vector
         * 向量加向量
         * @param rhs
         * @return
         */
        Derived& operator+=(const Derived& rhs);

        /**
         * Vector subtraction
         * 向量减向量
         * @param rhs
         * @return
         */
        Derived& operator-=(const Derived& rhs);

        /**
         *
         * @param rhs
         * @return
         */
        [[nodiscard]] float Dot(const Derived& rhs) const;

        /**
         * Length
         * 获取向量长度
         * @return
         */
        [[nodiscard]] float Length() const;

        /**
         * LengthSquared(Squared)
         * 获取向量长度（不开方）
         * @return
         */
        [[nodiscard]] float LengthSquared() const;

        /**
         * Calculate the distance of the vector
         * 获取向量的距离
         * @param rhs
         * @return
         */
        [[nodiscard]] float Distance(const Derived& rhs) const;

        /**
         * Calculate the distance of the vector without taking the square root.
         * 获取向量的距离不开方
         * @param rhs
         * @return
         */
        [[nodiscard]] float DistanceSquared(const Derived& rhs) const;

        /**
         * Normalized
         * 向量归一化
         * @return
         */
        [[nodiscard]] Derived Normalized() const;

        [[nodiscard]] float ToAngle(const Derived& to) const;


        [[nodiscard]] Vector2DFingerprint GetFingerprint() const override;
    };

    template <typename Derived>
    Vector2DBase<Derived>::Vector2DBase(const float tempX, const float tempY)
        : x(tempX), y(tempY)
    {
    }

    template <typename Derived>
    Derived& Vector2DBase<Derived>::operator+=(const Derived& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return static_cast<Derived&>(*this);
    }

    template <typename Derived>
    Derived& Vector2DBase<Derived>::operator-=(const Derived& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return static_cast<Derived&>(*this);
    }

    template <typename Derived>
    float Vector2DBase<Derived>::Dot(const Derived& rhs) const
    {
        return x * rhs.x + y * rhs.y;
    }

    template <typename Derived>
    float Vector2DBase<Derived>::Length() const
    {
        return std::sqrt(LengthSquared());
    }

    template <typename Derived>
    float Vector2DBase<Derived>::LengthSquared() const
    {
        return x * x + y * y;
    }

    template <typename Derived>
    float Vector2DBase<Derived>::Distance(const Derived& rhs) const
    {
        auto self = static_cast<const Derived&>(*this);
        return (self - rhs).Length();
    }

    template <typename Derived>
    float Vector2DBase<Derived>::DistanceSquared(const Derived& rhs) const
    {
        auto self = static_cast<const Derived&>(*this);
        return (self - rhs).LengthSquared();
    }

    template <typename Derived>
    Derived Vector2DBase<Derived>::Normalized() const
    {
        const float len = LengthSquared();
        if (len < 0.0001F)
        {
            return Derived(0, 0);
        }
        return Derived(x / len, y / len);
    }

    template <typename Derived>
    float Vector2DBase<Derived>::ToAngle(const Derived& to) const
    {
        const auto dx = static_cast<float>(to.x - x);
        const auto dy = static_cast<float>(to.y - y);
        const float ang = std::atan2(dy, dx) * 180.0F / std::numbers::pi_v<float>;
        return ang < 0 ? ang + 360.0F : ang;
    }

    template <typename Derived>
    Vector2DFingerprint Vector2DBase<Derived>::GetFingerprint() const
    {
        const auto bitsX = std::bit_cast<uint32_t>(x);
        const auto bitsY = std::bit_cast<uint32_t>(y);
        return static_cast<uint64_t>(bitsX) << 32 | bitsY;
    }
}
