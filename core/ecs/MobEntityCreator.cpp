//
// Created by coldmint on 2026/3/24.
//

#include "MobEntityCreator.h"

#include "component/AutoPickComponent.h"
#include "component/MagnetComponent.h"
#include "component/MobComponent.h"
#include "component/PlayerComponent.h"
#include "component/RayCast2DComponent.h"
#include "component/RigidBody2DComponent.h"
#include "component/SpiritRendererComponent.h"
#include "component/TilePlacementForbiddenZoneComponent.h"

glimmer::MobEntityCreator::MobEntityCreator(WorldContext *worldContext) : IPersistenceEntityCreator(worldContext) {
}

EntityItemMessage glimmer::MobEntityCreator::GetEntityItemMessage(const WorldVector2D position) {
    EntityItemMessage entityItemMessage{};
    ComponentMessage *transform2DComponentMessage =
            entityItemMessage.add_components();
    Transform2DComponent transform2DComponent{};
    transform2DComponent.SetPosition(position);
    transform2DComponentMessage->set_id(transform2DComponent.GetId());
    transform2DComponentMessage->set_data(transform2DComponent.Serialize());
    return entityItemMessage;
}

void glimmer::MobEntityCreator::LoadTemplateComponents(const GameEntity::ID id, const ResourceRef &resourceRef) {
    if (worldContext_ == nullptr || WorldContext::IsEmptyEntityId(id)) {
        return;
    }
    const AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    uint32_t resourceType = resourceRef.GetResourceType();
    if (resourceType != RESOURCE_TYPE_MOB) {
        return;
    }
    ResourceLocator *resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator == nullptr) {
        return;
    }
    MobResource *mobResource = resourceLocator->FindMob(resourceRef);
    if (mobResource == nullptr) {
        return;
    }
    worldContext_->SetResourceRef(id, resourceRef);
    worldContext_->SetPersistable(id, true);
    MobComponent *mobComponent;
    if (mobResource->isPlayer) {
        auto playerComponent = worldContext_->AddComponent<PlayerComponent>(id);
        mobComponent = playerComponent;
        auto cameraComponent = worldContext_->AddComponent<CameraComponent>(id);
        auto transform2DComponent = worldContext_->AddComponent<Transform2DComponent>(id);
        if (cameraComponent != nullptr && transform2DComponent != nullptr) {
            worldContext_->SetCameraComponent(cameraComponent);
            worldContext_->SetCameraPosition(transform2DComponent);
        }
        auto diggingComponent = worldContext_->AddComponent<DiggingComponent>(id);
        if (diggingComponent != nullptr) {
            worldContext_->SetDiggingComponent(diggingComponent);
        }
        auto *magnetComponent = worldContext_->AddComponent<MagnetComponent>(id);
        if (magnetComponent != nullptr) {
            magnetComponent->SetType(MAGNETIC_TYPE_ITEM);
        }
        worldContext_->AddComponent<AutoPickComponent>(id);
    } else {
        mobComponent = worldContext_->AddComponent<MobComponent>(id);
    }
    if (mobComponent == nullptr) {
        return;
    }
    mobComponent->movementAcceleration = mobResource->movementAcceleration;
    mobComponent->maxSpeed = mobResource->maxSpeed;
    mobComponent->airControlFactor = mobResource->airControlFactor;
    mobComponent->jumpForce = mobResource->jumpForce;
    for (auto &groundCheckRayCast: mobResource->groundCheckRayCast) {
        auto groundRayCast = worldContext_->CreateEntity();
        auto *rayCast2dComponent = worldContext_->AddComponent<RayCast2DComponent>(groundRayCast);
        if (rayCast2dComponent == nullptr) {
            continue;
        }
        rayCast2dComponent->SetOrigin(
            {groundCheckRayCast.origin.x * TILE_SIZE, groundCheckRayCast.origin.y * TILE_SIZE});
        rayCast2dComponent->SetTransform({
            groundCheckRayCast.translation.x * TILE_SIZE, groundCheckRayCast.translation.y * TILE_SIZE
        });
        rayCast2dComponent->SetFilter(groundCheckRayCast.filter.Tob2QueryFilter());
        rayCast2dComponent->SetTransform2DEntity(id);
        mobComponent->groundCheckRayEntityIds.push_back(groundRayCast);
    }
    const auto tilePlacementForbiddenZoneComponent = worldContext_->AddComponent<
        TilePlacementForbiddenZoneComponent>(id);
    if (tilePlacementForbiddenZoneComponent != nullptr) {
        TilePlacementForbiddenZone tilePlacementForbiddenZone = mobResource->tilePlacementForbiddenZone;
        tilePlacementForbiddenZoneComponent->SetHeight(tilePlacementForbiddenZone.height);
        tilePlacementForbiddenZoneComponent->SetWidth(tilePlacementForbiddenZone.width);
        tilePlacementForbiddenZoneComponent->SetOffsetX(tilePlacementForbiddenZone.offsetX);
        tilePlacementForbiddenZoneComponent->SetOffsetY(tilePlacementForbiddenZone.offsetY);
    }


    const auto rigidBody2DComponent = worldContext_->AddComponent<RigidBody2DComponent>(id);
    if (rigidBody2DComponent != nullptr) {
        rigidBody2DComponent->SetBodyType(static_cast<b2BodyType>(mobResource->bodyType));
        rigidBody2DComponent->SetAllowBodySleep(mobResource->allowBodySleep);
        rigidBody2DComponent->SetFilter(mobResource->box2dFilter);
        rigidBody2DComponent->SetDensity(mobResource->density);
        rigidBody2DComponent->SetFriction(mobResource->friction);
        rigidBody2DComponent->SetFixedRotation(mobResource->fixedRotation);
        rigidBody2DComponent->SetShapeRef(mobResource->shape);
    }
    const auto spiritRendererComponent = worldContext_->AddComponent<SpiritRendererComponent>(id);
    if (spiritRendererComponent != nullptr) {
        spiritRendererComponent->SetTextureRef(mobResource->texture);
        spiritRendererComponent->SetPosition({
            TILE_SIZE * mobResource->textureOffset.x, TILE_SIZE * mobResource->textureOffset.y
        });
    }
}

void glimmer::MobEntityCreator::MergeEntityItemMessage(GameEntity::ID id, const EntityItemMessage &entityItemMessage) {
    RecoveryAllComponent(id, entityItemMessage);
    auto *transform2dComponent = worldContext_->GetComponent<Transform2DComponent>(id);
    auto *rigidBody2dComponent = worldContext_->GetComponent<RigidBody2DComponent>(id);
    if (transform2dComponent != nullptr && rigidBody2dComponent != nullptr) {
        rigidBody2dComponent->CreateBody(worldContext_->GetAppContext()->GetResourceLocator(),
                                         worldContext_->GetWorldId(), transform2dComponent->GetPosition());
    }
}
