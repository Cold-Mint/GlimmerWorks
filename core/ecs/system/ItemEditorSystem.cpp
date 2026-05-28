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
