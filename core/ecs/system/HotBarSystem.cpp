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
#include "HotBarSystem.h"
#include "../component/HotBarComonent.h"
#include "../../world/WorldContext.h"
#include "../component/ItemSlotComponent.h"
#include "core/ecs/component/GuiTransform2DComponent.h"
#include "core/ecs/component/PlayerComponent.h"


glimmer::HotBarSystem::HotBarSystem(WorldContext* worldContext)
    : GameSystem(worldContext)
{
    RequireComponent<PlayerComponent>();
    RequireComponent<ItemContainerComponent>();
    RequireComponent<HotBarComponent>();
    RequireComponent<GuiTransform2DComponent>();
}

bool glimmer::HotBarSystem::HandleEvent(const SDL_Event& event)
{
    if (worldContext_ == nullptr)
    {
        return false;
    }
    if (event.type == SDL_EVENT_MOUSE_WHEEL)
    {
        const auto hotBarComponent = worldContext_->GetComponent<HotBarComponent>(worldContext_->GetHotBarEntity());
        if (hotBarComponent == nullptr)
        {
            return false;
        }

        auto& slotEntityList = hotBarComponent->GetSlotEntities();
        int total = slotEntityList.size();
        if (total == 0)
        {
            return false;
        }
        ItemSlotComponent* previousItemSlot = nullptr;
        ItemSlotComponent* currentItemSlot = nullptr;
        int prevIdx = 0;
        int nextIdx = 0;
        ItemSlotComponent* nextItemSlot = nullptr;
        for (int i = 0; i < slotEntityList.size(); ++i)
        {
            const auto entityId = slotEntityList[i];
            if (WorldContext::IsEmptyEntityId(entityId))
            {
                continue;
            }
            auto* itemSlot = worldContext_->GetComponent<ItemSlotComponent>(entityId);
            if (itemSlot == nullptr)
            {
                continue;
            }
            if (itemSlot->IsSelected())
            {
                currentItemSlot = itemSlot;
                prevIdx = (i - 1 + total) % total;
                nextIdx = (i + 1) % total;
                previousItemSlot = worldContext_->GetComponent<ItemSlotComponent>(slotEntityList[prevIdx]);
                nextItemSlot = worldContext_->GetComponent<ItemSlotComponent>(slotEntityList[nextIdx]);
                break;
            }
        }
        int focusIndex = 0;
        if (event.wheel.y > 0)
        {
            if (currentItemSlot != nullptr)
            {
                currentItemSlot->
                    SetSelected(false);
            }
            if (previousItemSlot != nullptr)
            {
                previousItemSlot->SetSelected(true);
                focusIndex = prevIdx;
            }
        }
        else if (event.wheel.y < 0)
        {
            if (currentItemSlot != nullptr)
            {
                currentItemSlot->SetSelected(false);
            }
            if (nextItemSlot != nullptr)
            {
                nextItemSlot->SetSelected(true);
                focusIndex = nextIdx;
            }
        }
        GameEntity::ID playerId = worldContext_->GetPlayerEntity();
        auto playerComponent = worldContext_->GetComponent<PlayerComponent>(
            playerId);
        if (playerComponent != nullptr)
        {
            auto containerComponent = worldContext_->GetComponent<ItemContainerComponent>(playerId);
            if (containerComponent != nullptr)
            {
                const ItemContainer* container = containerComponent->GetItemContainer();
                if (container != nullptr)
                {
                    playerComponent->item = container->GetItem(focusIndex);
                }
            }
        }
        return true;
    }
    return false;
}


uint8_t glimmer::HotBarSystem::GetRenderOrder()
{
    return RENDER_ORDER_HOTBAR;
}

std::string glimmer::HotBarSystem::GetName()
{
    return "glimmer.HotBarSystem";
}
