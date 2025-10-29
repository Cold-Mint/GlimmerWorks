//
// Created by Cold-Mint on 2025/10/24.
//

#ifndef GLIMMERWORKS_VECTOR2D_H
#define GLIMMERWORKS_VECTOR2D_H
#include <cmath>


namespace glimmer
{
    struct Vector2D
    {
        float x;
        float y;

        /**
         * Default constructor
         * 默认构造函数
         * Initializes x and y to 0.0f
         * 初始化 x 和 y 为 0.0f
         */
        Vector2D() : x(0.0f), y(0.0f)
        {
        }

        /**
         * Construct a vector with given components
         * 使用指定分量构造向量
         * @param x X component
         * @param y Y component
         * @param x X 分量
         * @param y Y 分量
         */
        Vector2D(const float x, const float y) : x(x), y(y)
        {
        }

        Vector2D(const int x, const int y) : x(static_cast<float>(x)), y(static_cast<float>(y))
        {
        }

        /**
         * Vector addition
         * 向量加法
         * @param rhs Right-hand side vector 右侧向量
         * @return A new vector that is the sum of this and rhs 加法结果向量
         */
        Vector2D operator+(const Vector2D& rhs) const
        {
            return {x + rhs.x, y + rhs.y};
        }

        /**
         * Vector subtraction
         * 向量减法
         * @param rhs Right-hand side vector 右侧向量
         * @return A new vector that is the difference of this and rhs 减法结果向量
         */
        Vector2D operator-(const Vector2D& rhs) const
        {
            return {x - rhs.x, y - rhs.y};
        }

        /**
         * Scalar multiplication
         * 向量与标量相乘
         * @param scalar Scalar value 标量值
         * @return A new vector scaled by the given scalar 乘以标量后的新向量
         */
        Vector2D operator*(const float scalar) const
        {
            return {x * scalar, y * scalar};
        }

        /**
         * Scalar division
         * 向量除以标量
         * @param scalar Scalar value 标量值
         * @return A new vector divided by the given scalar 除以标量后的新向量
         */
        Vector2D operator/(const float scalar) const
        {
            return {x / scalar, y / scalar};
        }

        /**
         * Add another vector to this one
         * 向量自加
         * @param rhs Right-hand side vector 右侧向量
         * @return Reference to this vector after addition 加法后的自身引用
         */
        Vector2D& operator+=(const Vector2D& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        /**
         * Subtract another vector from this one
         * 向量自减
         * @param rhs Right-hand side vector 右侧向量
         * @return Reference to this vector after subtraction 减法后的自身引用
         */
        Vector2D& operator-=(const Vector2D& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }

        /**
         * Compute the dot product of two vectors
         * 计算两个向量的点积
         * @param rhs Right-hand side vector 右侧向量
         * @return The dot product result 点积结果
         */
        [[nodiscard]] float dot(const Vector2D& rhs) const
        {
            return x * rhs.x + y * rhs.y;
        }

        /**
         * Compute the length (magnitude) of the vector  计算向量的长度（模）
         * @return The vector length 向量的长度
         */
        [[nodiscard]] float length() const
        {
            return std::sqrt(x * x + y * y);
        }

        /**
        * Compute the squared length of the vector (faster, no sqrt)  计算向量长度的平方（更高性能，无需开方）
        * @return The squared vector length  向量长度的平方
        */
        [[nodiscard]] float LengthSquared() const
        {
            return x * x + y * y;
        }

        /**
         * Compute the distance to another vector 计算到另一个向量的距离
         * @param rhs Target vector 目标向量
         * @return The distance between the two vectors 两个向量之间的距离
         */
        [[nodiscard]] float Distance(const Vector2D& rhs) const
        {
            const float dx = x - rhs.x;
            const float dy = y - rhs.y;
            return std::sqrt(dx * dx + dy * dy);
        }

        /**
         * Compute the squared distance to another vector (faster, no sqrt)   计算到另一个向量的距离平方（更高性能，无需开方）
         * @param rhs Target vector  目标向量
         * @return The squared distance between the two vectors  两个向量之间的距离平方
         */
        [[nodiscard]] float DistanceSquared(const Vector2D& rhs) const
        {
            const float dx = x - rhs.x;
            const float dy = y - rhs.y;
            return dx * dx + dy * dy;
        }


        /**
         * Normalize the vector 归一化向量
         * @return A new normalized vector (length = 1) 单位化后的新向量（长度为1）
         */
        [[nodiscard]] Vector2D Normalized() const
        {
            const float len = length();
            return len > 0.0f ? *this / len : Vector2D();
        }
    };

    /**
     * Scalar multiplication (commutative) 标量与向量相乘（交换律）
     * @param scalar Scalar value 标量值
     * @param v Vector 向量
     * @return A new vector scaled by the given scalar 乘以标量后的新向量
     */
    inline Vector2D operator*(const float scalar, const Vector2D& v)
    {
        return v * scalar;
    }
}

#endif //GLIMMERWORKS_VECTOR2D_H
