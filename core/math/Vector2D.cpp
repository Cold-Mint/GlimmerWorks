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
#include "Vector2D.h"

#include <cmath>
#include <bit>
#include "Vector2DI.h"

glimmer::Vector2D::Vector2D() : x(0.0F), y(0.0F) {
}

glimmer::Vector2D::Vector2D(const float x, const float y) : x(x), y(y) {
}

glimmer::Vector2D::Vector2D(const int x, const int y) : x(static_cast<float>(x)), y(static_cast<float>(y)) {
}

uint64_t glimmer::Vector2D::GetFingerprint() const {
    const uint32_t bitsX = std::bit_cast<uint32_t>(x);
    const uint32_t bitsY = std::bit_cast<uint32_t>(y);
    return static_cast<uint64_t>(bitsX) << 32 | bitsY;
}

glimmer::Vector2D glimmer::Vector2D::FromFingerprint(Vector2DFingerprint fingerprint) {
    const uint32_t bitsX = static_cast<uint32_t>(fingerprint >> 32);
    const uint32_t bitsY = static_cast<uint32_t>(fingerprint & 0xFFFFFFFF);
    const float x = std::bit_cast<float>(bitsX);
    const float y = std::bit_cast<float>(bitsY);
    return {x, y};
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

float glimmer::Vector2D::ToAngle(const Vector2D &to) const {
    const float dx = to.x - x;
    const float dy = to.y - y;
    const float ang = std::atan2(dy, dx) * 180.0f / static_cast<float>(M_PI);
    return ang < 0 ? ang + 360.0f : ang;
}

glimmer::Vector2DI glimmer::Vector2D::ToInt() const {
    return {static_cast<int>(x), static_cast<int>(y)};
}
