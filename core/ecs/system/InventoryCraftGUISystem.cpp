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
#include "InventoryCraftGUISystem.h"

#include "core/ecs/ItemSlotType.h"
#include "core/world/WorldContext.h"
#include "core/ecs/component/ItemSlotComponent.h"
#include "core/layout/GridLayoutStepper.h"


glimmer::InventoryCraftGUISystem::InventoryCraftGUISystem(WorldContext* worldContext)
    : GUISystem(worldContext)
{
    WatchComponent(COMPONENT_ITEM_SLOT);
    const AppContext* appContext = worldContext->GetAppContext();
    if (appContext == nullptr)
    {
        return;
    }
    const Config* config = appContext->GetConfig();
    if (config == nullptr)
    {
        return;
    }
    uiScale_ = config->window.uiScale;
}

void glimmer::InventoryCraftGUISystem::OnActivationChanged(bool activeStatus)
{
    if (activeStatus)
    {
        for (auto inventoryItemSlot : inventoryItemSlot_)
        {
            inventoryItemSlot->Show();
        }
    }
    else
    {
        for (auto inventoryItemSlot : inventoryItemSlot_)
        {
            inventoryItemSlot->Hide();
        }
    }
}

void glimmer::InventoryCraftGUISystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType,
                                                                 uint32_t count)
{
    if (gameComponentType == COMPONENT_ITEM_SLOT)
    {
        auto entities = entityManager_->GetEntityIDWithComponents({COMPONENT_ITEM_SLOT});
        inventoryItemSlot_.clear();
        for (auto entity : entities)
        {
            auto itemSlotComponent = entityManager_->GetComponent<ItemSlotComponent>(entity);
            if (itemSlotComponent == nullptr)
            {
                continue;
            }
            if (itemSlotComponent->GetItemSlotType() == ItemSlotType::Inventory)
            {
                inventoryItemSlot_.emplace_back(itemSlotComponent);
            }
        }
        std::sort(inventoryItemSlot_.begin(), inventoryItemSlot_.end(),
                  [](const ItemSlotComponent* lhs, const ItemSlotComponent* rhs)
                  {
                      return lhs->GetSlotIndex() < rhs->GetSlotIndex();
                  });
    }
}

glimmer::GameSystemType glimmer::InventoryCraftGUISystem::GetGameSystemType() const
{
    return GameSystemType::InventoryCraftGUISystem;
}

void glimmer::InventoryCraftGUISystem::OnConfigChanged(const Config* config)
{
    uiScale_ = config->window.uiScale;
}

void glimmer::InventoryCraftGUISystem::OnWindowSizeChanged(int width, int height)
{
    DesignDimension padding = 5;
    size_t hotBarItemSlotSize = inventoryItemSlot_.size();
    DesignVector2D hotBarStartPosition{padding, padding};
    auto horizontalLayoutStepper = GridLayoutStepper(ITEM_SLOT_SIZE, hotBarStartPosition, HOT_BAR_SIZE, padding,
                                                     hotBarItemSlotSize);
    int index = 0;
    while (horizontalLayoutStepper.HasNext())
    {
        if (index >= hotBarItemSlotSize)
        {
            break;
        }
        inventoryItemSlot_.at(index)->SetPosition(horizontalLayoutStepper.Next());
        index++;
    }
}
