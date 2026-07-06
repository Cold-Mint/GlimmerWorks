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
#include "DroppedItemCreator.h"

#include "component/DroppedItemComponent.h"
#include "component/MagneticComponent.h"
#include "component/RayCast2DComponent.h"
#include "component/RigidBody2DComponent.h"
#include "core/world/WorldContext.h"

glimmer::DroppedItemCreator::DroppedItemCreator(WorldContext* worldContext) : IPersistenceEntityCreator(worldContext)
{
}

EntityItemMessage glimmer::DroppedItemCreator::GetEntityItemMessage(const WorldVector2D& position,
                                                                    std::unique_ptr<Item> item,
                                                                    const float pickupCooldown)
{
    EntityItemMessage entityItemMessage{};
    Transform2DComponent transform2DComponent{};
    transform2DComponent.SetPosition(position);
    auto transform2DString = transform2DComponent.Serialize();
    if (transform2DString.has_value())
    {
        ComponentMessage* transform2DComponentMessage =
            entityItemMessage.add_components();
        transform2DComponentMessage->set_type(transform2DComponent.GetComponentType());
        transform2DComponentMessage->set_data(transform2DString.value());
    }

    DroppedItemComponent droppedItemComponent{};
    droppedItemComponent.SetItem(std::move(item));
    droppedItemComponent.SetPickupCooldown(pickupCooldown);
    auto droppedItemString = droppedItemComponent.Serialize();
    if (droppedItemString.has_value())
    {
        ComponentMessage* droppedItemComponentMessage =
            entityItemMessage.add_components();
        droppedItemComponentMessage->set_type(droppedItemComponent.GetComponentType());
        droppedItemComponentMessage->set_data(droppedItemString.value());
    }
    return entityItemMessage;
}

glimmer::ResourceRef glimmer::DroppedItemCreator::GetResourceRef()
{
    ResourceRef resourceRef{};
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_DROPPED_ITEM);
    resourceRef.SetResourceKey(DROPPED_ITEM_ID_DEFAULT);
    return resourceRef;
}

void glimmer::DroppedItemCreator::LoadTemplateComponents(const uint32_t id, const ResourceRef& resourceRef)
{
    uint32_t type = resourceRef.GetResourceType();
    if (type != RESOURCE_DROPPED_ITEM)
    {
        return;
    }
    WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr || WorldContext::IsEmptyEntityId(id))
    {
        return;
    }
    const AppContext* appContext = worldContext->GetAppContext();
    if (appContext == nullptr)
    {
        return;
    }
    EntityManager* entityManager = worldContext->GetEntityManager();
    entityManager->SetResourceRef(id, resourceRef);
    entityManager->SetPersistable(id, true);
    const auto rigidBody2DComponent = entityManager->AddComponent<RigidBody2DComponent>(
        id);
    if (rigidBody2DComponent != nullptr)
    {
        rigidBody2DComponent->SetFilter({BOX2D_CATEGORY_ITEM, BOX2D_CATEGORY_TILE});
        rigidBody2DComponent->SetBodyType(b2_dynamicBody);
        rigidBody2DComponent->SetDensity(0.005F);
        ResourceRef shapeResourceRef;
        shapeResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
        shapeResourceRef.SetResourceKey(SHAPE_ID_DROPPED_ITEM);
        shapeResourceRef.SetResourceType(RESOURCE_SHAPE);
        rigidBody2DComponent->SetShapeRef(shapeResourceRef);
    }
    const auto rayCast2DComponent = entityManager->AddComponent<RayCast2DComponent>(id);
    if (rayCast2DComponent != nullptr)
    {
        rayCast2DComponent->SetOrigin({0, 0});
        rayCast2DComponent->SetFilter({BOX2D_CATEGORY_ITEM, BOX2D_CATEGORY_TILE});
        rayCast2DComponent->SetTransform2DEntity(id);
    }
    auto magnetic = entityManager->AddComponent<MagneticComponent>(id);
    if (magnetic != nullptr)
    {
        magnetic->SetType(MAGNETIC_TYPE_ITEM);
    }
}

void glimmer::DroppedItemCreator::
MergeEntityItemMessage(uint32_t id, const EntityItemMessage& entityItemMessage)
{
    WorldContext* worldContext = GetWorldContext();
    RecoveryAllComponent(worldContext, id, entityItemMessage);
    EntityManager* entityManager = worldContext->GetEntityManager();
    auto transform2dComponent = entityManager->GetComponent<Transform2DComponent>(id);
    auto rigidBody2dComponent = entityManager->GetComponent<RigidBody2DComponent>(id);
    if (transform2dComponent != nullptr && rigidBody2dComponent != nullptr)
    {
        rigidBody2dComponent->CreateBody(worldContext->GetAppContext()->GetResourceLocator(),
                                         worldContext->GetWorldId(), transform2dComponent->GetPosition());
    }
}
