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
    appContext_ = worldContext_->GetAppContext();
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
    if (gameComponentType == COMPONENT_ITEM_TOOL_TIP && itemToolTipComponent_ == nullptr)
    {
        itemToolTipComponent_ = entityShortCut_->GetItemToolTipComponent();
    }
}

void glimmer::ItemToolTipSystem::OnConfigChanged(const Config* config)
{
    uiScale_ = config->window.uiScale;
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
    const LangsResources* langsResources = appContext_->GetLangsResources();
    std::vector<SDL_Texture*> textureToDraw;
    // 渲染物品名称
    const std::string& name = item->GetName();
    if (const uint64_t itemNameFingerprint = StringUtils::StringToUint64(name); itemNameFingerprint != itemNameFingerprint_)
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
        if (const uint64_t itemDescriptionFingerprint = StringUtils::StringToUint64(description); itemDescriptionFingerprint != itemDescriptionFingerprint_)
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
        if ((abilityConfig->mineAbleLayer & Ground) != static_cast<uint8_t>(0))
        {
            if (const uint64_t canMineBlockFingerprint = StringUtils::StringToUint64(langsResources->canMineBlockTip); canMineBlockFingerprint != canMineBlockFingerprint_)
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
        if ((abilityConfig->mineAbleLayer & BackGround) != static_cast<uint8_t>(0))
        {
            if (const uint64_t canMineWallFingerprint = StringUtils::StringToUint64(langsResources->canMineWallTip); canMineWallFingerprint != canMineWallFingerprint_)
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
            if (const uint64_t precisionMiningTipFingerprint = StringUtils::StringToUint64(langsResources->precisionMiningTip); precisionMiningTipFingerprint != precisionMiningTipFingerprint_)
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
            const uint64_t efficiencyTipFingerprint = StringUtils::StringToUint64(efficiencyTip);
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

        // 连锁挖掘半径
        int chainMiningRadius = abilityConfig->chainMiningRadius;
        if (chainMiningRadius != 0)
        {
            std::string chainMiningTip = fmt::format(
                fmt::runtime(langsResources->chainMiningTip),
                (chainMiningRadius > 0 ? "+" : "") + std::to_string(chainMiningRadius));
            const uint64_t chainMiningTipFingerprint = StringUtils::StringToUint64(chainMiningTip);
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

    if (item->IsLocked())
    {
        if (const uint64_t lockedFingerprint = StringUtils::StringToUint64(langsResources->lockedTip); lockedFingerprint != itemLockedFingerprint_)
        {
            itemLockedTexture_ = resourcePackManager_->
                CreateStringTexture(langsResources->lockedTip, &preloadColors_->game.negativeAttributeColor);
            itemLockedFingerprint_ = lockedFingerprint;
        }
        if (itemLockedTexture_ != nullptr)
        {
            textureToDraw.emplace_back(itemLockedTexture_.get());
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

    const ScreenVector2D& positionScreenVector2D = itemToolTipComponent_->GetPosition();
    const SDL_FRect backgroundRect{
        positionScreenVector2D.x,
        positionScreenVector2D.y,
        (maxWidth + 2 * basePadding) * uiScale_, // 左右padding
        (totalTextHeight + 2 * basePadding) * uiScale_ // 上下padding
    };

    if (tooltipBgTextureResult_ != nullptr)
    {
        SDL_Texture* texture = tooltipBgTextureResult_->GetResource();
        if (texture != nullptr)
        {
            const ResourcePack* resourcePack = tooltipBgTextureResult_->GetResourcePack();
            if (resourcePack != nullptr)
            {
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
                }
                else
                {
                    SDL_RenderTexture9Grid(renderer, texture, nullptr, packConfig.itemToolTipNineSlice.leftBorderPx,
                                           packConfig.itemToolTipNineSlice.rightBorderPx,
                                           packConfig.itemToolTipNineSlice.topBorderPx,
                                           packConfig.itemToolTipNineSlice.bottomBorderPx,
                                           packConfig.itemToolTipNineSlice.scale, &backgroundRect);
                }
            }
        }
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


uint8_t glimmer::ItemToolTipSystem::GetRenderOrder()
{
    return RENDER_ORDER_ITEM_TOOLTIP;
}

glimmer::GameSystemType glimmer::ItemToolTipSystem::GetGameSystemType() const
{
    return GameSystemType::ItemToolTipSystem;
}
