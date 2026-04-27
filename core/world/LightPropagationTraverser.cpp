//
// Created by coldmint on 2026/4/11.
//

#include "LightPropagationTraverser.h"

#include <queue>
#include <unordered_set>


void glimmer::LightPropagationTraverser::PropagateSingleRayImpl(const int rayIndex,
                                                                std::unordered_set<TileVector2D, Vector2DIHash> &
                                                                visited, const int maxRadSq) const {
    const float angleRadians = static_cast<float>(rayIndex) * rayAngleStep_ * static_cast<float>(M_PI) / 180.0F;
    const float dirX = std::cos(angleRadians);
    const float dirY = std::sin(angleRadians);
    auto currentX = static_cast<float>(center_.x);
    auto currentY = static_cast<float>(center_.y);
    TileVector2D currentTile = center_;
    bool centerOfCircle = true;
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
            //Here, the intercepted rays exceed the radius. This is necessary.
            //这里拦截射线超过半径。必须要的。
            break;
        }
        if (!visited.contains(nextTile)) {
            const TraverseAction action = stepCallback_(currentTile, nextTile, centerOfCircle, rayIndex);
            if (action == TraverseAction::StopAll) {
                return;
            }
            if (action == TraverseAction::SkipDirection) {
                break;
            }

            visited.insert(nextTile);
        }

        currentTile = nextTile;
        centerOfCircle = false;
    }
}

void glimmer::LightPropagationTraverser::HandleRadiusIsZero() const {
    (void) stepCallback_(center_, center_, true, LIGHT_CONTRIBUTION_CENTER_RAY_INDEX);
}


glimmer::LightPropagationTraverser::LightPropagationTraverser(const TileVector2D center, const int maxRadius,
                                                              const std::function<TraverseAction(
                                                                  TileVector2D current, TileVector2D next,
                                                                  bool centerOfCircle, int reyIndex)> &
                                                              stepCallback) : stepCallback_(
                                                                                  stepCallback), maxRadius_(maxRadius),
                                                                              center_(center) {
    if (maxRadius > 0) {
        const int index = maxRadius - 1;
        rayCount_ = RAY_COUNT[index];
        rayAngleStep_ = ANGLE_STEPS[index];
    } else {
        rayCount_ = 0;
        rayAngleStep_ = 0.0F;
    }
}

void glimmer::LightPropagationTraverser::PropagateAllRays() const {
    if (stepCallback_ == nullptr) {
        return;
    }

    if (maxRadius_ > 0) {
        std::unordered_set<TileVector2D, Vector2DIHash> visited;
        const int maxRadSq = maxRadius_ * maxRadius_;
        for (int rayIndex = 0; rayIndex < rayCount_; ++rayIndex) {
            PropagateSingleRayImpl(rayIndex, visited, maxRadSq);
        }
    } else {
        HandleRadiusIsZero();
    }
}

void glimmer::LightPropagationTraverser::PropagateSingleRay(const int rayIndex) const {
    if (stepCallback_ == nullptr) {
        return;
    }
    if (maxRadius_ > 0) {
        std::unordered_set<TileVector2D, Vector2DIHash> visited;
        PropagateSingleRayImpl(rayIndex, visited, maxRadius_ * maxRadius_);
    } else {
        HandleRadiusIsZero();
    }
}
