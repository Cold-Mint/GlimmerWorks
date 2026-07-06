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
#include "ItemToolTipSystem.h"

#include "core/math/CoordinateTransformer.h"
#include "core/utils/StringUtils.h"
#include "core/world/WorldContext.h"
#include "fmt/xchar.h"

glimmer::ItemToolTipSystem::ItemToolTipSystem(WorldContext* worldContext) : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_ITEM_TOOL_TIP);
    ResourceRef tooltipBgResourceRef;
    tooltipBgResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    tooltipBgResourceRef.SetResourceType(RESOURCE_TEXTURE);
    tooltipBgResourceRef.SetResourceKey("gui/tooltip_bg");
    const WorldContext* worldCtx = GetWorldContext();
    appContext_ = worldCtx->GetAppContext();
    if (appContext_ == nullptr)
    {
        return;
    }
    const ResourceLocator* resourceLocator = appContext_->GetResourceLocator();
    if (resourceLocator == nullptr)
    {
        return;
    }
    tooltipBgTextureResult_ = resourceLocator->FindTexture(&tooltipBgResourceRef);
    resourcePackManager_ = appContext_->GetResourcePackManager();
    preloadColors_ = appContext_->GetPreloadColors();
    Init();
}

void glimmer::ItemToolTipSystem::OnFrameStart()
{
    if (itemToolTipComponent_ == nullptr)
    {
        return;
    }
    itemToolTipComponent_->ResetItem();
}

void glimmer::ItemToolTipSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    const EntityShortCut* entityShortCut = GetEntityShortCut();
    if (gameComponentType == COMPONENT_ITEM_TOOL_TIP && itemToolTipComponent_ == nullptr)
    {
        itemToolTipComponent_ = entityShortCut->GetItemToolTipComponent();
    }
}

void glimmer::ItemToolTipSystem::OnConfigChanged(const Config* config)
{
    uiScale_ = config->window.uiScale;
}

void glimmer::ItemToolTipSystem::UpdateItemNameTexture(const Item* item)
{
    const std::string& name = item->GetName();
    const uint64_t itemNameFingerprint = StringUtils::StringToUint64(name);
    if (itemNameFingerprint == itemNameCache_.fingerprint)
    {
        return;
    }
    itemNameCache_.texture = resourcePackManager_->CreateStringTexture(name, &preloadColors_->textColor);
    itemNameCache_.fingerprint = itemNameFingerprint;
}

void glimmer::ItemToolTipSystem::UpdateItemDescriptionTexture(const Item* item)
{
    const std::optional<std::string>& descriptionOptional = item->GetDescription();
    if (!descriptionOptional.has_value())
    {
        return;
    }
    const std::string& description = descriptionOptional.value();
    const uint64_t itemDescriptionFingerprint = StringUtils::StringToUint64(description);
    if (itemDescriptionFingerprint == itemDescriptionCache_.fingerprint)
    {
        return;
    }
    itemDescriptionCache_.texture = resourcePackManager_->
        CreateStringTexture(description, &preloadColors_->textColor);
    itemDescriptionCache_.fingerprint = itemDescriptionFingerprint;
}

void glimmer::ItemToolTipSystem::UpdateAbilityTextures(const AbilityConfig* abilityConfig,
                                                       const LangsResources* langsResources,
                                                       std::vector<SDL_Texture*>& textureToDraw)
{
    if (abilityConfig == nullptr)
    {
        return;
    }

    if ((abilityConfig->mineAbleLayer & Ground) != 0)
    {
        const uint64_t canMineBlockFingerprint = StringUtils::StringToUint64(langsResources->canMineBlockTip);
        if (canMineBlockFingerprint != canMineBlockTipCache_.fingerprint)
        {
            canMineBlockTipCache_.texture = resourcePackManager_->
                CreateStringTexture(langsResources->canMineBlockTip, &preloadColors_->game.positiveAttributeColor,
                                    TOOLTIP_TEXT_WRAP_WIDTH);
            canMineBlockTipCache_.fingerprint = canMineBlockFingerprint;
        }
        if (canMineBlockTipCache_.texture != nullptr)
        {
            textureToDraw.emplace_back(canMineBlockTipCache_.texture.get());
        }
    }

    if ((abilityConfig->mineAbleLayer & BackGround) != 0)
    {
        const uint64_t canMineWallFingerprint = StringUtils::StringToUint64(langsResources->canMineWallTip);
        if (canMineWallFingerprint != canMineWallTipCache_.fingerprint)
        {
            canMineWallTipCache_.texture = resourcePackManager_->
                CreateStringTexture(langsResources->canMineWallTip, &preloadColors_->game.positiveAttributeColor,
                                    TOOLTIP_TEXT_WRAP_WIDTH);
            canMineWallTipCache_.fingerprint = canMineWallFingerprint;
        }
        if (canMineWallTipCache_.texture != nullptr)
        {
            textureToDraw.emplace_back(canMineWallTipCache_.texture.get());
        }
    }

    if (abilityConfig->enablePrecisionMining)
    {
        const uint64_t precisionMiningTipFingerprint = StringUtils::StringToUint64(langsResources->precisionMiningTip);
        if (precisionMiningTipFingerprint != precisionMiningTipCache_.fingerprint)
        {
            precisionMiningTipCache_.texture = resourcePackManager_->CreateStringTexture(
                langsResources->precisionMiningTip, &preloadColors_->game.positiveAttributeColor,
                TOOLTIP_TEXT_WRAP_WIDTH);
            precisionMiningTipCache_.fingerprint = precisionMiningTipFingerprint;
        }
        if (precisionMiningTipCache_.texture != nullptr)
        {
            textureToDraw.emplace_back(precisionMiningTipCache_.texture.get());
        }
    }

    float miningEfficiency = abilityConfig->miningEfficiency;
    if (miningEfficiency != 0.0F)
    {
        std::string efficiencyTip = fmt::format(
            fmt::runtime(langsResources->efficiencyTip),
            fmt::format("{0:+.0f}", miningEfficiency * 100)
        );
        const uint64_t efficiencyTipFingerprint = StringUtils::StringToUint64(efficiencyTip);
        bool efficiencyTipPositive = miningEfficiency > 0;
        if (efficiencyTipPositive != efficiencyTipCache_.positive || efficiencyTipFingerprint != efficiencyTipCache_.cache.fingerprint)
        {
            efficiencyTipCache_.cache.texture = resourcePackManager_->CreateStringTexture(
                efficiencyTip, efficiencyTipPositive
                                   ? &preloadColors_->game.positiveAttributeColor
                                   : &preloadColors_->game.negativeAttributeColor,
                TOOLTIP_TEXT_WRAP_WIDTH);
            efficiencyTipCache_.positive = efficiencyTipPositive;
            efficiencyTipCache_.cache.fingerprint = efficiencyTipFingerprint;
        }
        if (efficiencyTipCache_.cache.texture != nullptr)
        {
            textureToDraw.emplace_back(efficiencyTipCache_.cache.texture.get());
        }
    }

    int chainMiningRadius = abilityConfig->chainMiningRadius;
    if (chainMiningRadius != 0)
    {
        std::string chainMiningTip = fmt::format(
            fmt::runtime(langsResources->chainMiningTip),
            fmt::format("{}{}", chainMiningRadius > 0 ? "+" : "", chainMiningRadius));
        const uint64_t chainMiningTipFingerprint = StringUtils::StringToUint64(chainMiningTip);
        bool chainMiningTipPositive = chainMiningRadius > 0;
        if (chainMiningTipPositive != chainMiningTipCache_.positive || chainMiningTipFingerprint != chainMiningTipCache_.cache.fingerprint)
        {
            chainMiningTipCache_.cache.texture = resourcePackManager_->CreateStringTexture(
                chainMiningTip, chainMiningTipPositive
                                    ? &preloadColors_->game.positiveAttributeColor
                                    : &preloadColors_->game.negativeAttributeColor,
                TOOLTIP_TEXT_WRAP_WIDTH);
            chainMiningTipCache_.positive = chainMiningTipPositive;
            chainMiningTipCache_.cache.fingerprint = chainMiningTipFingerprint;
        }
        if (chainMiningTipCache_.cache.texture != nullptr)
        {
            textureToDraw.emplace_back(chainMiningTipCache_.cache.texture.get());
        }
    }
}

void glimmer::ItemToolTipSystem::UpdateLockedTexture(const LangsResources* langsResources,
                                                     std::vector<SDL_Texture*>& textureToDraw)
{
    const Item* item = itemToolTipComponent_->GetItem();
    if (item == nullptr || !item->IsLocked())
    {
        return;
    }
    const uint64_t lockedFingerprint = StringUtils::StringToUint64(langsResources->lockedTip);
    if (lockedFingerprint != itemLockedCache_.fingerprint)
    {
        itemLockedCache_.texture = resourcePackManager_->
            CreateStringTexture(langsResources->lockedTip, &preloadColors_->game.negativeAttributeColor);
        itemLockedCache_.fingerprint = lockedFingerprint;
    }
    if (itemLockedCache_.texture != nullptr)
    {
        textureToDraw.emplace_back(itemLockedCache_.texture.get());
    }
}

void glimmer::ItemToolTipSystem::CollectTooltipTextures(const Item* item, std::vector<SDL_Texture*>& textureToDraw)
{
    const LangsResources* langsResources = appContext_->GetLangsResources();

    UpdateItemNameTexture(item);
    if (itemNameCache_.texture != nullptr)
    {
        textureToDraw.emplace_back(itemNameCache_.texture.get());
    }

    UpdateItemDescriptionTexture(item);
    if (itemDescriptionCache_.texture != nullptr)
    {
        textureToDraw.emplace_back(itemDescriptionCache_.texture.get());
    }

    UpdateAbilityTextures(item->GetAbilityConfig(), langsResources, textureToDraw);
    UpdateLockedTexture(langsResources, textureToDraw);
}

void glimmer::ItemToolTipSystem::RenderTooltipBackground(SDL_Renderer* renderer, const SDL_FRect& backgroundRect)
{
    if (tooltipBgTextureResult_ == nullptr)
    {
        return;
    }
    SDL_Texture* texture = tooltipBgTextureResult_->GetResource();
    if (texture == nullptr)
    {
        return;
    }
    const ResourcePack* resourcePack = tooltipBgTextureResult_->GetResourcePack();
    if (resourcePack == nullptr)
    {
        return;
    }
    const ResourcePackConfig& packConfig = resourcePack->GetResourcePackConfig();
    if (packConfig.itemToolTipNineSlice.enableTiled)
    {
        SDL_RenderTexture9GridTiled(renderer, texture, nullptr,
                                    packConfig.itemToolTipNineSlice.leftBorderPx,
                                    packConfig.itemToolTipNineSlice.rightBorderPx,
                                    packConfig.itemToolTipNineSlice.topBorderPx,
                                    packConfig.itemToolTipNineSlice.bottomBorderPx,
                                    packConfig.itemToolTipNineSlice.scale, &backgroundRect,
                                    packConfig.itemToolTipNineSlice.tileScale);
        return;
    }
    SDL_RenderTexture9Grid(renderer, texture, nullptr, packConfig.itemToolTipNineSlice.leftBorderPx,
                           packConfig.itemToolTipNineSlice.rightBorderPx,
                           packConfig.itemToolTipNineSlice.topBorderPx,
                           packConfig.itemToolTipNineSlice.bottomBorderPx,
                           packConfig.itemToolTipNineSlice.scale, &backgroundRect);
}

void glimmer::ItemToolTipSystem::RenderTooltipTextures(SDL_Renderer* renderer,
                                                       const std::vector<SDL_Texture*>& textureToDraw,
                                                       float x, float y, float lineSpacing)
{
    float currentY = y;
    for (auto texture : textureToDraw)
    {
        if (texture == nullptr)
        {
            continue;
        }
        const SDL_FRect dstRect{
            x,
            currentY,
            static_cast<float>(texture->w) * uiScale_,
            static_cast<float>(texture->h) * uiScale_
        };
        SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
        currentY += static_cast<float>(texture->h) * uiScale_ + lineSpacing;
    }
}

void glimmer::ItemToolTipSystem::Render(SDL_Renderer* renderer)
{
    if (itemToolTipComponent_ == nullptr)
    {
        return;
    }
    const Item* item = itemToolTipComponent_->GetItem();
    if (item == nullptr)
    {
        return;
    }

    std::vector<SDL_Texture*> textureToDraw;
    CollectTooltipTextures(item, textureToDraw);

    constexpr float baseLineSpacing = 2.0F;
    constexpr float basePadding = 8.0F;
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

    const ScreenVector2D& positionScreenVector2D = itemToolTipComponent_->GetPosition();
    const SDL_FRect backgroundRect{
        positionScreenVector2D.x,
        positionScreenVector2D.y,
        (maxWidth + 2 * basePadding) * uiScale_,
        (totalTextHeight + 2 * basePadding) * uiScale_
    };

    RenderTooltipBackground(renderer, backgroundRect);
    RenderTooltipTextures(renderer, textureToDraw, backgroundRect.x + scaledPadding,
                          backgroundRect.y + scaledPadding, scaledLineSpacing);
}


uint8_t glimmer::ItemToolTipSystem::GetRenderOrder()
{
    return RENDER_ORDER_ITEM_TOOLTIP;
}

glimmer::GameSystemType glimmer::ItemToolTipSystem::GetGameSystemType() const
{
    return GameSystemType::ItemToolTipSystem;
}
