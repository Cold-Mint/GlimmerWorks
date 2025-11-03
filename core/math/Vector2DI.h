//
// Created by Cold-Mint on 2025/11/3.
//

#ifndef GLIMMERWORKS_VECTOR2DI_H
#define GLIMMERWORKS_VECTOR2DI_H
#include <cmath>

#include "Vector2D.h"

namespace glimmer
{
    struct Vector2DI
    {
        int x;
        int y;

        /**
         * Default constructor
         * 默认构造函数
         * Initializes x and y to 0
         * 初始化 x 和 y 为 0
         */
        Vector2DI() : x(0), y(0)
        {
        }

        /**
         * Construct a vector with given components
         * 使用指定分量构造向量
         * @param x X component X分量
         * @param y Y component Y分量
         */
        Vector2DI(const int x, const int y) : x(x), y(y)
        {
        }

        /**
         * Construct from floating vector (round down)
         * 从浮点向量构造（向下取整）
         */
        explicit Vector2DI(const Vector2D& v)
            : x(static_cast<int>(std::floor(v.x))),
              y(static_cast<int>(std::floor(v.y)))
        {
        }

        /**
         * Vector addition
         * 向量加法
         */
        [[nodiscard]] Vector2DI operator+(const Vector2DI& rhs) const;
        /**
         * Vector subtraction
         * 向量减法
         */
        [[nodiscard]] Vector2DI operator-(const Vector2DI& rhs) const;

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
        Vector2DI& operator+=(const Vector2DI& rhs);

        /**
         * Compound subtraction assignment
         * 自减
         */
        Vector2DI& operator-=(const Vector2DI& rhs);

        /**
         * Equality comparison
         * 相等比较
         */
        [[nodiscard]] bool operator==(const Vector2DI& rhs) const;

        /**
         * Inequality comparison
         * 不相等比较
         */
        [[nodiscard]] bool operator!=(const Vector2DI& rhs) const;

        /**
         * Compute squared length (no sqrt)
         * 计算向量长度平方（无需开方）
         */
        [[nodiscard]] int LengthSquared() const;

        /**
         * Compute distance squared to another vector
         * 计算到另一个向量的距离平方
         */
        [[nodiscard]] int DistanceSquared(const Vector2DI& rhs) const;

        /**
         * Convert to floating vector
         * 转换为浮点向量
         */
        [[nodiscard]] Vector2D ToFloat() const;
    };

    typedef Vector2DI TileVector2D;

    /**
     * Scalar multiplication (commutative)
     * 标量与向量相乘（交换律）
     */
    inline Vector2DI operator*(const int scalar, const Vector2DI& v)
    {
        return v * scalar;
    }
}

#endif //GLIMMERWORKS_VECTOR2DI_H
