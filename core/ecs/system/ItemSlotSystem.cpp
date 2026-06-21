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
#include <SDL3/SDL.h>
#include <string>
#include <algorithm>
#include "fmt/format.h"

#include "core/ecs/component/ItemSlotComponent.h"
#include "core/math/CoordinateTransformer.h"
#include "core/utils/StringUtils.h"
#include "core/world/PreloadColors.h"

void glimmer::ItemSlotSystem::Render(SDL_Renderer* renderer)
{
    if (worldContext_ == nullptr || preloadColors_ == nullptr)
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

void glimmer::ItemSlotSystem::RenderTooltip(SDL_Renderer* renderer, const Item* item)
{
    if (item == nullptr)
    {
        return;
    }
    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    const LangsResources* langsResources = appContext_->GetLangsResources();
    std::vector<SDL_Texture*> textureToDraw;
    // 渲染物品名称
    const std::string& name = item->GetName();
    const uint64_t itemNameFingerprint = StringUtils::StringToUint64(name);
    if (itemNameFingerprint != itemNameFingerprint_)
    {
        itemNameTexture_ = resourcePackManager_->CreateStringTexture(name, &preloadColors_->textColor);
        itemNameFingerprint_ = itemNameFingerprint;
    }
    if (itemNameTexture_ != nullptr)
    {
        textureToDraw.emplace_back(itemNameTexture_.get());
    }
    const std::optional<std::string>& descriptionOptional = item->GetDescription();
    if (descriptionOptional.has_value())
    {
        const std::string& description = descriptionOptional.value();
        const uint64_t itemDescriptionFingerprint = StringUtils::StringToUint64(description);
        if (itemDescriptionFingerprint != itemDescriptionFingerprint_)
        {
            itemDescriptionTexture_ = resourcePackManager_->
                CreateStringTexture(description, &preloadColors_->textColor);
            itemDescriptionFingerprint_ = itemDescriptionFingerprint;
        }
        if (itemDescriptionTexture_ != nullptr)
        {
            textureToDraw.emplace_back(itemDescriptionTexture_.get());
        }
    }

    // 渲染能力属性
    const AbilityConfig* abilityConfig = item->GetAbilityConfig();
    if (abilityConfig != nullptr)
    {
        if (abilityConfig->mineAbleLayer & Ground)
        {
            uint64_t canMineBlockFingerprint = StringUtils::StringToUint64(langsResources->canMineBlockTip);
            if (canMineBlockFingerprint != canMineBlockFingerprint_)
            {
                canMineBlockTipTexture_ = resourcePackManager_->
                    CreateStringTexture(langsResources->canMineBlockTip, &preloadColors_->game.positiveAttributeColor,
                                        TOOLTIP_TEXT_WRAP_WIDTH);
                canMineBlockFingerprint_ = canMineBlockFingerprint;
            }
            if (canMineBlockTipTexture_ != nullptr)
            {
                textureToDraw.emplace_back(canMineBlockTipTexture_.get());
            }
        }
        if (abilityConfig->mineAbleLayer & BackGround)
        {
            uint64_t canMineWallFingerprint = StringUtils::StringToUint64(langsResources->canMineWallTip);
            if (canMineWallFingerprint != canMineWallFingerprint_)
            {
                canMineWallTipTexture_ = resourcePackManager_->
                    CreateStringTexture(langsResources->canMineWallTip, &preloadColors_->game.positiveAttributeColor,
                                        TOOLTIP_TEXT_WRAP_WIDTH);
                canMineWallFingerprint_ = canMineWallFingerprint;
            }
            if (canMineWallTipTexture_ != nullptr)
            {
                textureToDraw.emplace_back(canMineWallTipTexture_.get());
            }
        }
        if (abilityConfig->enablePrecisionMining)
        {
            uint64_t precisionMiningTipFingerprint = StringUtils::StringToUint64(langsResources->precisionMiningTip);
            if (precisionMiningTipFingerprint != precisionMiningTipFingerprint_)
            {
                precisionMiningTipTexture_ = resourcePackManager_->CreateStringTexture(
                    langsResources->precisionMiningTip, &preloadColors_->game.positiveAttributeColor,
                    TOOLTIP_TEXT_WRAP_WIDTH);
                precisionMiningTipFingerprint_ = precisionMiningTipFingerprint;
            }
            if (precisionMiningTipTexture_ != nullptr)
            {
                textureToDraw.emplace_back(precisionMiningTipTexture_.get());
            }
        }

        // 挖掘效率
        float miningEfficiency = abilityConfig->miningEfficiency;
        if (miningEfficiency != 0.0F)
        {
            std::string efficiencyTip = fmt::format(
                fmt::runtime(langsResources->efficiencyTip),
                fmt::format("{0:+.0f}", miningEfficiency * 100)
            );
            uint64_t efficiencyTipFingerprint = StringUtils::StringToUint64(efficiencyTip);
            bool efficiencyTipPositive = miningEfficiency > 0;
            if (efficiencyTipPositive != efficiencyTipPositive_ || efficiencyTipFingerprint !=
                efficiencyTipFingerprint_)
            {
                efficiencyTipTexture_ = resourcePackManager_->CreateStringTexture(
                    efficiencyTip, efficiencyTipPositive
                                       ? &preloadColors_->game.positiveAttributeColor
                                       : &preloadColors_->game.negativeAttributeColor,
                    TOOLTIP_TEXT_WRAP_WIDTH);
                efficiencyTipPositive_ = efficiencyTipPositive;
                efficiencyTipFingerprint_ = efficiencyTipFingerprint;
            }
            if (efficiencyTipTexture_ != nullptr)
            {
                textureToDraw.emplace_back(efficiencyTipTexture_.get());
            }
        }

        // 失误概率
        float fumbleProbability = abilityConfig->fumbleProbability;
        if (fumbleProbability != 0.0F)
        {
            std::string fumbleTip = fmt::format(
                fmt::runtime(langsResources->fumbleTip),
                (fumbleProbability > 0 ? "+" : "") + fmt::format("{0:.0f}", fumbleProbability * 100));
            uint64_t fumbleTipFingerprint = StringUtils::StringToUint64(fumbleTip);
            bool fumbleTipPositive = fumbleProbability > 0;
            if (fumbleTipPositive != fumbleTipPositive_ || fumbleTipFingerprint != fumbleTipFingerprint_)
            {
                fumbleTipTexture_ = resourcePackManager_->CreateStringTexture(
                    fumbleTip, fumbleTipPositive
                                   ? &preloadColors_->game.positiveAttributeColor
                                   : &preloadColors_->game.negativeAttributeColor,
                    TOOLTIP_TEXT_WRAP_WIDTH);
                fumbleTipPositive_ = fumbleTipPositive;
                fumbleTipFingerprint_ = fumbleTipFingerprint;
            }
            if (fumbleTipTexture_ != nullptr)
            {
                textureToDraw.emplace_back(fumbleTipTexture_.get());
            }
        }

        // 连锁挖掘半径
        int chainMiningRadius = abilityConfig->chainMiningRadius;
        if (chainMiningRadius != 0)
        {
            std::string chainMiningTip = fmt::format(
                fmt::runtime(langsResources->chainMiningTip),
                (chainMiningRadius > 0 ? "+" : "") + std::to_string(chainMiningRadius));
            uint64_t chainMiningTipFingerprint = StringUtils::StringToUint64(chainMiningTip);
            bool chainMiningTipPositive = chainMiningRadius > 0;
            if (chainMiningTipPositive != chainMiningTipPositive_ || chainMiningTipFingerprint !=
                chainMiningTipFingerprint_)
            {
                chainMiningTipTexture_ = resourcePackManager_->CreateStringTexture(
                    chainMiningTip, chainMiningTipPositive
                                        ? &preloadColors_->game.positiveAttributeColor
                                        : &preloadColors_->game.negativeAttributeColor,
                    TOOLTIP_TEXT_WRAP_WIDTH);
                chainMiningTipPositive_ = chainMiningTipPositive;
                chainMiningTipFingerprint_ = chainMiningTipFingerprint;
            }
            if (chainMiningTipTexture_ != nullptr)
            {
                textureToDraw.emplace_back(chainMiningTipTexture_.get());
            }
        }
    }

    constexpr float baseLineSpacing = 2.0F; // 基础行间距（未缩放）
    constexpr float basePadding = 8.0F; // 基础内边距（未缩放）
    const float scaledLineSpacing = baseLineSpacing * uiScale_;
    const float scaledPadding = basePadding * uiScale_;

    float maxWidth = 0.0F;
    float totalTextHeight = 0.0F;
    const size_t textureCount = textureToDraw.size();

    for (size_t i = 0; i < textureCount; ++i)
    {
        SDL_Texture* texture = textureToDraw[i];
        if (texture == nullptr)
        {
            continue;
        }

        maxWidth = std::max(maxWidth, static_cast<float>(texture->w));
        totalTextHeight += static_cast<float>(texture->h);
        if (i != textureCount - 1)
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

    for (auto* texture : textureToDraw)
    {
        if (texture == nullptr)
        {
            continue;
        }

        const SDL_FRect dstRect{
            currentX,
            currentY,
            static_cast<float>(texture->w) * uiScale_,
            static_cast<float>(texture->h) * uiScale_
        };
        SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
        currentY += static_cast<float>(texture->h) * uiScale_ + scaledLineSpacing;
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
    resourcePackManager_ = appContext_->GetResourcePackManager();
    Init();
}
