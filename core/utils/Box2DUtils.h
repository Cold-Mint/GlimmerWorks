//
// Created by Cold-Mint on 2025/11/7.
//

#ifndef GLIMMERWORKS_BOX2DUTILS_H
#define GLIMMERWORKS_BOX2DUTILS_H
#include "../Constants.h"
#include "../math/Vector2D.h"
#include "box2d/math_functions.h"

namespace glimmer
{
    class Box2DUtils
    {
    public:
        /**
         * Convert meters to pixels
         * 将米转换为像素
         * @param meters meters 米
         * @return Pixel 像素
         */
        static float ToPixels(const float meters) { return meters * KSCALE; }
        /**
         * Convert pixels to meters
         * 将像素转换为米
         * @param pixels pixels 像素
         * @return Meter 米
         */
        static float ToMeters(const float pixels) { return pixels * KINV_SCALE; }


        /**
         * Convert Box2D Vec2 to Vector2D
         * 将Box2D Vec2转换为Vector2D
         * @param vec2 vec2 Box2D Vec2
         * @return Vector2D Vector2D
         */
        static Vector2D ToPixels(const b2Vec2 vec2)
        {
            return {ToPixels(vec2.x), ToPixels(vec2.y)};
        }

        /**
         * Convert Vector2D to Box2D Vec2
         * 将Vector2D转换为Box2D Vec2
         * @param vector2D vector2D Vector2D
         * @return b2Vec2 b2Vec2
         */
        static b2Vec2 ToMeters(const Vector2D vector2D)
        {
            return {ToMeters(vector2D.x), ToMeters(vector2D.y)};
        }
    };
}

#endif //GLIMMERWORKS_BOX2DUTILS_H
