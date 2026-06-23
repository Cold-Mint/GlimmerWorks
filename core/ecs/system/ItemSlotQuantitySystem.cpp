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
#include "ItemSlotQuantitySystem.h"

#include "core/math/CoordinateTransformer.h"
#include "core/world/WorldContext.h"

glimmer::ItemSlotQuantitySystem::ItemSlotQuantitySystem(WorldContext* worldContext)
    : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_ITEM_SLOT_QUANTITY);
    appContext_ = worldContext->GetAppContext();
    const ResourceLocator* resourceLocator = appContext_->GetResourceLocator();
    ResourceRef itemSlotResourceRef;
    itemSlotResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    itemSlotResourceRef.SetResourceType(RESOURCE_TEXTURE);
    itemSlotResourceRef.SetResourceKey("gui/item_slot");
    itemSlotTexture_ = resourceLocator->FindTexture(&itemSlotResourceRef);
    resourcePackManager_ = appContext_->GetResourcePackManager();
    preloadColors_ = appContext_->GetPreloadColors();
    Init();
}

uint8_t glimmer::ItemSlotQuantitySystem::GetRenderOrder()
{
    return RENDER_ORDER_ITEM_SLOT_QUANTITY;
}

void glimmer::ItemSlotQuantitySystem::OnConfigChanged(const Config* config)
{
    uiScale_ = config->window.uiScale;
}

void glimmer::ItemSlotQuantitySystem::Render(SDL_Renderer* renderer)
{
    if (worldContext_ == nullptr || preloadColors_ == nullptr)
    {
        return;
    }
    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    const Item* hoveredItem = nullptr;
    for (auto itemSlotComponent : itemSlotQuantityComponents_)
    {
        if (itemSlotComponent == nullptr)
        {
            continue;
        }
        if (!itemSlotComponent->IsVisible())
        {
            continue;
        }
        const ScreenVector2D& size = CoordinateTransformer::DesignToScreen(
            itemSlotComponent->GetSize(), uiScale_);
        const ScreenVector2D& position = CoordinateTransformer::DesignToScreen(
            itemSlotComponent->GetPosition(), uiScale_);
        const SDL_FRect rect = {position.x, position.y, size.x, size.y};
        bool isHovered = mouseX >= rect.x && mouseX <= rect.x + rect.w &&
            mouseY >= rect.y && mouseY <= rect.y + rect.h;

        itemSlotComponent->SetHovered(isHovered);
        if (itemSlotTexture_ != nullptr)
        {
            SDL_RenderTexture(renderer, itemSlotTexture_.get(), nullptr, &rect);
        }
        const Item* item = itemSlotComponent->GetItem();
        if (isHovered)
        {
            hoveredItem = item;
        }
        if (item == nullptr)
        {
            continue;
        }
        float padding = itemSlotComponent->GetPadding() * uiScale_;
        const SDL_FRect itemRect = {
            rect.x + padding, rect.y + padding, rect.w - padding * 2.0F,
            rect.h - padding * 2.0F
        };

        if (!item->IsUnbreakable())
        {
            const uint32_t totalDur = std::max(item->GetMaxDurability(), 1U);
            const uint32_t usedDur = item->GetUsedDurability();
            const uint32_t remainDur = usedDur >= totalDur ? 0U : totalDur - usedDur;
            float remainingDurabilityPercentage = static_cast<float>(remainDur) / static_cast<float>(totalDur);
            if (remainingDurabilityPercentage != 1.0F)
            {
                Color color;
                if (remainingDurabilityPercentage >= 0.8F)
                {
                    color = preloadColors_->durability.durabilityGood;
                }
                else if (remainingDurabilityPercentage >= 0.6F)
                {
                    color = preloadColors_->durability.durabilityNotice;
                }
                else if (remainingDurabilityPercentage >= 0.1F)
                {
                    color = preloadColors_->durability.durabilityWarning;
                }
                else
                {
                    color = preloadColors_->durability.durabilityDanger;
                }
                const float durabilityBarHeight = itemRect.h * remainingDurabilityPercentage;
                const float barY = itemRect.y + itemRect.h - durabilityBarHeight;
                const SDL_FRect durabilityRect = {
                    itemRect.x,
                    barY,
                    itemRect.w,
                    durabilityBarHeight
                };
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                SDL_RenderFillRect(renderer, &durabilityRect);
            }
        }

        auto texture = item->GetIcon();
        if (texture != nullptr)
        {
            SDL_RenderTexture(renderer, texture, nullptr, &itemRect);
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

void glimmer::ItemSlotQuantitySystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType,
                                                                uint32_t count)
{
    if (gameComponentType == COMPONENT_ITEM_SLOT_QUANTITY)
    {
        auto entities_ = entityManager_->GetEntityIDWithComponents({COMPONENT_ITEM_SLOT_QUANTITY});
        for (auto entity : entities_)
        {
            auto itemSlotQuantityComponent = entityManager_->GetComponent<ItemSlotQuantityComponent>(entity);
            if (itemSlotQuantityComponent == nullptr)
            {
                continue;
            }
            itemSlotQuantityComponents_.emplace_back(itemSlotQuantityComponent);
        }
    }
}

glimmer::GameSystemType glimmer::ItemSlotQuantitySystem::GetGameSystemType() const
{
    return GameSystemType::ItemSlotQuantitySystem;
}
