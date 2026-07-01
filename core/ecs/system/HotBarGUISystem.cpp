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
#include "HotBarGUISystem.h"
#include "../component/HotBarComponent.h"
#include "../../world/WorldContext.h"
#include "core/ecs/component/ItemSlotComponent.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/layout/HorizontalLayoutStepper.h"


void glimmer::HotBarGUISystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    if (gameComponentType == COMPONENT_HOT_BAR && hotBarComponent_ == nullptr)
    {
        hotBarComponent_ = entityShortCut_->GetHotBarComponent();
    }
    if (gameComponentType == COMPONENT_PLAYER && playerComponent_ == nullptr)
    {
        const GameEntityID player = entityShortCut_->GetPlayer();
        if (!WorldContext::IsEmptyEntityId(player))
        {
            playerComponent_ = entityManager_->GetComponent<PlayerComponent>(player);
        }
    }
    if (gameComponentType == COMPONENT_ITEM_CONTAINER && itemContainerComponent_ == nullptr)
    {
        itemContainerComponent_ = entityShortCut_->GetItemContainerComponent();
    }
    if (gameComponentType == COMPONENT_ITEM_SLOT)
    {
        auto entities = entityManager_->GetEntityIDWithComponents({COMPONENT_ITEM_SLOT});
        hotBarItemSlot_.clear();
        for (auto entity : entities)
        {
            auto itemSlotComponent = entityManager_->GetComponent<ItemSlotComponent>(entity);
            if (itemSlotComponent == nullptr)
            {
                continue;
            }
            if (itemSlotComponent->GetItemSlotType() == ItemSlotType::HotBar)
            {
                hotBarItemSlot_.emplace_back(itemSlotComponent);
            }
        }
        std::sort(hotBarItemSlot_.begin(), hotBarItemSlot_.end(),
                  [](const ItemSlotComponent* lhs, const ItemSlotComponent* rhs)
                  {
                      return lhs->GetSlotIndex() < rhs->GetSlotIndex();
                  });
    }
}


void glimmer::HotBarGUISystem::AfterSelectItemSlot(const uint8_t selectedSlotIndex) const
{
    if (playerComponent_ != nullptr && itemContainerComponent_ != nullptr)
    {
        const ItemContainer* container = itemContainerComponent_->GetItemContainer();
        if (container != nullptr)
        {
            playerComponent_->SetItem(container->GetItem(selectedSlotIndex));
        }
    }
    if (selectedSlotIndex < hotBarItemSlot_.size())
    {
        hotBarComponent_->SetSelectedSlotComponent(hotBarItemSlot_[selectedSlotIndex]);
    }
}

void glimmer::HotBarGUISystem::OnActivationChanged(bool activeStatus)
{
    if (activeStatus)
    {
        for (auto hotBarItemSlot : hotBarItemSlot_)
        {
            hotBarItemSlot->Show();
        }
    }
    else
    {
        for (auto hotBarItemSlot : hotBarItemSlot_)
        {
            hotBarItemSlot->Hide();
        }
    }
}

glimmer::HotBarGUISystem::HotBarGUISystem(WorldContext* worldContext) : GUISystem(worldContext)
{
    WatchComponent(COMPONENT_PLAYER);
    WatchComponent(COMPONENT_ITEM_CONTAINER);
    WatchComponent(COMPONENT_HOT_BAR);
    WatchComponent(COMPONENT_ITEM_SLOT);
    AfterSelectItemSlot(hotBarComponent_->GetSelectedSlot());
    Init();
}

void glimmer::HotBarGUISystem::OnWindowSizeChanged(int width, int height)
{
    size_t hotBarItemSlotSize = hotBarItemSlot_.size();
    if (hotBarItemSlotSize == 0)
    {
        return;
    }
    DesignDimension padding = 5;
    DesignVector2D hotBarStartPosition{padding, padding};
    auto horizontalLayoutStepper = HorizontalLayoutStepper(ITEM_SLOT_SIZE, hotBarStartPosition, padding,
                                                           hotBarItemSlotSize);
    int hotBarIndex = 0;
    while (horizontalLayoutStepper.HasNext())
    {
        if (hotBarIndex >= hotBarItemSlotSize)
        {
            break;
        }
        hotBarItemSlot_.at(hotBarIndex)->SetPosition(horizontalLayoutStepper.Next());
        hotBarIndex++;
    }
}

bool glimmer::HotBarGUISystem::HandleEvent(const SDL_Event& event)
{
    if (worldContext_ == nullptr)
    {
        return false;
    }
    if (hotBarComponent_ == nullptr)
    {
        return false;
    }
    const uint8_t preSelectIndex = hotBarComponent_->GetSelectedSlot();
    if (event.type == SDL_EVENT_MOUSE_WHEEL)
    {
        if (event.wheel.y > 0)
        {
            hotBarComponent_->SelectPreviousSlot();
        }
        else if (event.wheel.y < 0)
        {
            hotBarComponent_->SelectNextSlot();
        }
    }
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        if (event.key.scancode == SDL_SCANCODE_1)
        {
            hotBarComponent_->SetSelectedSlot(0);
        }
        if (event.key.scancode == SDL_SCANCODE_2)
        {
            hotBarComponent_->SetSelectedSlot(1);
        }
        if (event.key.scancode == SDL_SCANCODE_3)
        {
            hotBarComponent_->SetSelectedSlot(2);
        }
        if (event.key.scancode == SDL_SCANCODE_4)
        {
            hotBarComponent_->SetSelectedSlot(3);
        }
        if (event.key.scancode == SDL_SCANCODE_5)
        {
            hotBarComponent_->SetSelectedSlot(4);
        }
        if (event.key.scancode == SDL_SCANCODE_6)
        {
            hotBarComponent_->SetSelectedSlot(5);
        }
        if (event.key.scancode == SDL_SCANCODE_7)
        {
            hotBarComponent_->SetSelectedSlot(6);
        }
        if (event.key.scancode == SDL_SCANCODE_8)
        {
            hotBarComponent_->SetSelectedSlot(7);
        }
        if (event.key.scancode == SDL_SCANCODE_9)
        {
            hotBarComponent_->SetSelectedSlot(8);
        }
    }
    const uint8_t afterSelectIndex = hotBarComponent_->GetSelectedSlot();
    if (afterSelectIndex != preSelectIndex)
    {
        AfterSelectItemSlot(afterSelectIndex);
        return true;
    }
    return false;
}

glimmer::GameSystemType glimmer::HotBarGUISystem::GetGameSystemType() const
{
    return GameSystemType::HotBarGUISystem;
}
