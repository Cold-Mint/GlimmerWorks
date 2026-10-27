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
#include "HorizontalPhysicsSolver.h"

#include <algorithm>
#include <cmath>

#include "core/config/Constants.h"
#include "core/log/LogCat.h"
#include "core/math/CoordinateTransformer.h"
#include "core/world/ChunkManager.h"
#include "core/world/Tile.h"
#include "core/world/generator/TileLayerType.h"

glimmer::SolveResult glimmer::HorizontalPhysicsSolver::SolveXDirection(const ChunkManager *chunkManager,
                                                                       const WorldAABB &box, float velocityX) {
    SolveResult result;
    if (chunkManager == nullptr) {
        LogCat::w(LogLabel::PHYSICS, std::source_location::current(), "physics_horizontal_chunk_manager_null",
                  "Horizontal physics solve: chunk manager is null, skip");
        return result;
    }

    //The displacement to attempt this tick (signed: positive = right, negative = left).
    //这一步想走的位移（有符号：正=向右，负=向左）。
    const float stepX = velocityX * FIXED_TIME_STEP;
    if (stepX == 0.0F) {
        LogCat::d(LogLabel::PHYSICS, "physics_horizontal_zero_step",
                  "Horizontal physics: zero step, skip");
        return result;
    }

    //Swept bounding box along X. Y is fixed during a horizontal sweep.
    //沿 X 的扫掠包围盒。水平扫掠时 Y 不变。
    const float xMin = std::min(box.Left(), box.Left() + stepX);
    const float xMax = std::max(box.Right(), box.Right() + stepX);
    const TileVector2D minTile = CoordinateTransformer::WorldToTile(WorldVector2D{xMin, box.Bottom()});
    const TileVector2D maxTile = CoordinateTransformer::WorldToTile(WorldVector2D{xMax, box.Top()});

    LogCat::d(LogLabel::PHYSICS, "physics_horizontal_sweep_start",
              "Horizontal physics sweep: velocityX={}, stepX={}, xRange=[{}, {}], tileRange=({}, {})..({}, {})",
              velocityX, stepX, xMin, xMax, minTile.x, minTile.y, maxTile.x, maxTile.y);

    //The farthest distance still allowed to travel (magnitude).
    //还允许行走的最远距离（大小）。
    float remaining = std::fabs(stepX);

    for (int ty = minTile.y; ty <= maxTile.y; ++ty) {
        for (int tx = minTile.x; tx <= maxTile.x; ++tx) {
            const Tile *tile = chunkManager->GetTileAt(TileLayerType::Ground, TileVector2D{tx, ty});
            if (tile == nullptr) {
                continue;
            }
            const uint8_t mask = tile->GetCollisionMask();
            if (mask == COLLISION_MASK_NONE) {
                continue;
            }

            //Tile collision box (configurable size/offset, default = full cell).
            //瓦片碰撞箱（可配置尺寸/偏移，默认整格）。
            const WorldAABB tileBox = tile->GetCollisionAABB(TileVector2D{tx, ty});
            if (tileBox.halfWidth <= 0.0F || tileBox.halfHeight <= 0.0F) {
                continue;
            }
            const float tileLeft = tileBox.Left();
            const float tileRight = tileBox.Right();
            const float tileBottom = tileBox.Bottom();
            const float tileTop = tileBox.Top();

            //Perpendicular (Y) overlap check: skip tiles outside the vertical lane.
            //垂直轴（Y）重叠检查：跳过不在竖直通道内的瓦片。
            if (box.Top() <= tileBottom || box.Bottom() >= tileTop) {
                continue;
            }

            //The face we approach depends on the horizontal direction.
            //逼近哪个面取决于水平方向。
            float dist = 0.0F;
            if (stepX > 0.0F) {
                //Moving right: the right edge approaches the tile's left face.
                //向右：右边缘逼近瓦片左面。
                if ((mask & COLLISION_MASK_LEFT) == 0) {
                    continue;
                }
                dist = tileLeft - box.Right();
            } else {
                //Moving left: the left edge approaches the tile's right face.
                //向左：左边缘逼近瓦片右面。
                if ((mask & COLLISION_MASK_RIGHT) == 0) {
                    continue;
                }
                dist = box.Left() - tileRight;
            }

            //Only approaching contacts (dist >= 0) are resolved. Already-overlapping tiles (dist < 0)
            //are skipped so the body can freely move back out of them.
            //只解算逼近接触（dist >= 0）。已重叠的瓦片（dist < 0）被跳过，让身体能自由走出。
            if (dist >= -PHYSICS_CONTACT_EPSILON && dist < remaining) {
                remaining = std::max(dist, 0.0F);
                result.blocked = true;
                result.hitTile = TileVector2D{tx, ty};
                LogCat::d(LogLabel::PHYSICS, "physics_horizontal_tile_hit",
                          "Horizontal physics: hit tile at ({}, {}), dist={}, remaining={}", tx, ty, dist, remaining);
            }
        }
    }

    //Move up to the closest contact, keeping the sign of the intended step.
    //走到最近的接触点，保留原步长的符号。
    result.distance = std::copysign(remaining, stepX);
    //The corrected horizontal velocity (zeroed when blocked).
    //修正后的水平速度（被挡则为 0）。
    result.velocity = result.blocked ? 0.0F : velocityX;

    LogCat::d(LogLabel::PHYSICS, "physics_horizontal_solve_result",
              "Horizontal physics done: distance={}, velocity={}, blocked={}, hitTile=({}, {})",
              result.distance, result.velocity, result.blocked, result.hitTile.x, result.hitTile.y);
    return result;
}
