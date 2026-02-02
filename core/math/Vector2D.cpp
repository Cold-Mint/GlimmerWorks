//
// Created by Cold-Mint on 2025/10/24.
//

#include "Vector2D.h"

#include <cmath>
#include "Vector2DI.h"

glimmer::Vector2D::Vector2D() : x(0.0F), y(0.0F) {
}

glimmer::Vector2D::Vector2D(const float x, const float y) : x(x), y(y) {
}

glimmer::Vector2D::Vector2D(const int x, const int y) : x(static_cast<float>(x)), y(static_cast<float>(y)) {
}

glimmer::Vector2D glimmer::Vector2D::operator+(const Vector2D &rhs) const {
    return {x + rhs.x, y + rhs.y};
}

glimmer::Vector2D glimmer::Vector2D::operator-(const Vector2D &rhs) const {
    return {x - rhs.x, y - rhs.y};
}

glimmer::Vector2D glimmer::Vector2D::operator*(const float scalar) const {
    return {x * scalar, y * scalar};
}

glimmer::Vector2D glimmer::Vector2D::operator/(const float scalar) const {
    return {x / scalar, y / scalar};
}

glimmer::Vector2D &glimmer::Vector2D::operator+=(const Vector2D &rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
}

glimmer::Vector2D &glimmer::Vector2D::operator-=(const Vector2D &rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

float glimmer::Vector2D::dot(const Vector2D &rhs) const {
    return x * rhs.x + y * rhs.y;
}

float glimmer::Vector2D::Length() const {
    return std::sqrt(x * x + y * y);
}

float glimmer::Vector2D::LengthSquared() const {
    return x * x + y * y;
}

float glimmer::Vector2D::Distance(const Vector2D &rhs) const {
    const float dx = x - rhs.x;
    const float dy = y - rhs.y;
    return std::sqrt(dx * dx + dy * dy);
}

float glimmer::Vector2D::DistanceSquared(const Vector2D &rhs) const {
    const float dx = x - rhs.x;
    const float dy = y - rhs.y;
    return dx * dx + dy * dy;
}

glimmer::Vector2D glimmer::Vector2D::Normalized() const {
    const float len = Length();
    return len > 0.0F ? *this / len : Vector2D();
}

glimmer::Vector2DI glimmer::Vector2D::ToInt() const {
    return {static_cast<int>(x), static_cast<int>(y)};
}
