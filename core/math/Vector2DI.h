//
// Created by Cold-Mint on 2025/11/3.
//

#pragma once
#include <cstdint>

#include "Vector2D.h"
#include "core/IFingerprintable.h"


class Vector2DIMessage;
using Vector2DIFingerprint = uint64_t;

namespace glimmer {
    struct Vector2DI : IFingerprintable {
        int x;
        int y;

        /**
         * Default constructor
         * 默认构造函数
         * Initializes x and y to 0
         * 初始化 x 和 y 为 0
         */
        Vector2DI();

        /**
         * Construct a vector with given components
         * 使用指定分量构造向量
         * @param x X component X分量
         * @param y Y component Y分量
         */
        Vector2DI(int x, int y);

        /**
         * Construct from floating vector (round down)
         * 从浮点向量构造（向下取整）
         */
        explicit Vector2DI(const Vector2D &v);

        void ReadVector2DIMessage(const Vector2DIMessage &vector2di);

        void WriteVector2DIMessage(Vector2DIMessage &vector2di) const;

        [[nodiscard]] Vector2DIFingerprint GetFingerprint() const override;

        [[nodiscard]] static Vector2DI FromFingerprint(Vector2DIFingerprint fingerprint);

        /**
         * Vector addition
         * 向量加法
         */
        [[nodiscard]] Vector2DI operator+(const Vector2DI &rhs) const;

        /**
         * Vector subtraction
         * 向量减法
         */
        [[nodiscard]] Vector2DI operator-(const Vector2DI &rhs) const;

        /**
         * Scalar multiplication
         * 向量与标量相乘
         */
        [[nodiscard]] Vector2DI operator*(int scalar) const;

        /**
         * Scalar division
         * 向量除以标量
         */
        [[nodiscard]] Vector2DI operator/(int scalar) const;

        /**
         * Compound addition assignment
         * 自加
         */
        Vector2DI &operator+=(const Vector2DI &rhs);

        /**
         * Compound subtraction assignment
         * 自减
         */
        Vector2DI &operator-=(const Vector2DI &rhs);

        /**
         * Equality comparison
         * 相等比较
         */
        [[nodiscard]] bool operator==(const Vector2DI &rhs) const;

        /**
         * Inequality comparison
         * 不相等比较
         */
        [[nodiscard]] bool operator!=(const Vector2DI &rhs) const;

        /**
         * Compute squared length (no sqrt)
         * 计算向量长度平方（无需开方）
         */
        [[nodiscard]] int LengthSquared() const;

        /**
         * Compute distance squared to another vector
         * 计算到另一个向量的距离平方
         */
        [[nodiscard]] uint32_t DistanceSquared(const Vector2DI &rhs) const;

        /**
         * Calculate the angle between this vector and another vector.
         * 计算到另一个向量之间的角度。
         * @param to to
         * @return Between 0 and 360 degrees. 0-360度之间。
         */
        [[nodiscard]] float ToAngle(const Vector2DI &to) const;

        /**
         * Convert to floating vector
         * 转换为浮点向量
         */
        [[nodiscard]] Vector2D ToFloat() const;
    };

    /**
     * Scalar multiplication (commutative)
     * 标量与向量相乘（交换律）
     */
    inline Vector2DI operator*(const int scalar, const Vector2DI &v) {
        return v * scalar;
    }

    /**
     * @brief 哈希函数结构体，用于 TileVector2D 作为 unordered_map / unordered_set 的 key
     *
     * unordered_map/unordered_set 需要 key 可以生成哈希值。
     * TileVector2DHash 提供了一个自定义哈希算法，将 TileVector2D 的 x 和 y 坐标映射为一个 size_t 值。
     */
    struct Vector2DIHash {
        std::size_t operator()(const Vector2DI &v) const noexcept {
            return v.GetFingerprint();
        }
    };
}

using TileVector2D = glimmer::Vector2DI;
