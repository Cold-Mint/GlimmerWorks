//
// Created by coldmint on 2026/4/11.
//

#include "LightPropagationTraverser.h"

#include <queue>
#include <unordered_set>


glimmer::LightPropagationTraverser::LightPropagationTraverser(
    const int maxRadius,
    const std::function<TraverseAction(TileVector2D, TileVector2D, float)> &stepCallback,
    const TileVector2D center)
    : maxRadius_(maxRadius), stepCallback_(stepCallback), center_(center) {
    if (maxRadius_ > 1) {
        rayCount_ = static_cast<int>(std::ceil(2.0 * M_PI * maxRadius_));
        rayAngleStep_ = 360.0F / static_cast<float>(rayCount_);
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
    const auto maxRadSq = static_cast<float>(maxRadius_ * maxRadius_);
    for (int rayIndex = 0; rayIndex < rayCount_; ++rayIndex) {
        const float angleRadians = static_cast<float>(rayIndex) * rayAngleStep_ * static_cast<float>(M_PI) / 180.0F;
        const float dirX = std::cos(angleRadians);
        const float dirY = std::sin(angleRadians);
        TileVector2D currentTile = center_;
        for (int stepDist = 1; stepDist <= maxRadius_; ++stepDist) {
            const float stepX = dirX * static_cast<float>(stepDist);
            const float stepY = dirY * static_cast<float>(stepDist);
            const float idealDistSq = stepX * stepX + stepY * stepY;
            if (idealDistSq > maxRadSq) {
                break;
            }
            const float worldX = static_cast<float>(center_.x) + stepX;
            const float worldY = static_cast<float>(center_.y) + stepY;
            const int nextX = static_cast<int>(std::round(worldX));
            const int nextY = static_cast<int>(std::round(worldY));
            const TileVector2D nextTile = { nextX, nextY };
            const float realDist = std::sqrt(idealDistSq);
            const TraverseAction action = stepCallback_(currentTile, nextTile, realDist);
            if (action == TraverseAction::StopAll) {
                return;
            }
            if (action == TraverseAction::SkipDirection) {
                break;
            }
            visited.insert(nextTile);
            currentTile = nextTile;
        }
    }

}
