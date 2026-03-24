//
// Created by coldmint on 2026/3/24.
//

#include "DroppedItemCreator.h"

#include "component/DroppedItemComponent.h"
#include "component/MagneticComponent.h"
#include "component/RigidBody2DComponent.h"

glimmer::DroppedItemCreator::DroppedItemCreator(WorldContext *worldContext) : IEntityCreator(worldContext) {
}

EntityItemMessage glimmer::DroppedItemCreator::GetEntityItemMessage(const WorldVector2D position,
                                                                    std::unique_ptr<Item> item,
                                                                    const float pickupCooldown) {
    EntityItemMessage entityItemMessage{};
    ComponentMessage *transform2DComponentMessage =
            entityItemMessage.add_components();
    Transform2DComponent transform2DComponent{};
    transform2DComponent.SetPosition(position);
    transform2DComponentMessage->set_id(transform2DComponent.GetId());
    transform2DComponentMessage->set_data(transform2DComponent.Serialize());
    ComponentMessage *droppedItemComponentMessage =
            entityItemMessage.add_components();
    DroppedItemComponent droppedItemComponent{};
    droppedItemComponent.SetItem(std::move(item));
    droppedItemComponent.SetPickupCooldown(pickupCooldown);
    droppedItemComponentMessage->set_id(droppedItemComponent.GetId());
    droppedItemComponentMessage->set_data(droppedItemComponent.Serialize());
    return entityItemMessage;
}

glimmer::ResourceRef glimmer::DroppedItemCreator::GetResourceRef() {
    ResourceRef resourceRef{};
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_TYPE_DROPPED_ITEM);
    resourceRef.SetResourceKey(DROPPED_ITEM_ID_DEFAULT);
    return resourceRef;
}

void glimmer::DroppedItemCreator::LoadTemplateComponents(const GameEntity::ID id, const ResourceRef &resourceRef) {
    uint32_t type = resourceRef.GetResourceType();
    if (type != RESOURCE_TYPE_DROPPED_ITEM) {
        return;
    }
    if (worldContext_ == nullptr || WorldContext::IsEmptyEntityId(id)) {
        return;
    }
    const AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    worldContext_->SetResourceRef(id, resourceRef);
    worldContext_->SetPersistable(id, true);
    const auto rigidBody2DComponent = worldContext_->AddComponent<RigidBody2DComponent>(
        id);
    if (rigidBody2DComponent != nullptr) {
        rigidBody2DComponent->SetFilter({BOX2D_CATEGORY_ITEM, BOX2D_CATEGORY_TILE});
        rigidBody2DComponent->SetBodyType(b2_dynamicBody);
        rigidBody2DComponent->SetDensity(0.005F);
        ResourceRef shapeResourceRef;
        shapeResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
        shapeResourceRef.SetResourceKey(SHAPE_ID_DROPPED_ITEM);
        shapeResourceRef.SetResourceType(RESOURCE_TYPE_SHAPE);
        rigidBody2DComponent->SetShapeRef(shapeResourceRef);
    }
    auto *magnetic = worldContext_->AddComponent<MagneticComponent>(id);
    if (magnetic != nullptr) {
        magnetic->SetType(MAGNETIC_TYPE_ITEM);
    }
}

void glimmer::DroppedItemCreator::
MergeEntityItemMessage(GameEntity::ID id, const EntityItemMessage &entityItemMessage) {
    RecoveryAllComponent(id, entityItemMessage);
    auto *transform2dComponent = worldContext_->GetComponent<Transform2DComponent>(id);
    auto *rigidBody2dComponent = worldContext_->GetComponent<RigidBody2DComponent>(id);
    if (transform2dComponent != nullptr && rigidBody2dComponent != nullptr) {
        rigidBody2dComponent->CreateBody(worldContext_->GetAppContext()->GetResourceLocator(),
                                         worldContext_->GetWorldId(), transform2dComponent->GetPosition());
    }
}
