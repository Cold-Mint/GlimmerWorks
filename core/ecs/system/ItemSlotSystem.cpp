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
#include "ItemSlotSystem.h"
#include "../../inventory/Item.h"
#include "../../world/WorldContext.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <SDL3/SDL.h>
#include <string>
#include <algorithm>
#include "fmt/format.h"

#include "core/ecs/component/ItemSlotComponent.h"
#include "core/math/CoordinateTransformer.h"
#include "core/world/PreloadColors.h"

void glimmer::ItemSlotSystem::Render(SDL_Renderer* renderer)
{
    if (worldContext_ == nullptr || preloadColors_ == nullptr || font_ == nullptr)
    {
        return;
    }
    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    const Item* hoveredItem = nullptr;
    for (auto itemSlotComponent : itemSlotComponents_)
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
        if (hotBarComponent_ != nullptr && hotBarComponent_->GetSelectedSlotComponent() == itemSlotComponent)
        {
            if (itemSlotSelectedTexture_ != nullptr)
            {
                SDL_RenderTexture(renderer, itemSlotSelectedTexture_.get(), nullptr, &rect);
            }
        }
        else
        {
            if (itemSlotTexture_ != nullptr)
            {
                SDL_RenderTexture(renderer, itemSlotTexture_.get(), nullptr, &rect);
            }
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
        size_t amount = item->GetAmount();
        if (amount > 1)
        {
            std::string text = std::to_string(amount);
            SDL_Color color = {255, 255, 255, 255};
            SDL_Surface* surface =
                TTF_RenderText_Blended(font_, text.c_str(), text.length(), color);
            if (surface)
            {
                SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(renderer, surface);
                if (sdlTexture != nullptr)
                {
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

                    SDL_RenderTexture(renderer, sdlTexture, nullptr, &dst);
                    SDL_DestroyTexture(sdlTexture);
                }
                SDL_DestroySurface(surface);
            }
        }
    }
    if (hoveredItem)
    {
        RenderTooltip(renderer, hoveredItem);
    }
    AppContext::RestoreColorRenderer(renderer);
}


uint8_t glimmer::ItemSlotSystem::GetRenderOrder()
{
    return RENDER_ORDER_ITEM_SLOT;
}

glimmer::GameSystemType glimmer::ItemSlotSystem::GetGameSystemType() const
{
    return GameSystemType::ItemSlotSystem;
}

void glimmer::ItemSlotSystem::RenderQuantity(SDL_Renderer* renderer, const SDL_FRect& slotDest, int amount) const
{
    const std::string text = std::to_string(amount);
    AppContext* appContext = worldContext_->GetAppContext();
    SDL_Surface* surface = TTF_RenderText_Blended(appContext->GetFont(), text.c_str(), text.length(),
                                                  appContext->GetPreloadColors()->textColor.ToSDLColor());
    if (surface)
    {
        if (SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface))
        {
            auto textW = static_cast<float>(surface->w);
            auto textH = static_cast<float>(surface->h);
            SDL_FRect dst = {
                slotDest.x + slotDest.w - textW - 2,
                slotDest.y + slotDest.h - textH - 2,
                textW,
                textH
            };
            SDL_RenderTexture(renderer, texture, nullptr, &dst);
            SDL_DestroyTexture(texture);
        }
        SDL_DestroySurface(surface);
    }
}

void glimmer::ItemSlotSystem::RenderTooltip(SDL_Renderer* renderer, const Item* item) const
{
    if (item == nullptr)
    {
        return;
    }
    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    const std::string& nameInfo = item->GetName();
    const std::optional<std::string>& descInfo = item->GetDescription();
    const LangsResources* langsResources = appContext_->GetLangsResources();
    std::vector<SDL_Surface*> surfacesToDraw;

    // 渲染物品名称
    SDL_Surface* sName = TTF_RenderText_Blended(font_, nameInfo.c_str(), nameInfo.length(),
                                                preloadColors_->textColor.ToSDLColor());
    if (sName != nullptr)
    {
        surfacesToDraw.emplace_back(sName);
    }

    // 渲染物品描述
    if (descInfo.has_value())
    {
        const std::string& description = descInfo.value();
        SDL_Surface* sDesc = TTF_RenderText_Blended_Wrapped(font_, description.c_str(),
                                                            description.length(),
                                                            preloadColors_->textColor.ToSDLColor(), 0);
        if (sDesc != nullptr)
        {
            surfacesToDraw.push_back(sDesc);
        }
    }

    // 渲染能力属性
    const AbilityConfig* abilityConfig = item->GetAbilityConfig();
    if (abilityConfig != nullptr)
    {
        if (abilityConfig->mineAbleLayer & Ground)
        {
            SDL_Surface* sVar = TTF_RenderText_Blended_Wrapped(
                font_,
                langsResources->canMineBlockTip.c_str(),
                langsResources->canMineBlockTip.length(),
                preloadColors_->game.positiveAttributeColor.ToSDLColor(), TOOLTIP_TEXT_WRAP_WIDTH
            );
            if (sVar) surfacesToDraw.push_back(sVar);
        }
        if (abilityConfig->mineAbleLayer & BackGround)
        {
            SDL_Surface* sVar = TTF_RenderText_Blended_Wrapped(
                font_,
                langsResources->canMineWallTip.c_str(),
                langsResources->canMineWallTip.length(),
                preloadColors_->game.positiveAttributeColor.ToSDLColor(), TOOLTIP_TEXT_WRAP_WIDTH
            );
            if (sVar) surfacesToDraw.push_back(sVar);
        }
        if (abilityConfig->enablePrecisionMining)
        {
            SDL_Surface* sVar = TTF_RenderText_Blended_Wrapped(
                font_,
                langsResources->precisionMiningTip.c_str(),
                langsResources->precisionMiningTip.length(),
                preloadColors_->game.positiveAttributeColor.ToSDLColor(), TOOLTIP_TEXT_WRAP_WIDTH
            );
            if (sVar) surfacesToDraw.push_back(sVar);
        }

        // 挖掘效率
        float miningEfficiency = abilityConfig->miningEfficiency;
        if (miningEfficiency != 0.0F)
        {
            std::string tip = fmt::format(
                fmt::runtime(langsResources->efficiencyTip),
                fmt::format("{0:+.0f}", miningEfficiency * 100)
            );
            SDL_Surface* sVar = TTF_RenderText_Blended_Wrapped(
                font_, tip.c_str(), tip.length(),
                miningEfficiency > 0
                    ? preloadColors_->game.positiveAttributeColor.ToSDLColor()
                    : preloadColors_->game.negativeAttributeColor.ToSDLColor(),
                TOOLTIP_TEXT_WRAP_WIDTH
            );
            if (sVar) surfacesToDraw.push_back(sVar);
        }

        // 失误概率
        float fumbleProbability = abilityConfig->fumbleProbability;
        if (fumbleProbability != 0.0F)
        {
            std::string tip = fmt::format(
                fmt::runtime(langsResources->fumbleTip),
                (fumbleProbability > 0 ? "+" : "") + fmt::format("{0:.0f}", fumbleProbability * 100));
            SDL_Surface* sVar = TTF_RenderText_Blended_Wrapped(
                font_, tip.c_str(), tip.length(),
                fumbleProbability > 0
                    ? preloadColors_->game.negativeAttributeColor.ToSDLColor()
                    : preloadColors_->game.positiveAttributeColor.ToSDLColor(),
                TOOLTIP_TEXT_WRAP_WIDTH
            );
            if (sVar) surfacesToDraw.push_back(sVar);
        }

        // 连锁挖掘半径
        int chainMiningRadius = abilityConfig->chainMiningRadius;
        if (chainMiningRadius != 0)
        {
            std::string tip = fmt::format(fmt::runtime(langsResources->chainMiningTip),
                                          (chainMiningRadius > 0 ? "+" : "") + std::to_string(chainMiningRadius));
            SDL_Surface* sVar = TTF_RenderText_Blended_Wrapped(
                font_, tip.c_str(), tip.length(),
                chainMiningRadius > 0
                    ? preloadColors_->game.positiveAttributeColor.ToSDLColor()
                    : preloadColors_->game.negativeAttributeColor.ToSDLColor(),
                TOOLTIP_TEXT_WRAP_WIDTH
            );
            if (sVar) surfacesToDraw.push_back(sVar);
        }
    }

    constexpr float baseLineSpacing = 2.0F; // 基础行间距（未缩放）
    constexpr float basePadding = 8.0F; // 基础内边距（未缩放）
    const float scaledLineSpacing = baseLineSpacing * uiScale_;
    const float scaledPadding = basePadding * uiScale_;

    float maxWidth = 0.0F;
    float totalTextHeight = 0.0F;
    const size_t surfaceCount = surfacesToDraw.size();

    for (size_t i = 0; i < surfaceCount; ++i)
    {
        SDL_Surface* surf = surfacesToDraw[i];
        if (!surf) continue;

        maxWidth = std::max(maxWidth, static_cast<float>(surf->w));
        totalTextHeight += static_cast<float>(surf->h);

        if (i != surfaceCount - 1)
        {
            totalTextHeight += scaledLineSpacing;
        }
    }

    const SDL_FRect backgroundRect{
        mouseX,
        mouseY,
        (maxWidth + 2 * basePadding) * uiScale_, // 左右padding
        (totalTextHeight + 2 * basePadding) * uiScale_ // 上下padding
    };

    if (tooltipBgTexture_ != nullptr)
    {
        SDL_RenderTexture(renderer, tooltipBgTexture_.get(), nullptr, &backgroundRect);
    }
    float currentX = backgroundRect.x + scaledPadding;
    float currentY = backgroundRect.y + scaledPadding;

    for (auto* surf : surfacesToDraw)
    {
        if (!surf) continue;

        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, surf);
        if (textTexture)
        {
            const SDL_FRect dstRect{
                currentX,
                currentY,
                static_cast<float>(surf->w) * uiScale_,
                static_cast<float>(surf->h) * uiScale_
            };
            SDL_RenderTexture(renderer, textTexture, nullptr, &dstRect);
            SDL_DestroyTexture(textTexture);
        }
        currentY += static_cast<float>(surf->h) * uiScale_ + scaledLineSpacing;
        SDL_DestroySurface(surf);
    }
}

void glimmer::ItemSlotSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    if (gameComponentType == COMPONENT_HOT_BAR && hotBarComponent_ == nullptr)
    {
        hotBarComponent_ = entityShortCut_->GetHotBarComponent();
    }
    if (gameComponentType == COMPONENT_ITEM_SLOT)
    {
        auto entities_ = entityManager_->GetEntityIDWithComponents({COMPONENT_ITEM_SLOT});
        for (auto entity : entities_)
        {
            auto itemSlotComponent = entityManager_->GetComponent<ItemSlotComponent>(entity);
            if (itemSlotComponent == nullptr)
            {
                continue;
            }
            itemSlotComponents_.emplace_back(itemSlotComponent);
        }
    }
}

void glimmer::ItemSlotSystem::OnConfigChanged(const Config* config)
{
    uiScale_ = config->window.uiScale;
}


glimmer::ItemSlotSystem::ItemSlotSystem(WorldContext* worldContext)
    : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_ITEM_SLOT);
    WatchComponent(COMPONENT_HOT_BAR);
    appContext_ = worldContext->GetAppContext();
    const ResourceLocator* resourceLocator = appContext_->GetResourceLocator();
    ResourceRef itemSlotResourceRef;
    itemSlotResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    itemSlotResourceRef.SetResourceType(RESOURCE_TEXTURE);
    itemSlotResourceRef.SetResourceKey("gui/item_slot");
    itemSlotTexture_ = resourceLocator->FindTexture(&itemSlotResourceRef);
    ResourceRef itemSlotSelectedResourceRef;
    itemSlotSelectedResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    itemSlotSelectedResourceRef.SetResourceType(RESOURCE_TEXTURE);
    itemSlotSelectedResourceRef.SetResourceKey("gui/item_slot_selected");
    itemSlotSelectedTexture_ = resourceLocator->FindTexture(&itemSlotSelectedResourceRef);
    ResourceRef tooltipBgResourceRef;
    tooltipBgResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    tooltipBgResourceRef.SetResourceType(RESOURCE_TEXTURE);
    tooltipBgResourceRef.SetResourceKey("gui/tooltip_bg");
    tooltipBgTexture_ = resourceLocator->FindTexture(&tooltipBgResourceRef);
    preloadColors_ = appContext_->GetPreloadColors();
    font_ = appContext_->GetFont();
    const Config* config = appContext_->GetConfig();
    if (config == nullptr)
    {
        return;
    }
    uiScale_ = config->window.uiScale;
}
