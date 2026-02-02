//
// Created by Cold-Mint on 2025/11/7.
//

#include "Box2DUtils.h"
#include "core/Constants.h"

float glimmer::Box2DUtils::ToPixels(const float meters) { return meters * KSCALE; }

float glimmer::Box2DUtils::ToMeters(const float pixels) { return pixels * KINV_SCALE; }

glimmer::Vector2D glimmer::Box2DUtils::ToPixels(const b2Vec2 vec2) {
    return {ToPixels(vec2.x), ToPixels(vec2.y)};
}

b2Vec2 glimmer::Box2DUtils::ToMeters(const Vector2D vector2D) {
    return {ToMeters(vector2D.x), ToMeters(vector2D.y)};
}
