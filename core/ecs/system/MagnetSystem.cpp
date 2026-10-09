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
#include "MagnetSystem.h"

#include "core/log/LogCat.h"
#include "core/world/WorldContext.h"
#include "core/ecs/component/RigidBody2DComponent.h"
#include "core/ecs/component/DroppedItemComponent.h"
#include "core/ecs/component/MagnetComponent.h"
#include "core/ecs/component/MagneticComponent.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "box2d/box2d.h"
#include "core/ecs/component/ItemContainerComponent.h"
#include "core/ecs/component/RayCast2DComponent.h"
#include "core/utils/Box2DUtils.h"


void glimmer::MagnetSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) {
    EntityManager *entityManager = GetEntityManager();
    if (gameComponentType == COMPONENT_TRANSFORM_2D) {
        transform2DComponentCount_ = count;
    }
    if (gameComponentType == COMPONENT_MAGNET) {
        magnetComponentCount_ = count;
    }
    if (gameComponentType == COMPONENT_MAGNETIC) {
        magneticComponentCount_ = count;
    }
    if (gameComponentType == COMPONENT_RIGID_BODY_2D) {
        rigidComponentCount_ = count;
    }
    if (gameComponentType == COMPONENT_RAY_CAST_2D) {
        rayCast2dComponentCount_ = count;
    }
    if (gameComponentType == COMPONENT_DROPPED_ITEM) {
        droppedItemComponentCount_ = count;
    }
    if (gameComponentType == COMPONENT_ITEM_CONTAINER) {
        itemContainerCount_ = count;
    }
    if (transform2DComponentCount_ > 0 && magnetComponentCount_ > 0) {
        std::lock_guard lock(magnetMutex_);
        magnetEntities_ = entityManager->GetEntityIDWithComponents({
            COMPONENT_TRANSFORM_2D, COMPONENT_MAGNET, COMPONENT_ITEM_CONTAINER
        });
        LogCat::d(LogLabel::DEFAULT, "magnet_entities_rebuilt", "Magnet entities rebuilt: {} magnets",
                  magnetEntities_.size());
    }
    if (magneticComponentCount_ > 0 && transform2DComponentCount_ > 0 && rigidComponentCount_ > 0 &&
        rayCast2dComponentCount_ > 0 && droppedItemComponentCount_ > 0) {
        std::lock_guard lock(magnetMutex_);
        magneticEntities_ = entityManager->GetEntityIDWithComponents({
            COMPONENT_MAGNETIC, COMPONENT_TRANSFORM_2D, COMPONENT_RIGID_BODY_2D, COMPONENT_RAY_CAST_2D,
            COMPONENT_DROPPED_ITEM
        });
        LogCat::d(LogLabel::DEFAULT, "magnetic_entities_rebuilt", "Magnetic entities rebuilt: {} entities",
                  magneticEntities_.size());
    }
}

glimmer::MagnetSystem::MagnetSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    WatchComponent(COMPONENT_TRANSFORM_2D);
    WatchComponent(COMPONENT_MAGNET);
    WatchComponent(COMPONENT_MAGNETIC);
    WatchComponent(COMPONENT_RIGID_BODY_2D);
    WatchComponent(COMPONENT_RAY_CAST_2D);
    WatchComponent(COMPONENT_ITEM_CONTAINER);
    WatchComponent(COMPONENT_DROPPED_ITEM);
    Init();
}

bool glimmer::MagnetSystem::ProcessMagneticEntity(GameEntityID magneticEntity,
                                                  MagnetComponent *magnet,
                                                  const WorldVector2D &magnetPos,
                                                  ItemContainer *itemContainer) {
    EntityManager *entityManager = GetEntityManager();
    auto magneticTransform =
            entityManager->GetComponent<Transform2DComponent>(magneticEntity);
    if (magneticTransform == nullptr) {
        return false;
    }
    auto magnetic =
            entityManager->GetComponent<MagneticComponent>(magneticEntity);
    if (magnetic == nullptr) {
        return false;
    }
    auto rigidBody2DComponent = entityManager->GetComponent<RigidBody2DComponent>(magneticEntity);
    if (rigidBody2DComponent == nullptr || !rigidBody2DComponent->IsReady() || !rigidBody2DComponent->IsEnabled()) {
        return false;
    }
    auto rayCast2DComponent = entityManager->GetComponent<RayCast2DComponent>(magneticEntity);
    if (rayCast2DComponent == nullptr) {
        return false;
    }
    if ((magnet->GetType() & magnetic->GetType()) == 0) {
        return false;
    }
    const auto droppedItem = entityManager->GetComponent<DroppedItemComponent>(magneticEntity);
    if (droppedItem == nullptr) {
        return false;
    }
    if (!droppedItem->CanBePickedUp()) {
        return false;
    }
    const auto item = droppedItem->GetItem();
    if (item == nullptr) {
        return false;
    }
    const ItemStackModule *itemStackModule = item->GetStackModule();
    if (itemStackModule == nullptr) {
        return false;
    }
    if (size_t remainingItemAmount = itemContainer->GetRemainingItemAmountAfterAdd(item);
        remainingItemAmount == itemStackModule->GetAmount()) {
        //There's no room for even one more.
        //一个都放不下。
        return false;
    }
    WorldVector2D magneticPos = magneticTransform->GetPosition();
    const WorldVector2D distanceVector = magnetPos - magneticPos;
    float distance = distanceVector.Length();
    const float detectionRadius = magnet->GetDetectionRadius();
    if (distance > detectionRadius) {
        magnet->RemoveEntity(magneticEntity);
        return false;
    }
    rayCast2DComponent->SetTransform(distanceVector);
    if (rayCast2DComponent->IsHit()) {
        return false;
    }
    if (distance < MIN_SAFE_DISTANCE) {
        distance = MIN_SAFE_DISTANCE;
    }
    float normalizedDistance = distance / detectionRadius;
    float forceStrength = 1 - normalizedDistance;
    b2MassData massData = b2Body_GetMassData(rigidBody2DComponent->GetBodyId());
    WorldVector2D force = distanceVector.Normalized()
                          * massData.mass
                          * MAX_MAGNET_FORCE
                          * forceStrength;
    b2Body_ApplyForceToCenter(rigidBody2DComponent->GetBodyId(), {force.x, force.y}, true);
    if (distance <= magnet->GetAdsorptionRadius()) {
        magnet->AddEntity(magneticEntity);
        LogCat::d(LogLabel::DEFAULT, "magnetic_entity_adsorbed", "Magnetic entity {} entered adsorption radius",
                  magneticEntity);
    }
    return true;
}

void glimmer::MagnetSystem::ProcessMagnetEntity(GameEntityID magnetEntity,
                                                const std::vector<GameEntityID> &magneticEntities) {
    EntityManager *entityManager = GetEntityManager();
    auto magnet = entityManager->GetComponent<MagnetComponent>(magnetEntity);
    if (magnet == nullptr) {
        return;
    }
    auto magnetTransform = entityManager->GetComponent<Transform2DComponent>(magnetEntity);
    if (magnetTransform == nullptr) {
        return;
    }
    auto itemContainerComponent = entityManager->GetComponent<ItemContainerComponent>(magnetEntity);
    if (itemContainerComponent == nullptr) {
        return;
    }
    auto itemContainer = itemContainerComponent->GetItemContainer();
    if (itemContainer == nullptr) {
        return;
    }
    const WorldVector2D magnetPos = magnetTransform->GetPosition();
    for (auto magneticEntity: magneticEntities) {
        ProcessMagneticEntity(magneticEntity, magnet, magnetPos, itemContainer);
    }
}

void glimmer::MagnetSystem::OnTick(const uint64_t tick) {
    WorldContext *worldContext = GetWorldContext();
    if (worldContext == nullptr || GetEntityManager() == nullptr) {
        return;
    }
    std::vector<GameEntityID> magnetEntities;
    std::vector<GameEntityID> magneticEntities;
    {
        std::lock_guard lock(magnetMutex_);
        if (magnetEntities_.empty() || magneticEntities_.empty()) {
            return;
        }
        magnetEntities = magnetEntities_;
        magneticEntities = magneticEntities_;
    }
    for (const GameEntityID magnetEntity: magnetEntities) {
        ProcessMagnetEntity(magnetEntity, magneticEntities);
    }
}

glimmer::GameSystemType glimmer::MagnetSystem::GetGameSystemType() const {
    return GameSystemType::MagnetSystem;
}
