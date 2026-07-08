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


void glimmer::MagnetSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    EntityManager* entityManager = GetEntityManager();
    if (gameComponentType == COMPONENT_TRANSFORM_2D)
    {
        transform2DComponentCount_ = count;
    }
    if (gameComponentType == COMPONENT_MAGNET)
    {
        magnetComponentCount_ = count;
    }
    if (gameComponentType == COMPONENT_MAGNETIC)
    {
        magneticComponentCount_ = count;
    }
    if (gameComponentType == COMPONENT_RIGID_BODY_2D)
    {
        rigidComponentCount_ = count;
    }
    if (gameComponentType == COMPONENT_RAY_CAST_2D)
    {
        rayCast2dComponentCount_ = count;
    }
    if (gameComponentType == COMPONENT_DROPPED_ITEM)
    {
        droppedItemComponentCount_ = count;
    }
    if (gameComponentType == COMPONENT_ITEM_CONTAINER)
    {
        itemContainerCount_ = count;
    }
    if (transform2DComponentCount_ > 0 && magnetComponentCount_ > 0)
    {
        magnetEntities_ = entityManager->GetEntityIDWithComponents({
            COMPONENT_TRANSFORM_2D, COMPONENT_MAGNET, COMPONENT_ITEM_CONTAINER
        });
    }
    if (magneticComponentCount_ > 0 && transform2DComponentCount_ > 0 && rigidComponentCount_ > 0 &&
        rayCast2dComponentCount_ > 0 && droppedItemComponentCount_ > 0)
    {
        magneticEntities_ = entityManager->GetEntityIDWithComponents({
            COMPONENT_MAGNETIC, COMPONENT_TRANSFORM_2D, COMPONENT_RIGID_BODY_2D, COMPONENT_RAY_CAST_2D,
            COMPONENT_DROPPED_ITEM
        });
    }
}

glimmer::MagnetSystem::MagnetSystem(WorldContext* worldContext) : GameSystem(worldContext)
{
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
                                                   MagnetComponent* magnet,
                                                   const WorldVector2D& magnetPos,
                                                   ItemContainer* itemContainer)
{
    EntityManager* entityManager = GetEntityManager();
    auto magneticTransform =
        entityManager->GetComponent<Transform2DComponent>(magneticEntity);
    if (magneticTransform == nullptr)
    {
        return false;
    }
    auto magnetic =
        entityManager->GetComponent<MagneticComponent>(magneticEntity);
    if (magnetic == nullptr)
    {
        return false;
    }
    auto rigidBody2DComponent = entityManager->GetComponent<RigidBody2DComponent>(magneticEntity);
    if (rigidBody2DComponent == nullptr || !rigidBody2DComponent->IsReady() || !rigidBody2DComponent->IsEnabled())
    {
        return false;
    }
    auto rayCast2DComponent = entityManager->GetComponent<RayCast2DComponent>(magneticEntity);
    if (rayCast2DComponent == nullptr)
    {
        return false;
    }
    if ((magnet->GetType() & magnetic->GetType()) == 0)
    {
        return false;
    }
    const auto droppedItem = entityManager->GetComponent<DroppedItemComponent>(magneticEntity);
    if (droppedItem == nullptr)
    {
        return false;
    }
    if (!droppedItem->CanBePickedUp())
    {
        return false;
    }
    const auto item = droppedItem->GetItem();
    if (item == nullptr)
    {
        return false;
    }
    const ItemStackModule* itemStackModule = item->GetStackModule();
    if (itemStackModule == nullptr)
    {
        return false;
    }
    size_t remainingItemAmount = itemContainer->GetRemainingItemAmountAfterAdd(itemStackModule);
    if (remainingItemAmount == itemStackModule->GetAmount())
    {
        return false;
    }
    WorldVector2D magneticPos = magneticTransform->GetPosition();
    const WorldVector2D distanceVector = magnetPos - magneticPos;
    float distance = distanceVector.Length();
    const float detectionRadius = magnet->GetDetectionRadius();
    if (distance > detectionRadius)
    {
        magnet->RemoveEntity(magneticEntity);
        return false;
    }
    rayCast2DComponent->SetTransform(distanceVector);
    if (rayCast2DComponent->IsHit())
    {
        return false;
    }
    if (distance < MIN_SAFE_DISTANCE)
    {
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
    if (distance <= magnet->GetAdsorptionRadius())
    {
        magnet->AddEntity(magneticEntity);
    }
    return true;
}

void glimmer::MagnetSystem::ProcessMagnetEntity(GameEntityID magnetEntity)
{
    EntityManager* entityManager = GetEntityManager();
    auto magnet = entityManager->GetComponent<MagnetComponent>(magnetEntity);
    if (magnet == nullptr)
    {
        return;
    }
    auto magnetTransform = entityManager->GetComponent<Transform2DComponent>(magnetEntity);
    if (magnetTransform == nullptr)
    {
        return;
    }
    auto itemContainerComponent = entityManager->GetComponent<ItemContainerComponent>(magnetEntity);
    if (itemContainerComponent == nullptr)
    {
        return;
    }
    auto itemContainer = itemContainerComponent->GetItemContainer();
    if (itemContainer == nullptr)
    {
        return;
    }
    const WorldVector2D magnetPos = magnetTransform->GetPosition();
    for (auto magneticEntity : magneticEntities_)
    {
        ProcessMagneticEntity(magneticEntity, magnet, magnetPos, itemContainer);
    }
}

void glimmer::MagnetSystem::Update(const float delta)
{
    WorldContext* worldContext = GetWorldContext();
    EntityManager* entityManager = GetEntityManager();
    if (worldContext == nullptr)
    {
        return;
    }
    if (entityManager == nullptr)
    {
        return;
    }
    if (magnetEntities_.empty())
    {
        return;
    }
    if (magneticEntities_.empty())
    {
        return;
    }
    for (auto magnetEntity : magnetEntities_)
    {
        ProcessMagnetEntity(magnetEntity);
    }
}

glimmer::GameSystemType glimmer::MagnetSystem::GetGameSystemType() const
{
    return GameSystemType::MagnetSystem;
}
