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

#include <cmath>

#include "core/config/Constants.h"
#include "core/ecs/component/ColliderComponent.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/world/TileCollision.h"
#include "core/world/WorldContext.h"

void glimmer::PhysicsSystem::OnWatchedComponentChanged(const GameComponentTypeMessage gameComponentType,
                                                       const uint32_t count) {
    if (gameComponentType == COMPONENT_RIGID_BODY_2D) {
        colliderCount_ = count;
    }
    if (gameComponentType == COMPONENT_TRANSFORM_2D) {
        transform2dCount_ = count;
    }
    if (colliderCount_ > 0 && transform2dCount_ > 0) {
        entities_.clear();
        entities_ = entityManager_->GetEntityIDWithComponents({COMPONENT_RIGID_BODY_2D, COMPONENT_TRANSFORM_2D});
    }
}

void glimmer::PhysicsSystem::StepEntity(WorldContext *worldContext, ColliderComponent *collider,
                                        Transform2DComponent *transform) const {
    WorldVector2D velocity = collider->GetVelocity();

    // Integrate gravity and clamp the fall speed to avoid accumulating huge velocities.
    // 积分重力并钳制下落速度，避免速度无限累积。
    velocity.y -= PHYSICS_GRAVITY * FIXED_TIME_STEP;
    if (velocity.y < -PHYSICS_MAX_FALL_SPEED) {
        velocity.y = -PHYSICS_MAX_FALL_SPEED;
    }

    WorldVector2D position = transform->GetPosition();
    const float halfWidth = collider->GetHalfWidth();
    const float halfHeight = collider->GetHalfHeight();

    // Move on the X axis and resolve horizontal collisions (walls).
    // 在 X 轴上移动并解算水平碰撞（墙体）。
    position.x += velocity.x * FIXED_TIME_STEP;
    {
        const float centerX = position.x;
        int minTx = static_cast<int>(std::floor((position.x - halfWidth) / TILE_SIZE + 0.5F));
        int maxTx = static_cast<int>(std::floor((position.x + halfWidth) / TILE_SIZE + 0.5F));
        int minTy = static_cast<int>(std::floor((position.y - halfHeight) / TILE_SIZE + 0.5F));
        int maxTy = static_cast<int>(std::floor((position.y + halfHeight) / TILE_SIZE + 0.5F));
        for (int ty = minTy; ty <= maxTy; ++ty) {
            for (int tx = minTx; tx <= maxTx; ++tx) {
                if (!TileCollision::IsSolidTile(worldContext, TileVector2D{tx, ty})) {
                    continue;
                }
                // A tile is centered on (tx*TILE_SIZE, ty*TILE_SIZE) and spans
                // [tx*TILE_SIZE - HALF_TILE_SIZE, tx*TILE_SIZE + HALF_TILE_SIZE). Push the AABB out of the
                // tile's left/right faces accordingly.
                // 瓦片以 (tx*TILE_SIZE, ty*TILE_SIZE) 为中心，覆盖 [tx*TILE_SIZE - HALF_TILE_SIZE,
                // tx*TILE_SIZE + HALF_TILE_SIZE)。据此把 AABB 从瓦片左/右面推出。
                const float tileCenterX = static_cast<float>(tx * TILE_SIZE);
                if (velocity.x > 0.0F && tileCenterX > centerX) {
                    position.x = tileCenterX - HALF_TILE_SIZE - halfWidth;
                    velocity.x = 0.0F;
                } else if (velocity.x < 0.0F && tileCenterX < centerX) {
                    position.x = tileCenterX + HALF_TILE_SIZE + halfWidth;
                    velocity.x = 0.0F;
                }
            }
        }
    }

    // Move on the Y axis and resolve vertical collisions (landing / ceiling bump).
    // 在 Y 轴上移动并解算竖直碰撞（落地 / 撞头）。
    bool grounded = false;
    position.y += velocity.y * FIXED_TIME_STEP;
    {
        const float centerY = position.y;
        int minTx = static_cast<int>(std::floor((position.x - halfWidth) / TILE_SIZE + 0.5F));
        int maxTx = static_cast<int>(std::floor((position.x + halfWidth) / TILE_SIZE + 0.5F));
        int minTy = static_cast<int>(std::floor((position.y - halfHeight) / TILE_SIZE + 0.5F));
        int maxTy = static_cast<int>(std::floor((position.y + halfHeight) / TILE_SIZE + 0.5F));
        for (int ty = minTy; ty <= maxTy; ++ty) {
            for (int tx = minTx; tx <= maxTx; ++tx) {
                if (!TileCollision::IsSolidTile(worldContext, TileVector2D{tx, ty})) {
                    continue;
                }
                // Only resolve a tile that actually lies in the direction of motion: a floor below the
                // center when falling, a ceiling above the center when rising. This avoids pushing the
                // entity through the opposite face when a tile is merely touched at its edge boundary.
                // 只解算真正位于运动方向上的瓦片：下落时为中心下方的地板，上升时为中心上方的天花板。
                // 这避免了当瓦片仅在边缘处被接触时，把实体朝反方向推出。
                const float tileCenterY = static_cast<float>(ty * TILE_SIZE);
                if (velocity.y < 0.0F && tileCenterY < centerY) {
                    position.y = tileCenterY + HALF_TILE_SIZE + halfHeight;
                    grounded = true;
                    velocity.y = 0.0F;
                } else if (velocity.y > 0.0F && tileCenterY > centerY) {
                    position.y = tileCenterY - HALF_TILE_SIZE - halfHeight;
                    velocity.y = 0.0F;
                }
            }
        }
    }

    collider->SetVelocity(velocity);
    collider->SetGrounded(grounded);
    transform->SetPosition(position);
}

void glimmer::PhysicsSystem::OnTick(const uint64_t tick) {
    (void) tick;
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
    WatchComponent(COMPONENT_RIGID_BODY_2D);
    WatchComponent(COMPONENT_TRANSFORM_2D);
    Init();
}

glimmer::GameSystemType glimmer::PhysicsSystem::GetGameSystemType() const {
    return GameSystemType::PhysicsSystem;
}
