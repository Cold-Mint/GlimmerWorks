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
#include "CraftPreviewSlotSystem.h"
#include "fmt/format.h"

#include "core/math/CoordinateTransformer.h"
#include "core/world/WorldContext.h"

glimmer::CraftPreviewSlotSystem::CraftPreviewSlotSystem(WorldContext* worldContext)
    : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_CRAFT_PREVIEW);
    appContext_ = worldContext->GetAppContext();
    const ResourceLocator* resourceLocator = appContext_->GetResourceLocator();
    ResourceRef craftPreviewSlotResourceRef;
    craftPreviewSlotResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    craftPreviewSlotResourceRef.SetResourceType(RESOURCE_TEXTURE);
    craftPreviewSlotResourceRef.SetResourceKey("gui/craft_preview_slot");
    craftPreviewSlotTextureResult_ = resourceLocator->FindTexture(&craftPreviewSlotResourceRef);
    preloadColors_ = appContext_->GetPreloadColors();
    resourcePackManager_ = appContext_->GetResourcePackManager();
    Init();
}

void glimmer::CraftPreviewSlotSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType,
                                                                uint32_t count)
{
    if (gameComponentType == COMPONENT_CRAFT_PREVIEW)
    {
        auto entities_ = entityManager_->GetEntityIDWithComponents({COMPONENT_CRAFT_PREVIEW});
        for (auto entity : entities_)
        {
            auto craftPreviewSlotComponent = entityManager_->GetComponent<CraftPreviewSlotComponent>(entity);
            if (craftPreviewSlotComponent == nullptr)
            {
                continue;
            }
            craftPreviewSlotComponents_.emplace_back(craftPreviewSlotComponent);
        }
    }
}

void glimmer::CraftPreviewSlotSystem::OnConfigChanged(const Config* config)
{
    uiScale_ = config->window.uiScale;
}

void glimmer::CraftPreviewSlotSystem::Render(SDL_Renderer* renderer)
{
    if (worldContext_ == nullptr || preloadColors_ == nullptr)
    {
        return;
    }
    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    const Item* hoveredItem = nullptr;
    hoveredCraftPreviewSlotComponent_ = nullptr;
    for (auto craftPreviewSlotComponent : craftPreviewSlotComponents_)
    {
        if (craftPreviewSlotComponent == nullptr)
        {
            continue;
        }
        if (!craftPreviewSlotComponent->IsVisible())
        {
            continue;
        }
        const ScreenVector2D& size = CoordinateTransformer::DesignToScreen(
            craftPreviewSlotComponent->GetSize(), uiScale_);
        const ScreenVector2D& position = CoordinateTransformer::DesignToScreen(
            craftPreviewSlotComponent->GetPosition(), uiScale_);
        const SDL_FRect rect = {position.x, position.y, size.x, size.y};
        bool isHovered = mouseX >= rect.x && mouseX <= rect.x + rect.w &&
            mouseY >= rect.y && mouseY <= rect.y + rect.h;
        craftPreviewSlotComponent->SetHovered(isHovered);
        if (craftPreviewSlotTextureResult_ != nullptr)
        {
            SDL_Texture* craftPreviewSlotTexture = craftPreviewSlotTextureResult_->GetResource();
            if (craftPreviewSlotTexture != nullptr)
            {
                SDL_RenderTexture(renderer, craftPreviewSlotTexture, nullptr, &rect);
            }
        }


        const Item* item = craftPreviewSlotComponent->GetItem();
        if (isHovered)
        {
            hoveredItem = item;
            hoveredCraftPreviewSlotComponent_ = craftPreviewSlotComponent;
        }
        if (item == nullptr)
        {
            continue;
        }
        float padding = craftPreviewSlotComponent->GetPadding() * uiScale_;
        const SDL_FRect itemRect = {
            rect.x + padding, rect.y + padding, rect.w - padding * 2.0F,
            rect.h - padding * 2.0F
        };

        auto iconTexture = item->GetIcon();
        if (iconTexture != nullptr)
        {
            SDL_RenderTexture(renderer, iconTexture, nullptr, &itemRect);
        }
        uint8_t amount = item->GetAmount();
        if (amount > 1)
        {
            auto amountIterator = numberTextures_.find(amount);
            SDL_Texture* amountTexture = nullptr;
            if (amountIterator == numberTextures_.end())
            {
                std::shared_ptr<SDL_Texture> amountTexturePtr = resourcePackManager_->CreateStringTexture(
                    std::to_string(amount), &preloadColors_->game.itemSlotTextColor);
                numberTextures_[amount] = amountTexturePtr;
                amountTexture = amountTexturePtr.get();
            }
            else
            {
                amountTexture = amountIterator->second.get();
            }

            const float textOffset = 2.0F * uiScale_;
            float textW = static_cast<float>(itemRect.w) * 0.5F;
            if (amount < 10)
            {
                //Reduce the single number by half again
                //单个数字再减少一半
                textW = textW * 0.5F;
            }
            const float textH = static_cast<float>(itemRect.h) * 0.5F;
            SDL_FRect dst = {
                rect.x + rect.w - textW - textOffset,
                rect.y + rect.h - textH - textOffset, textW,
                textH
            };
            SDL_RenderTexture(renderer, amountTexture, nullptr, &dst);
        }
    }
    if (entityShortCut_ != nullptr)
    {
        ItemToolTipComponent* itemToolTipComponent =
            entityShortCut_->GetItemToolTipComponent();
        if (itemToolTipComponent != nullptr)
        {
            itemToolTipComponent->SetItem(hoveredItem);
            itemToolTipComponent->SetPosition({mouseX, mouseY});
        }
    }
    AppContext::RestoreColorRenderer(renderer);
}

bool glimmer::CraftPreviewSlotSystem::HandleEvent(const SDL_Event& event)
{
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        if (hoveredCraftPreviewSlotComponent_ == nullptr)
        {
            return false;
        }
        entityShortCut_->SetSelectedCraftPreviewSlotComponent(hoveredCraftPreviewSlotComponent_);
        worldContext_->PushGuiSystemType(GameSystemType::MaterialSelectCraftUISystem);
        return true;
    }
    return false;
}

uint8_t glimmer::CraftPreviewSlotSystem::GetRenderOrder()
{
    return RENDER_ORDER_CRAFT_PREVIEW_SLOT;
}

glimmer::GameSystemType glimmer::CraftPreviewSlotSystem::GetGameSystemType() const
{
    return GameSystemType::CraftPreviewSlotSystem;
}
