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
#include "box2d/math_functions.h"
#include "core/math/WorldVector2D.h"

namespace glimmer {
    class Box2DUtils {
    public:
        /**
         * Convert meters to pixels
         * 将米转换为像素
         * @param meters meters 米
         * @return Pixel 像素
         */
        static float ToPixels(float meters);

        /**
         * Convert pixels to meters
         * 将像素转换为米
         * @param pixels pixels 像素
         * @return Meter 米
         */
        static float ToMeters(float pixels);


        /**
         * Convert Box2D Vec2 to Vector2D
         * 将Box2D Vec2转换为Vector2D
         * @param vec2 vec2 Box2D Vec2
         * @return Vector2D Vector2D
         */
        static WorldVector2D ToPixels(b2Vec2 vec2);

        /**
         * Convert Vector2D to Box2D Vec2
         * 将Vector2D转换为Box2D Vec2
         * @param vector2D vector2D Vector2D
         * @return b2Vec2 b2Vec2
         */
        static b2Vec2 ToMeters(const WorldVector2D& vector2D);
    };
}
