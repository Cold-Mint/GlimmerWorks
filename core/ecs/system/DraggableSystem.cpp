//
// Created by coldmint on 2026/4/2.
//

#include "DraggableSystem.h"

#include "core/Constants.h"
#include "core/ecs/DroppedItemCreator.h"
#include "core/ecs/component/DraggableComponent.h"
#include "core/ecs/component/GuiTransform2DComponent.h"
#include "core/ecs/component/ItemSlotComponent.h"
#include "core/ecs/component/SpiritRendererComponent.h"
#include "core/scene/AppContext.h"
#include "core/world/WorldContext.h"


SDL_FRect glimmer::DraggableSystem::DraggableBorder(GameEntity::ID entityId, WorldVector2D cameraPosition,
                                                    const CameraComponent *cameraComponent) const {
    SDL_FRect border = {0, 0, 0, 0};
    const DraggableComponent *draggableComponent = worldContext_->GetComponent<DraggableComponent>(entityId);
    if (draggableComponent == nullptr) {
        return border;
    }
    CameraVector2D startCoordinates;
    const Transform2DComponent *transformComponent = worldContext_->GetComponent<Transform2DComponent>(entityId);
    if (transformComponent != nullptr) {
        const WorldVector2D entityPosition = transformComponent->GetPosition();
        if (!cameraComponent->IsPointInViewport(cameraPosition, entityPosition)) {
            return border;
        }
        startCoordinates = cameraComponent->GetViewPortPosition(
            cameraPosition, transformComponent->GetPosition());
    }
    const GuiTransform2DComponent *guiTransformComponent = worldContext_->GetComponent<
        GuiTransform2DComponent>(entityId);
    if (guiTransformComponent != nullptr) {
        startCoordinates = guiTransformComponent->GetPosition();
    }
    border.x = startCoordinates.x;
    border.y = startCoordinates.y;
    border.w = draggableComponent->GetSize().x;
    border.h = draggableComponent->GetSize().y;
    return border;
}

bool glimmer::DraggableSystem::ShouldActivate() {
    if (worldContext_ == nullptr) {
        return false;
    }
    return worldContext_->IsDragMode();
}

void glimmer::DraggableSystem::OnActivationChanged(bool activeStatus) {
    if (!activeStatus) {
        DropItem();
    }
}

bool glimmer::DraggableSystem::DropItem() {
    if (item_ == nullptr) {
        return false;
    }
    GameEntity::ID droppedEntity = worldContext_->CreateEntity();
    DroppedItemCreator droppedItemCreator{worldContext_};
    droppedItemCreator.LoadTemplateComponents(droppedEntity,
                                              DroppedItemCreator::GetResourceRef());
    droppedItemCreator.MergeEntityItemMessage(droppedEntity,
                                              DroppedItemCreator::GetEntityItemMessage(
                                                  worldContext_->GetCameraTransform2D()
                                                  ->
                                                  GetPosition(),
                                                  std::move(item_),
                                                  2));
    return true;
}

glimmer::DraggableSystem::DraggableSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    item_ = nullptr;
}

uint8_t glimmer::DraggableSystem::GetRenderOrder() {
    return RENDER_ORDER_DRAGGABLE;
}

void glimmer::DraggableSystem::Render(SDL_Renderer *renderer) {
    const AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    auto uiScale = appContext->GetConfig()->window.uiScale;
    if (item_ != nullptr) {
        float mouseX, mouseY = 0;
        SDL_GetMouseState(&mouseX, &mouseY);
        const SDL_FRect dst = {mouseX + 6, mouseY + 24, ITEM_SLOT_SIZE * uiScale, ITEM_SLOT_SIZE * uiScale};
        SDL_Texture *texture = item_.get()->GetIcon();
        if (texture != nullptr) {
            SDL_RenderTexture(renderer, texture, nullptr, &dst);
        }
    }

    if (appContext->GetConfig()->debug.displayDraggableTarget) {
        CameraComponent *cameraComponent = worldContext_->GetCameraComponent();
        if (cameraComponent == nullptr) {
            return;
        }
        Transform2DComponent *cameraTransformComponent = worldContext_->GetCameraTransform2D();
        if (cameraTransformComponent == nullptr) {
            return;
        }
        const std::vector<GameEntity::ID> draggableEntity = worldContext_->GetEntityIDWithComponents<
            DraggableComponent>();
        if (draggableEntity.empty()) {
            return;
        }

        Color draggableColor = appContext->GetPreloadColors()->debugColor.draggableColor;
        SDL_SetRenderDrawColor(renderer, draggableColor.r, draggableColor.g, draggableColor.b, draggableColor.a);
        for (uint32_t entity: draggableEntity) {
            SDL_FRect border = DraggableBorder(entity, cameraTransformComponent->GetPosition(), cameraComponent);
            if (border.h <= 0 || border.w <= 0) {
                continue;
            }
            SDL_RenderRect(renderer, &border);
        }
    }
    AppContext::RestoreColorRenderer(renderer);
}

bool glimmer::DraggableSystem::HandleEvent(const SDL_Event &event) {
    if (worldContext_ == nullptr) {
        return false;
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
        CameraComponent *cameraComponent = worldContext_->GetCameraComponent();
        if (cameraComponent == nullptr) {
            return false;
        }
        Transform2DComponent *cameraTransformComponent = worldContext_->GetCameraTransform2D();
        if (cameraTransformComponent == nullptr) {
            return false;
        }
        const std::vector<GameEntity::ID> draggableEntity = worldContext_->GetEntityIDWithComponents<
            DraggableComponent>();
        if (draggableEntity.empty()) {
            return false;
        }
        bool changed = false;
        for (uint32_t entity: draggableEntity) {
            const DraggableComponent *draggableComponent = worldContext_->GetComponent<DraggableComponent>(entity);
            if (draggableComponent == nullptr) {
                continue;
            }
            const SDL_FRect border = DraggableBorder(entity, cameraTransformComponent->GetPosition(),
                                                     cameraComponent);
            if (border.h <= 0 || border.w <= 0) {
                continue;
            }
            float mouseX, mouseY = 0;
            SDL_GetMouseState(&mouseX, &mouseY);
            if (mouseX >= border.x && mouseX <= border.x + border.w &&
                mouseY >= border.y && mouseY <= border.y + border.h) {
                const ItemSlotComponent *itemSlotComponent = worldContext_->GetComponent<ItemSlotComponent>(entity);
                if (itemSlotComponent != nullptr) {
                    item_ = std::move(itemSlotComponent->ReplaceItem(std::move(item_)));
                    changed = true;
                }
                break;
            }
        }
        if (!changed) {
            return DropItem();
        }
        return false;
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_RIGHT) {
        return DropItem();
    }
    return false;
}

std::string glimmer::DraggableSystem::GetName() {
    return "glimmer::DraggableSystem";
}
