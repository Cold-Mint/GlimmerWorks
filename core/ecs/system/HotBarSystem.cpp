//
// Created by Cold-Mint on 2025/12/18.
//

#include "HotBarSystem.h"
#include "../component/HotBarComonent.h"
#include "../../world/WorldContext.h"
#include "../component/ItemSlotComponent.h"
#include "core/ecs/component/GuiTransform2DComponent.h"
#include "core/ecs/component/PlayerComponent.h"


glimmer::HotBarSystem::HotBarSystem(WorldContext *worldContext)
    : GameSystem(worldContext) {
    RequireComponent<PlayerComponent>();
    RequireComponent<ItemContainerComponent>();
    RequireComponent<HotBarComponent>();
    RequireComponent<GuiTransform2DComponent>();
}

bool glimmer::HotBarSystem::HandleEvent(const SDL_Event &event) {
    if (worldContext_ == nullptr) {
        return false;
    }
    if (event.type == SDL_EVENT_MOUSE_WHEEL) {
        const auto hotBarComponent = worldContext_->GetComponent<HotBarComponent>(worldContext_->GetHotBarEntity());
        if (hotBarComponent == nullptr) {
            return false;
        }

        auto &slotEntityList = hotBarComponent->GetSlotEntities();
        int total = slotEntityList.size();
        if (total == 0) {
            return false;
        }
        ItemSlotComponent *previousItemSlot = nullptr;
        ItemSlotComponent *currentItemSlot = nullptr;
        ItemSlotComponent *nextItemSlot = nullptr;
        for (int i = 0; i < slotEntityList.size(); ++i) {
            const auto entityId = slotEntityList[i];
            if (WorldContext::IsEmptyEntityId(entityId)) {
                continue;
            }
            auto *itemSlot = worldContext_->GetComponent<ItemSlotComponent>(entityId);
            if (itemSlot == nullptr) {
                continue;
            }
            if (itemSlot->IsSelected()) {
                currentItemSlot = itemSlot;
                int prevIdx = (i - 1 + total) % total;
                int nextIdx = (i + 1) % total;
                previousItemSlot = worldContext_->GetComponent<ItemSlotComponent>(slotEntityList[prevIdx]);
                nextItemSlot = worldContext_->GetComponent<ItemSlotComponent>(slotEntityList[nextIdx]);
                break;
            }
        }
        if (event.wheel.y > 0) {
            if (currentItemSlot != nullptr) {
                currentItemSlot->
                SetSelected(false);
            }
            if (previousItemSlot != nullptr) {
                previousItemSlot->SetSelected(true);
            }
        } else if (event.wheel.y < 0) {
            if (currentItemSlot != nullptr) {
                currentItemSlot->SetSelected(false);
            }
            if (nextItemSlot != nullptr) {
                nextItemSlot->SetSelected(true);
            }
        }
        return true;
    }
    return false;
}


uint8_t glimmer::HotBarSystem::GetRenderOrder() {
    return RENDER_ORDER_DEBUG_HOTBAR;
}

std::string glimmer::HotBarSystem::GetName() {
    return "glimmer.HotBarSystem";
}
