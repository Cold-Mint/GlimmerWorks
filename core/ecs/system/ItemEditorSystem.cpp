//
// Created by Cold-Mint on 2026/1/23.
//

#include "ItemEditorSystem.h"

#include "core/ecs/component/HotBarComonent.h"
#include "core/ecs/component/ItemContainerComonent.h"
#include "core/ecs/component/ItemSlotComponent.h"
#include "core/inventory/ComposableItem.h"
#include "core/world/WorldContext.h"


bool glimmer::ItemEditorSystem::ShouldActivate() {
    return true;
}

glimmer::ItemEditorSystem::ItemEditorSystem(WorldContext *worldContext)
    : GameSystem(worldContext) {
}

bool glimmer::ItemEditorSystem::HandleEvent(const SDL_Event &event) {
    if (worldContext_ == nullptr) {
        return false;
    }
    if (!event.key.repeat && event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_E) {
        auto hotBar = worldContext_->GetHotBarEntity();
        if (WorldContext::IsEmptyEntityId(hotBar)) {
            return false;
        }
        auto hotBarComp = worldContext_->GetComponent<HotBarComponent>(hotBar);
        const auto player = worldContext_->GetPlayerEntity();
        if (WorldContext::IsEmptyEntityId(player)) {
            return false;
        }

        auto containerComp = worldContext_->GetComponent<ItemContainerComponent>(player);
        if (containerComp == nullptr) {
            return false;
        }
        auto itemContainer = containerComp->GetItemContainer();
        if (itemContainer == nullptr) {
            return false;
        }

        auto &slotEntityList = hotBarComp->GetSlotEntities();
        int total = slotEntityList.size();
        for (int i = 0; i < total; i++) {
            const auto entityId = slotEntityList[i];
            if (WorldContext::IsEmptyEntityId(entityId)) {
                continue;
            }
            auto *itemSlot = worldContext_->GetComponent<ItemSlotComponent>(entityId);
            if (itemSlot == nullptr) {
                continue;
            }
            if (itemSlot->IsSelected()) {
                Item *item = itemContainer->GetItem(i);
                auto composable = dynamic_cast<ComposableItem *>(item);
                if (composable == nullptr) {
                    worldContext_->HideItemEditorPanel();
                    return false;
                }

                if (worldContext_->IsItemEditorPanelVisible()) {
                    worldContext_->HideItemEditorPanel();
                } else {
                    worldContext_->ShowItemEditorPanel(composable);
                }
                return true;
            }
        }
    }
    return false;
}

std::string glimmer::ItemEditorSystem::GetName() {
    return "glimmer.ItemEditorSystem";
}

uint8_t glimmer::ItemEditorSystem::GetRenderOrder() {
    return RENDER_ORDER_ITEM_EDITOR;
}
