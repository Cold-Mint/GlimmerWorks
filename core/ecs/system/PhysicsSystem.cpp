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
#include "PhysicsSystem.h"

#include "core/config/Constants.h"
#include "core/ecs/component/ColliderComponent.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/log/LogCat.h"
#include "core/math/CoordinateTransformer.h"
#include "core/world/TileCollision.h"
#include "core/world/WorldContext.h"

void glimmer::PhysicsSystem::OnWatchedComponentChanged(const GameComponentTypeMessage gameComponentType,
                                                       const uint32_t count) {
    if (gameComponentType == COMPONENT_COLLIDER_2D) {
        colliderCount_ = count;
    }
    if (gameComponentType == COMPONENT_TRANSFORM_2D) {
        transform2dCount_ = count;
    }
    if (colliderCount_ > 0 && transform2dCount_ > 0) {
        entities_.clear();
        entities_ = entityManager_->GetEntityIDWithComponents({COMPONENT_COLLIDER_2D, COMPONENT_TRANSFORM_2D});
    }
}

void glimmer::PhysicsSystem::StepEntity(const WorldContext *worldContext, ColliderComponent *collider,
                                        Transform2DComponent *transform) const {
    WorldVector2D velocityCopy = collider->GetVelocity();
    LogCat::d(LogLabel::MOVE, "move_step_start", "Physics step start: velocity=({}, {})", velocityCopy.x,
              velocityCopy.y);
    // Integrate gravity and clamp the fall speed to avoid accumulating huge velocities.
    // 积分重力并钳制下落速度，避免速度无限累积。
    velocityCopy.y -= PHYSICS_GRAVITY * FIXED_TIME_STEP;
    if (velocityCopy.y < -PHYSICS_MAX_FALL_SPEED) {
        velocityCopy.y = -PHYSICS_MAX_FALL_SPEED;
        LogCat::d(LogLabel::MOVE, "move_fall_speed_clamped", "Fall speed clamped: velocity.y={}", velocityCopy.y);
    }
    WorldVector2D position = transform->GetPosition();
    const float halfWidth = collider->GetHalfWidth();
    const float halfHeight = collider->GetHalfHeight();
    const ChunkManager *chunkManager = worldContext->GetChunkManager();
    // Move on the X axis and resolve horizontal collisions (walls).
    // 在 X 轴上移动并解算水平碰撞（墙体）。
    position.x += velocityCopy.x * FIXED_TIME_STEP;

    {
        const float playerBottom = position.y - halfHeight;
        const float playerTop = position.y + halfHeight;
        const TileVector2D minTile = CoordinateTransformer::WorldToTile(
            WorldVector2D{position.x - halfWidth, playerBottom});
        const TileVector2D maxTile = CoordinateTransformer::WorldToTile(
            WorldVector2D{position.x + halfWidth, playerTop});
        for (int ty = minTile.y; ty <= maxTile.y; ++ty) {
            for (int tx = minTile.x; tx <= maxTile.x; ++tx) {
                if (!TileCollision::IsSolidTile(chunkManager, TileVector2D{tx, ty})) {
                    continue;
                }
                // A tile is centered on (tx*TILE_SIZE, ty*TILE_SIZE) and spans
                // [tx*TILE_SIZE - HALF_TILE_SIZE, tx*TILE_SIZE + HALF_TILE_SIZE). Only resolve a tile
                // whose AABB actually overlaps the player's AABB on both axes, then push the player out
                // along the direction of horizontal motion.
                // 瓦片以 (tx*TILE_SIZE, ty*TILE_SIZE) 为中心，覆盖 [tx*TILE_SIZE - HALF_TILE_SIZE,
                // tx*TILE_SIZE + HALF_TILE_SIZE)。只解算与玩家 AABB 在两个轴上都真正重叠的瓦片，
                // 再沿水平运动方向把玩家推出。
                const float tileBottom = static_cast<float>(ty * TILE_SIZE) - HALF_TILE_SIZE;
                const float tileTop = static_cast<float>(ty * TILE_SIZE) + HALF_TILE_SIZE;
                if (playerTop <= tileBottom || playerBottom >= tileTop) {
                    continue;
                }
                const float tileLeft = static_cast<float>(tx * TILE_SIZE) - HALF_TILE_SIZE;
                const float tileRight = static_cast<float>(tx * TILE_SIZE) + HALF_TILE_SIZE;
                const float playerLeft = position.x - halfWidth;
                const float playerRight = position.x + halfWidth;
                if (playerRight > tileLeft && playerLeft < tileRight) {
                    if (velocityCopy.x > 0.0F) {
                        position.x = tileLeft - halfWidth;
                    } else if (velocityCopy.x < 0.0F) {
                        position.x = tileRight + halfWidth;
                    }
                    velocityCopy.x = 0.0F;
                }
            }
        }
    }

    // Move on the Y axis and resolve vertical collisions (landing / ceiling bump).
    // 在 Y 轴上移动并解算竖直碰撞（落地 / 撞头）。
    LogCat::d(LogLabel::MOVE, "move_x_axis_resolved", "X axis resolved: position=({}, {})", position.x,
              position.y);
    bool grounded = false;
    position.y += velocityCopy.y * FIXED_TIME_STEP;
    {
        const float playerLeft = position.x - halfWidth;
        const float playerRight = position.x + halfWidth;
        const TileVector2D minTile = CoordinateTransformer::WorldToTile(
            WorldVector2D{playerLeft, position.y - halfHeight});
        const TileVector2D maxTile = CoordinateTransformer::WorldToTile(
            WorldVector2D{playerRight, position.y + halfHeight});
        for (int ty = minTile.y; ty <= maxTile.y; ++ty) {
            for (int tx = minTile.x; tx <= maxTile.x; ++tx) {
                if (!TileCollision::IsSolidTile(chunkManager, TileVector2D{tx, ty})) {
                    continue;
                }
                // Only resolve a tile whose AABB actually overlaps the player's AABB on both axes, then
                // push the player out along the direction of vertical motion: onto the tile top when
                // falling, or below the tile bottom when rising.
                // 只解算与玩家 AABB 在两个轴上都真正重叠的瓦片，再沿竖直运动方向把玩家推出：
                // 下落时放到瓦片顶部，上升时放到瓦片底部。
                const float tileLeft = static_cast<float>(tx * TILE_SIZE) - HALF_TILE_SIZE;
                const float tileRight = static_cast<float>(tx * TILE_SIZE) + HALF_TILE_SIZE;
                if (playerRight <= tileLeft || playerLeft >= tileRight) {
                    continue;
                }
                const float tileBottom = static_cast<float>(ty * TILE_SIZE) - HALF_TILE_SIZE;
                const float tileTop = static_cast<float>(ty * TILE_SIZE) + HALF_TILE_SIZE;
                const float playerBottom = position.y - halfHeight;
                const float playerTop = position.y + halfHeight;
                if (playerBottom < tileTop && playerTop > tileBottom) {
                    if (velocityCopy.y < 0.0F) {
                        position.y = tileTop + halfHeight;
                        grounded = true;
                        LogCat::d(LogLabel::MOVE, "move_landed", "Landed: position=({}, {})", position.x,
                                  position.y);
                    } else if (velocityCopy.y > 0.0F) {
                        position.y = tileBottom - halfHeight;
                        LogCat::d(LogLabel::MOVE, "move_ceiling_bump", "Ceiling bump: position=({}, {})", position.x,
                                  position.y);
                    }
                    velocityCopy.y = 0.0F;
                }
            }
        }
    }
    collider->SetVelocity(velocityCopy);
    collider->SetGrounded(grounded);
    transform->SetPosition(position);
    LogCat::d(LogLabel::MOVE, "move_step_end", "Physics step end: position=({}, {}), velocity=({}, {}), grounded={}",
              position.x, position.y, velocityCopy.x, velocityCopy.y, grounded);
}

void glimmer::PhysicsSystem::OnTick(const uint64_t tick) {
    WorldContext *worldContext = GetWorldContext();
    if (worldContext == nullptr) {
        return;
    }
    for (const GameEntityID entityId: entities_) {
        auto *collider = entityManager_->GetComponent<ColliderComponent>(entityId);
        if (collider == nullptr || !collider->IsEnabled()) {
            continue;
        }
        auto *transform = entityManager_->GetComponent<Transform2DComponent>(entityId);
        if (transform == nullptr) {
            continue;
        }
        StepEntity(worldContext, collider, transform);
    }
}

glimmer::PhysicsSystem::PhysicsSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    entityManager_ = worldContext->GetEntityManager();
    WatchComponent(COMPONENT_COLLIDER_2D);
    WatchComponent(COMPONENT_TRANSFORM_2D);
    Init();
}

glimmer::GameSystemType glimmer::PhysicsSystem::GetGameSystemType() const {
    return GameSystemType::PhysicsSystem;
}
