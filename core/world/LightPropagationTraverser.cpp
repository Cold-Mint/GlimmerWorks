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
#include "LightPropagationTraverser.h"

#include <cmath>
#include <queue>
#include <unordered_set>


void glimmer::LightPropagationTraverser::PropagateSingleRayImpl(const int rayIndex,
                                                                std::unordered_set<TileVector2D, Vector2DIHash> &
                                                                visited, const int maxRadSq) const {
    const float angleRadians = static_cast<float>(rayIndex) * rayAngleStep_ * 3.14159265358979323846F / 180.0F;
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
        //Clangd: In template: invalid operands to binary expression ('const glimmer::TileVector2D' and 'const glimmer::TileVector2D')
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
