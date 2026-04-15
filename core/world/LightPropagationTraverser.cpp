//
// Created by coldmint on 2026/4/11.
//

#include "LightPropagationTraverser.h"

#include <queue>
#include <unordered_set>


glimmer::LightPropagationTraverser::LightPropagationTraverser(LightSource *lightSource,
                                                              const std::function<TraverseAction(
                                                                  TileVector2D current,
                                                                  TileVector2D next)> &stepCallback) : lightSource_(
        lightSource), stepCallback_(stepCallback) {
    if (lightSource != nullptr && lightSource->maxRadius > 0) {
        const int index = lightSource->maxRadius - 1;
        rayCount_ = RAY_COUNT[index];
        rayAngleStep_ = ANGLE_STEPS[index];
    }
}

/**
 * 游走时的回调函数，TileVector2D表示当前游走坐标，返回bool，如果返回true，则终止游走当前方向。
 *  distance The distance from the current point to the center point
 *  distance 当前点距离中心点的距离
 */
void glimmer::LightPropagationTraverser::Start() const {
    if (lightSource_ == nullptr || lightSource_->maxRadius <= 0 || stepCallback_ == nullptr) {
        return;
    }

    std::unordered_set<TileVector2D, Vector2DIHash> visited;
    const TileVector2D center = lightSource_->center;
    const int maxRadius = lightSource_->maxRadius;
    const int maxRadSq = maxRadius * maxRadius;
    for (int rayIndex = 0; rayIndex < rayCount_; ++rayIndex) {
        const float angleRadians = static_cast<float>(rayIndex) * rayAngleStep_ * static_cast<float>(M_PI) / 180.0F;
        const float dirX = std::cos(angleRadians);
        const float dirY = std::sin(angleRadians);
        auto currentX = static_cast<float>(center.x);
        auto currentY = static_cast<float>(center.y);
        TileVector2D currentTile = center;
        for (int step = 0; step < maxRadius; ++step) {
            currentX += dirX;
            currentY += dirY;
            const int nextX = static_cast<int>(std::round(currentX));
            const int nextY = static_cast<int>(std::round(currentY));
            const TileVector2D nextTile = {nextX, nextY};
            const int dx = nextTile.x - center.x;
            const int dy = nextTile.y - center.y;
            const int distSq = dx * dx + dy * dy;
            if (distSq > maxRadSq) {
                break;
            }
            if (!visited.contains(nextTile)) {
                const TraverseAction action = stepCallback_(currentTile, nextTile);
                if (action == TraverseAction::StopAll) {
                    return;
                }
                if (action == TraverseAction::SkipDirection) {
                    break;
                }

                visited.insert(nextTile);
            }

            currentTile = nextTile;
        }
    }
}
