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
#include "VerticalPhysicsSolver.h"

#include <algorithm>
#include <cmath>

#include "core/config/Constants.h"
#include "core/log/LogCat.h"
#include "core/math/CoordinateTransformer.h"
#include "core/world/ChunkManager.h"
#include "core/world/Tile.h"
#include "core/world/generator/TileLayerType.h"

glimmer::SolveResult glimmer::VerticalPhysicsSolver::SolveYDirection(const ChunkManager *chunkManager,
                                                                     const WorldAABB &box, float velocityY,
                                                                     float gravityScale) {
    SolveResult result;
    if (chunkManager == nullptr) {
        LogCat::w(LogLabel::PHYSICS, std::source_location::current(), "physics_vertical_chunk_manager_null",
                  "Vertical physics solve: chunk manager is null, skip");
        return result;
    }

    //Make use of gravity.
    //应用重力。
    velocityY -= PHYSICS_GRAVITY * FIXED_TIME_STEP * gravityScale;
    //Clamp the fall speed to avoid accumulating huge velocities.
    //钳制下落速度，避免速度无限累积。
    if (velocityY < -PHYSICS_MAX_FALL_SPEED) {
        LogCat::d(LogLabel::PHYSICS, "physics_vertical_fall_clamped",
                  "Vertical physics: fall speed clamped: {} -> {}", velocityY, -PHYSICS_MAX_FALL_SPEED);
        velocityY = -PHYSICS_MAX_FALL_SPEED;
    }

    //The displacement to attempt this tick (signed: positive = up, negative = down).
    //这一步想走的位移（有符号：正=向上，负=向下）。
    const float stepY = velocityY * FIXED_TIME_STEP;
    if (stepY == 0.0F) {
        LogCat::d(LogLabel::PHYSICS, "physics_vertical_zero_step",
                  "Vertical physics: zero step, skip");
        return result;
    }


    //Swept bounding box along Y. X is fixed during a vertical sweep.
    //沿 Y 的扫掠包围盒。竖直扫掠时 X 不变。
    const float yMin = std::min(box.Bottom(), box.Bottom() + stepY);
    const float yMax = std::max(box.Top(), box.Top() + stepY);
    const TileVector2D minTile = CoordinateTransformer::WorldToTile(WorldVector2D{box.Left(), yMin});
    const TileVector2D maxTile = CoordinateTransformer::WorldToTile(WorldVector2D{box.Right(), yMax});

    LogCat::d(LogLabel::PHYSICS, "physics_vertical_sweep_start",
              "Vertical physics sweep: velocityY={}, stepY={}, gravityScale={}, yRange=[{}, {}], tileRange=({}, {})..({}, {})",
              velocityY, stepY, gravityScale, yMin, yMax, minTile.x, minTile.y, maxTile.x, maxTile.y);

    //The farthest distance still allowed to travel (magnitude).
    //还允许行走的最远距离（大小）。
    float remaining = std::fabs(stepY);

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

            //Perpendicular (X) overlap check: skip tiles outside the horizontal lane.
            //垂直轴（X）重叠检查：跳过不在水平通道内的瓦片。
            if (box.Right() <= tileLeft || box.Left() >= tileRight) {
                continue;
            }

            //The face we approach depends on the vertical direction.
            //逼近哪个面取决于竖直方向。
            float dist = 0.0F;
            if (stepY < 0.0F) {
                //Falling: the bottom edge approaches the tile's top face.
                //下落：底边逼近瓦片顶面。
                if ((mask & COLLISION_MASK_TOP) == 0) {
                    continue;
                }
                dist = box.Bottom() - tileTop;
            } else {
                //Rising: the top edge approaches the tile's bottom face.
                //上升：顶边逼近瓦片底面。
                if ((mask & COLLISION_MASK_BOTTOM) == 0) {
                    continue;
                }
                dist = tileBottom - box.Top();
            }

            //Only approaching contacts (dist >= 0) are resolved. Already-overlapping tiles (dist < 0)
            //are skipped so the body can freely move back out of them.
            //只解算逼近接触（dist >= 0）。已重叠的瓦片（dist < 0）被跳过，让身体能自由走出。
            if (dist >= -PHYSICS_CONTACT_EPSILON && dist < remaining) {
                remaining = std::max(dist, 0.0F);
                result.blocked = true;
                result.hitTile = TileVector2D{tx, ty};
                LogCat::d(LogLabel::PHYSICS, "physics_vertical_tile_hit",
                          "Vertical physics: hit tile at ({}, {}), dist={}, remaining={}", tx, ty, dist, remaining);
            }
        }
    }

    //Move up to the closest contact, keeping the sign of the intended step.
    //走到最近的接触点，保留原步长的符号。
    result.distance = std::copysign(remaining, stepY);
    //The corrected vertical velocity (zeroed when blocked), and whether the body landed.
    //修正后的竖直速度（被挡则为 0），以及是否落地。
    result.velocity = result.blocked ? 0.0F : velocityY;
    result.grounded = result.blocked && stepY < 0.0F;

    LogCat::d(LogLabel::PHYSICS, "physics_vertical_solve_result",
              "Vertical physics done: distance={}, velocity={}, blocked={}, grounded={}, hitTile=({}, {})",
              result.distance, result.velocity, result.blocked, result.grounded, result.hitTile.x, result.hitTile.y);
    return result;
}
