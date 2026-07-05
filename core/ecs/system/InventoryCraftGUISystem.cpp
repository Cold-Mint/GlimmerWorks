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
#include "core/utils/StringUtils.h"


glimmer::InventoryCraftGUISystem::InventoryCraftGUISystem(WorldContext* worldContext)
    : GUISystem(worldContext)
{
    WatchComponent(COMPONENT_ITEM_SLOT);
    WatchComponent(COMPONENT_ITEM_CONTAINER);
    WatchComponent(COMPONENT_PLAYER);
    const AppContext* appContext = worldContext->GetAppContext();
    if (appContext == nullptr)
    {
        return;
    }
    langsResources_ = appContext->GetLangsResources();
    preloadColors_ = appContext->GetPreloadColors();
    recipeManager_ = appContext->GetRecipeManager();
    resourcePackManager_ = appContext->GetResourcePackManager();
    Init();
}

void glimmer::InventoryCraftGUISystem::Render(SDL_Renderer* renderer)
{
    if (unlockedRecipesSize_ == 0)
    {
        return;
    }
    DesignDimension padding = 5;
    const std::string& craftInfo = langsResources_->craft;
    if (const uint64_t craftFingerprint = StringUtils::StringToUint64(craftInfo); craftFingerprint_ != craftFingerprint)
    {
        craftTexture_ = resourcePackManager_->CreateStringTexture(craftInfo, &preloadColors_->textColor);
    }
    const SDL_FRect dstRect{
        padding * uiScale_,
        ITEM_SLOT_SIZE * 5 * uiScale_,
        static_cast<float>(craftTexture_->w) * uiScale_,
        static_cast<float>(craftTexture_->h) * uiScale_
    };
    SDL_RenderTexture(renderer, craftTexture_.get(), nullptr, &dstRect);
}

void glimmer::InventoryCraftGUISystem::OnActivationChanged(bool activeStatus)
{
    EntityManager* entityManager = GetEntityManager();
    WorldContext* worldContext = GetWorldContext();
    if (activeStatus)
    {
        for (auto inventoryItemSlot : inventoryItemSlot_)
        {
            inventoryItemSlot->Show();
        }
        std::vector<RecipeResource*> recipeResources = recipeManager_->FindUnlockedRecipes(
            playerComponent_->GetTechnologyMap(), itemContainer_->GetTotalTags());
        DesignDimension padding = 5;
        unlockedRecipesSize_ = recipeResources.size();
        DesignVector2D craftPreviewSlotStartPosition{padding, ITEM_SLOT_SIZE * 6};
        auto gridLayoutStepper = GridLayoutStepper(ITEM_SLOT_SIZE, craftPreviewSlotStartPosition, HOT_BAR_SIZE, padding,
                                                   unlockedRecipesSize_);
        for (int i = 0; i < unlockedRecipesSize_; i++)
        {
            CraftPreviewSlotComponent* craftPreviewSlotComponent;
            if (i == craftPreviewSlot_.size())
            {
                auto craftPreviewSlotEntity = entityManager->AddEntity();
                craftPreviewSlotComponent = entityManager->AddComponent<CraftPreviewSlotComponent>(
                    craftPreviewSlotEntity);
                craftPreviewSlot_.emplace_back(craftPreviewSlotComponent);
            }
            else
            {
                craftPreviewSlotComponent = craftPreviewSlot_[i];
            }
            craftPreviewSlotComponent->SetSize({ITEM_SLOT_SIZE, ITEM_SLOT_SIZE});
            const DesignVector2D& position = gridLayoutStepper.Next();
            craftPreviewSlotComponent->SetPosition(position);
            craftPreviewSlotComponent->SetRecipeResource(worldContext, recipeResources[i]);
            craftPreviewSlotComponent->Show();
        }
        if (craftPreviewSlot_.size() > unlockedRecipesSize_)
        {
            for (int i = static_cast<int>(unlockedRecipesSize_); i < craftPreviewSlot_.size(); i++)
            {
                CraftPreviewSlotComponent* craftPreviewSlotComponent = craftPreviewSlot_[i];
                if (craftPreviewSlotComponent == nullptr)
                {
                    continue;
                }
                craftPreviewSlotComponent->Hide();
            }
        }
    }
    else
    {
        for (auto inventoryItemSlot : inventoryItemSlot_)
        {
            inventoryItemSlot->Hide();
        }
        for (int i = 0; i < craftPreviewSlot_.size(); i++)
        {
            CraftPreviewSlotComponent* craftPreviewSlotComponent = craftPreviewSlot_[i];
            if (craftPreviewSlotComponent == nullptr)
            {
                continue;
            }
            craftPreviewSlotComponent->Hide();
        }
    }
}

void glimmer::InventoryCraftGUISystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType,
                                                                 uint32_t count)
{
    EntityManager* entityManager = GetEntityManager();
    EntityShortCut* entityShortCut = GetEntityShortCut();
    if (gameComponentType == COMPONENT_ITEM_SLOT)
    {
        auto entities = entityManager->GetEntityIDWithComponents({COMPONENT_ITEM_SLOT});
        inventoryItemSlot_.clear();
        for (auto entity : entities)
        {
            auto itemSlotComponent = entityManager->GetComponent<ItemSlotComponent>(entity);
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
    if (gameComponentType == COMPONENT_PLAYER)
    {
        auto playerEntity = entityShortCut->GetPlayer();
        if (WorldContext::IsEmptyEntityId(playerEntity))
        {
            return;
        }
        playerComponent_ = entityManager->GetComponent<PlayerComponent>(playerEntity);
    }
    if (gameComponentType == COMPONENT_ITEM_CONTAINER)
    {
        auto* itemContainerComponent = entityShortCut->GetItemContainerComponent();
        if (itemContainerComponent == nullptr)
        {
            return;
        }
        itemContainer_ = itemContainerComponent->GetItemContainer();
    }
}

glimmer::GameSystemType glimmer::InventoryCraftGUISystem::GetGameSystemType() const
{
    return GameSystemType::InventoryCraftGUISystem;
}

uint8_t glimmer::InventoryCraftGUISystem::GetRenderOrder()
{
    return RENDER_ORDER_INVENTORY_CRAFT_GUI;
}

void glimmer::InventoryCraftGUISystem::OnConfigChanged(const Config* config)
{
    uiScale_ = config->window.uiScale;
}

void glimmer::InventoryCraftGUISystem::OnWindowSizeChanged(const int& width, const int& height)
{
    const size_t inventoryItemSlotSize = inventoryItemSlot_.size();
    if (inventoryItemSlotSize == 0)
    {
        return;
    }
    DesignDimension padding = 5;
    DesignVector2D hotBarStartPosition{padding, padding};
    auto gridLayoutStepper = GridLayoutStepper(ITEM_SLOT_SIZE, hotBarStartPosition, HOT_BAR_SIZE, padding,
                                               inventoryItemSlotSize);
    int index = 0;
    while (gridLayoutStepper.HasNext())
    {
        if (index >= inventoryItemSlotSize)
        {
            break;
        }
        inventoryItemSlot_.at(index)->SetPosition(gridLayoutStepper.Next());
        index++;
    }
}
