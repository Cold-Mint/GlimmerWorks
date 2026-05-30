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
#include <cmath>

#include "core/IFingerprintable.h"
#include "src/core/vector2di.pb.h"

namespace glimmer
{
    using Vector2DIFingerprint = uint64_t;

    template <typename Derived>
    class Vector2DIBase : public IFingerprintable
    {
    public:
        int x = 0.0F;
        int y = 0.0F;
        Vector2DIBase() = default;

        Vector2DIBase(int tempX, int tempY);

        void ReadVector2DIMessage(const Vector2DIMessage& vector2di);

        void WriteVector2DIMessage(Vector2DIMessage& vector2di) const;


        /**
         * Vector addition
         * 向量加法
         * @param rhs
         * @return
         */
        [[nodiscard]] Derived operator+(const Derived& rhs) const;

        /**
         * Vector subtraction
         * 向量减法
         * @param rhs
         * @return
         */
        [[nodiscard]] Derived operator-(const Derived& rhs) const;

        /**
         * Vector multiplication
         * 向量乘法
         * @param scalar scalar 标量
         * @return
         */
        [[nodiscard]] Derived operator*(float scalar) const;

        /**
         * Vector division
         * 向量除法
         * @param scalar scalar 标量
         * @return
         */
        [[nodiscard]] Derived operator/(float scalar) const;

        /**
         * Vector plus vector
         * 向量加向量
         * @param rhs
         * @return
         */
        Derived& operator+=(const Derived& rhs);

        [[nodiscard]] bool operator==(const Derived& rhs) const noexcept;

        [[nodiscard]] bool operator!=(const Derived& rhs) const noexcept;

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
        [[nodiscard]] uint32_t Length() const;

        /**
         * LengthSquared(Squared)
         * 获取向量长度（不开方）
         * @return
         */
        [[nodiscard]] uint32_t LengthSquared() const;

        /**
         * Calculate the distance of the vector
         * 获取向量的距离
         * @param rhs
         * @return
         */
        [[nodiscard]] uint32_t Distance(const Derived& rhs) const;

        /**
         * Calculate the distance of the vector without taking the square root.
         * 获取向量的距离不开方
         * @param rhs
         * @return
         */
        [[nodiscard]] uint32_t DistanceSquared(const Derived& rhs) const;

        /**
         * Normalized
         * 向量归一化
         * @return
         */
        [[nodiscard]] Derived Normalized() const;

        /**
        * Calculate the angle between this vector and another vector.
        * 计算到另一个向量之间的角度。
        * @param to to
        * @return Between 0 and 360 degrees. 0-360度之间。
        */
        [[nodiscard]] float ToAngle(const Derived& to) const;

        [[nodiscard]] Vector2DIFingerprint GetFingerprint() const override;
    };

    template <typename Derived>
    Vector2DIBase<Derived>::Vector2DIBase(int tempX, int tempY)
    {
        x = tempX;
        y = tempY;
    }

    template <typename Derived>
    void Vector2DIBase<Derived>::ReadVector2DIMessage(const Vector2DIMessage& vector2di)
    {
        x = vector2di.x();
        y = vector2di.y();
    }

    template <typename Derived>
    void Vector2DIBase<Derived>::WriteVector2DIMessage(Vector2DIMessage& vector2di) const
    {
        vector2di.set_x(x);
        vector2di.set_y(y);
    }


    template <typename Derived>
    Derived Vector2DIBase<Derived>::operator+(const Derived& rhs) const
    {
        return Derived(x + rhs.x, y + rhs.y);
    }

    template <typename Derived>
    Derived Vector2DIBase<Derived>::operator-(const Derived& rhs) const
    {
        return Derived(x - rhs.x, y - rhs.y);
    }

    template <typename Derived>
    Derived Vector2DIBase<Derived>::operator*(const float scalar) const
    {
        return Derived(x * scalar, y * scalar);
    }

    template <typename Derived>
    Derived Vector2DIBase<Derived>::operator/(const float scalar) const
    {
        return Derived(x / scalar, y / scalar);
    }

    template <typename Derived>
    Derived& Vector2DIBase<Derived>::operator+=(const Derived& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return static_cast<Derived&>(*this);
    }

    template <typename Derived>
    bool Vector2DIBase<Derived>::operator==(const Derived& rhs) const noexcept
    {
        return rhs.x == x && rhs.y == y;
    }

    template <typename Derived>
    bool Vector2DIBase<Derived>::operator!=(const Derived& rhs) const noexcept
    {
        return rhs.x != x || rhs.y != y;
    }

    template <typename Derived>
    Derived& Vector2DIBase<Derived>::operator-=(const Derived& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return static_cast<Derived&>(*this);
    }

    template <typename Derived>
    float Vector2DIBase<Derived>::Dot(const Derived& rhs) const
    {
        return x * rhs.x + y * rhs.y;
    }

    template <typename Derived>
    uint32_t Vector2DIBase<Derived>::Length() const
    {
        return static_cast<uint32_t>(std::sqrt(static_cast<float>(LengthSquared())));
    }

    template <typename Derived>
    uint32_t Vector2DIBase<Derived>::LengthSquared() const
    {
        return x * x + y * y;
    }

    template <typename Derived>
    uint32_t Vector2DIBase<Derived>::Distance(const Derived& rhs) const
    {
        return (*this - rhs).Length();
    }

    template <typename Derived>
    uint32_t Vector2DIBase<Derived>::DistanceSquared(const Derived& rhs) const
    {
        return (*this - rhs).LengthSquared();
    }

    template <typename Derived>
    Derived Vector2DIBase<Derived>::Normalized() const
    {
        const uint32_t lenSq = LengthSquared();
        if (lenSq < 1U)
        {
            return Derived(0, 0);
        }
        const float len = std::sqrt(static_cast<float>(lenSq));
        return Derived(static_cast<int>(static_cast<float>(x) / len),
                       static_cast<int>(static_cast<float>(y) / len));
    }

    template <typename Derived>
    float Vector2DIBase<Derived>::ToAngle(const Derived& to) const
    {
        const auto dx = static_cast<float>(to.x - x);
        const auto dy = static_cast<float>(to.y - y);
        const float ang = std::atan2(dy, dx) * 180.0F / static_cast<float>(M_PI);
        return ang < 0 ? ang + 360.0F : ang;
    }

    template <typename Derived>
    Vector2DIFingerprint Vector2DIBase<Derived>::GetFingerprint() const
    {
        const auto bitsX = std::bit_cast<uint32_t>(x);
        const auto bitsY = std::bit_cast<uint32_t>(y);
        return static_cast<uint64_t>(bitsX) << 32 | bitsY;
    }
}
