//
// Created by coldmint on 2025/12/20.
//

#include "DroppedItemSystem.h"

#include "../../world/WorldContext.h"
#include "../component/CameraComponent.h"

uint8_t glimmer::DroppedItemSystem::GetRenderOrder() {
    return RENDER_ORDER_DROPPED_ITEM;
}

void glimmer::DroppedItemSystem::Update(float delta) {
    auto entity = worldContext_->GetEntitiesWithComponents<DroppedItemComponent, Transform2DComponent>();
    for (auto gameEntity: entity) {
        auto *droppedItemComponent = worldContext_->GetComponent<DroppedItemComponent>(
            gameEntity->GetID());
        auto *transform2DComponent = worldContext_->GetComponent<Transform2DComponent>(gameEntity->GetID());
        if (droppedItemComponent == nullptr || transform2DComponent == nullptr) {
            continue;
        }
        if (droppedItemComponent->IsExpired()) {
            worldContext_->RemoveEntity(gameEntity->GetID());
            continue;
        }
        float remaining = droppedItemComponent->GetRemainingTime();
        remaining -= delta;
        droppedItemComponent->SetRemainingTime(remaining);

        float cooldown = droppedItemComponent->GetPickupCooldown();
        if (cooldown > 0.0F) {
            cooldown -= delta;
            droppedItemComponent->SetPickupCooldown(cooldown);
        }
    }
}

void glimmer::DroppedItemSystem::Render(SDL_Renderer *renderer) {
    auto entity = worldContext_->GetEntitiesWithComponents<DroppedItemComponent, Transform2DComponent>();
    auto cameraComponent = worldContext_->GetCameraComponent();
    auto cameraTransform2D = worldContext_->GetCameraTransform2D();
    float size = DROPPED_ITEM_SIZE * cameraComponent->GetZoom();

    for (auto gameEntity: entity) {
        auto *droppedItemComponent = worldContext_->GetComponent<DroppedItemComponent>(
            gameEntity->GetID());
        auto *transform2DComponent = worldContext_->GetComponent<Transform2DComponent>(gameEntity->GetID());
        if (droppedItemComponent == nullptr || transform2DComponent == nullptr) {
            continue;
        }
        Item *item = droppedItemComponent->GetItem();
        if (item == nullptr) {
            continue;
        }
        auto icon = item->GetIcon();
        if (icon == nullptr) {
            continue;
        }
        if (cameraComponent->IsPointInViewport(cameraTransform2D->GetPosition(), transform2DComponent->GetPosition())) {
            const auto worldPos = transform2DComponent->GetPosition();
            const CameraVector2D viewport = cameraComponent->GetViewPortPosition(
                cameraTransform2D->GetPosition(), worldPos);
            SDL_FRect dstRect{
                viewport.x - size * 0.5f,
                viewport.y - size * 0.5f,
                size,
                size
            };

            SDL_RenderTexture(renderer, icon.get(), nullptr, &dstRect);
        }
    }
}

std::string glimmer::DroppedItemSystem::GetName() {
    return "glimmer.DroppedItemSystem";
}
