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
#include "Vector2DI.h"
#ifndef UNIT_TEST_MODE
#include "src/core/vector2di.pb.h"
#endif
glimmer::Vector2DI::Vector2DI() : x(0), y(0) {
}

glimmer::Vector2DI::Vector2DI(const int x, const int y) : x(x), y(y) {
}

glimmer::Vector2DI::Vector2DI(const Vector2D &v)
    : x(static_cast<int>(std::floor(v.x))),
      y(static_cast<int>(std::floor(v.y))) {
}

void glimmer::Vector2DI::ReadVector2DIMessage(const Vector2DIMessage &vector2di) {
#ifndef UNIT_TEST_MODE
    x = vector2di.x();
    y = vector2di.y();
#endif
}

void glimmer::Vector2DI::WriteVector2DIMessage(Vector2DIMessage &vector2di) const {
#ifndef UNIT_TEST_MODE
    vector2di.set_x(x);
    vector2di.set_y(y);
#endif
}

uint64_t glimmer::Vector2DI::GetFingerprint() const {
    const uint64_t high = static_cast<uint32_t>(x);
    const uint64_t low = static_cast<uint32_t>(y);
    return high << 32 | low;
}

glimmer::Vector2DI glimmer::Vector2DI::FromFingerprint(const Vector2DIFingerprint fingerprint) {
    return Vector2DI(static_cast<int>(static_cast<uint32_t>(fingerprint >> 32)),
                     static_cast<int>(static_cast<uint32_t>(fingerprint & 0xFFFFFFFF)));
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

uint32_t glimmer::Vector2DI::DistanceSquared(const Vector2DI &rhs) const {
    const int dx = x - rhs.x;
    const int dy = y - rhs.y;
    return dx * dx + dy * dy;
}

float glimmer::Vector2DI::ToAngle(const Vector2DI &to) const {
    const auto dx = static_cast<float>(to.x - x);
    const auto dy = static_cast<float>(to.y - y);
    float ang = std::atan2(dy, dx) * 180.0F / static_cast<float>(M_PI);
    return ang < 0 ? ang + 360.0F : ang;
}


glimmer::Vector2D glimmer::Vector2DI::ToFloat() const {
    return {static_cast<float>(x), static_cast<float>(y)};
}
