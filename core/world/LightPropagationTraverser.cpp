//
// Created by coldmint on 2026/4/11.
//

#include "LightPropagationTraverser.h"

#include <queue>
#include <unordered_set>


glimmer::LightPropagationTraverser::LightPropagationTraverser(
    const int maxRadius,
    const std::function<TraverseAction(TileVector2D, TileVector2D)> &stepCallback,
    const TileVector2D center)
    : maxRadius_(maxRadius), stepCallback_(stepCallback), center_(center) {
    if (maxRadius_ > 0) {
        rayCount_ = RAY_COUNT[maxRadius - 1];
        rayAngleStep_ = ANGLE_STEPS[maxRadius - 1];
    }
}


/**
 * 游走时的回调函数，TileVector2D表示当前游走坐标，返回bool，如果返回true，则终止游走当前方向。
 *  distance The distance from the current point to the center point
 *  distance 当前点距离中心点的距离
 */
void glimmer::LightPropagationTraverser::Start() const {
    if (maxRadius_ <= 0 || stepCallback_ == nullptr) {
        return;
    }

    std::unordered_set<TileVector2D, Vector2DIHash> visited;
    const int maxRadSq = maxRadius_ * maxRadius_;
    for (int rayIndex = 0; rayIndex < rayCount_; ++rayIndex) {
        const float angleRadians = static_cast<float>(rayIndex) * rayAngleStep_ * static_cast<float>(M_PI) / 180.0F;
        const float dirX = std::cos(angleRadians);
        const float dirY = std::sin(angleRadians);
        auto currentX = static_cast<float>(center_.x);
        auto currentY = static_cast<float>(center_.y);
        TileVector2D currentTile = center_;
        for (int step = 0; step < maxRadius_; ++step) {
            currentX += dirX;
            currentY += dirY;
            const int nextX = static_cast<int>(std::round(currentX));
            const int nextY = static_cast<int>(std::round(currentY));
            const TileVector2D nextTile = {nextX, nextY};
            const int dx = nextTile.x - center_.x;
            const int dy = nextTile.y - center_.y;
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
