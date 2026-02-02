//
// Created by Cold-Mint on 2025/11/3.
//

#include "Vector2DI.h"
#include "src/core/vector2di.pb.h"

glimmer::Vector2DI::Vector2DI() : x(0), y(0) {
}

glimmer::Vector2DI::Vector2DI(const int x, const int y) : x(x), y(y) {
}

glimmer::Vector2DI::Vector2DI(const Vector2D &v)
    : x(static_cast<int>(std::floor(v.x))),
      y(static_cast<int>(std::floor(v.y))) {
}

void glimmer::Vector2DI::FromMessage(const Vector2DIMessage &vector2di) {
    x = vector2di.x();
    y = vector2di.y();
}

void glimmer::Vector2DI::ToMessage(Vector2DIMessage &vector2di) const {
    vector2di.set_x(x);
    vector2di.set_y(y);
}

glimmer::Vector2DI glimmer::Vector2DI::operator+(const Vector2DI &rhs) const {
    return {x + rhs.x, y + rhs.y};
}

glimmer::Vector2DI glimmer::Vector2DI::operator-(const Vector2DI &rhs) const {
    return {x - rhs.x, y - rhs.y};
}

glimmer::Vector2DI glimmer::Vector2DI::operator*(const int scalar) const {
    return {x * scalar, y * scalar};
}

glimmer::Vector2DI glimmer::Vector2DI::operator/(const int scalar) const {
    return {x / scalar, y / scalar};
}

glimmer::Vector2DI &glimmer::Vector2DI::operator+=(const Vector2DI &rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
}

glimmer::Vector2DI &glimmer::Vector2DI::operator-=(const Vector2DI &rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

bool glimmer::Vector2DI::operator==(const Vector2DI &rhs) const {
    return x == rhs.x && y == rhs.y;
}

bool glimmer::Vector2DI::operator!=(const Vector2DI &rhs) const {
    return !(*this == rhs);
}

int glimmer::Vector2DI::LengthSquared() const {
    return x * x + y * y;
}

int glimmer::Vector2DI::DistanceSquared(const Vector2DI &rhs) const {
    const int dx = x - rhs.x;
    const int dy = y - rhs.y;
    return dx * dx + dy * dy;
}

glimmer::Vector2D glimmer::Vector2DI::ToFloat() const {
    return {static_cast<float>(x), static_cast<float>(y)};
}
