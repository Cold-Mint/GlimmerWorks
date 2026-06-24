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
#include "MaterialSelectCraftUISystem.h"

#include <algorithm>
#include "core/layout/GridLayoutStepper.h"
#include "core/layout/VerticalLayoutStepper.h"
#include "core/world/WorldContext.h"

glimmer::MaterialSelectCraftUISystem::MaterialSelectCraftUISystem(WorldContext* worldContext)
    : GUISystem(worldContext)
{
    appContext_ = worldContext_->GetAppContext();
    langsResources_ = appContext_->GetLangsResources();
    if (appContext_ == nullptr)
    {
        return;
    }
    stringManager_ = appContext_->GetStringManager();
    resourcePackManager_ = appContext_->GetResourcePackManager();
    preloadColors_ = appContext_->GetPreloadColors();
    Init();
}

void glimmer::MaterialSelectCraftUISystem::OnActivationChanged(bool activeStatus)
{
    if (activeStatus)
    {
        const CraftPreviewSlotComponent* slotComponent = entityShortCut_->GetSelectedCraftPreviewSlotComponent();
        if (slotComponent == nullptr)
        {
            worldContext_->PopGuiSystemType();
            return;
        }
        RecipeResource* recipeResource = slotComponent->GetRecipeResource();
        if (recipeResource == nullptr)
        {
            worldContext_->PopGuiSystemType();
            return;
        }
        recipeResource_ = recipeResource;
        const size_t inputResourceSize = recipeResource_->input.size();
        if (inputResourceSize == 0)
        {
            worldContext_->PopGuiSystemType();
        }
        std::vector<RequiredTag>& input = recipeResource_->input;
        tagRuntimeDataMap_.clear();
        selectedItemVector_.clear();
        if (stringManager_ == nullptr)
        {
            return;
        }
        for (auto& requiredTag : input)
        {
            std::stringstream stringStream;
            uint64_t cachedTagId = requiredTag.GetCachedTagId();
            std::optional<std::string> tagTranslateOptional = stringManager_->GetTagTranslate(cachedTagId);
            if (tagTranslateOptional.has_value())
            {
                stringStream << tagTranslateOptional.value();
            }
            else
            {
                stringStream << requiredTag.requiredTag;
            }
            stringStream << '*';
            stringStream << static_cast<int>(requiredTag.requiredWeight);
            std::string text = stringStream.str();
            std::unique_ptr<TagRuntimeData> tagRuntimeData = std::make_unique<TagRuntimeData>();
            tagRuntimeData->SetText(text);
            tagRuntimeData->SetRequiredWeight(requiredTag.requiredWeight);
            tagRuntimeData->SetPositiveTexture(
                resourcePackManager_->CreateStringTexture(text, &preloadColors_->game.positiveAttributeColor));
            tagRuntimeData->SetNegativeTexture(
                resourcePackManager_->CreateStringTexture(text, &preloadColors_->game.negativeAttributeColor));
            tagRuntimeDataMap_[cachedTagId] = std::move(tagRuntimeData);
        }
        //Filter out all the items that have the labels required for this recipe synthesis.
        //筛选所有的带有此配方合成需要的标签的物品。
        ItemContainerComponent* itemContainerComponent = entityShortCut_->GetItemContainerComponent();
        if (itemContainerComponent == nullptr)
        {
            worldContext_->PopGuiSystemType();
            return;
        }
        ItemContainer* itemContainer = itemContainerComponent->GetItemContainer();
        if (itemContainer == nullptr)
        {
            worldContext_->PopGuiSystemType();
            return;
        }
        uint8_t capacity = itemContainer->GetCapacity();
        uint8_t nextIndex = 0;

        // Count matching items first to determine actual grid width
        // 先统计匹配的物品数量以确定实际网格宽度
        matchingCount_ = 0;
        for (int i = 0; i < capacity; i++)
        {
            const Item* item = itemContainer->GetItem(i);
            if (item == nullptr)
            {
                continue;
            }
            for (auto& tagRuntimeDataPair : tagRuntimeDataMap_)
            {
                if (item->HasTag(tagRuntimeDataPair.first))
                {
                    matchingCount_++;
                    break;
                }
            }
        }

        constexpr DesignDimension panelPadding = 8.0F;
        constexpr DesignDimension cellPadding = 4.0F;
        maxTextureWidth_ = 0.0F;
        maxTextureHeight_ = 0.0F;
        for (const auto& tagRuntimeDataPair : tagRuntimeDataMap_)
        {
            const auto& tagRuntimeData = tagRuntimeDataPair.second;
            if (tagRuntimeData == nullptr)
            {
                continue;
            }
            if (tagRuntimeData->Matched())
            {
                SDL_Texture* positiveTexture = tagRuntimeData->GetPositiveTexture();
                if (positiveTexture == nullptr)
                {
                    continue;
                }
                maxTextureWidth_ = std::max(maxTextureWidth_, static_cast<DesignDimension>(positiveTexture->w));
                maxTextureHeight_ = std::max(maxTextureHeight_, static_cast<DesignDimension>(positiveTexture->h));
            }
            else
            {
                SDL_Texture* negativeTexture = tagRuntimeData->GetNegativeTexture();
                if (negativeTexture == nullptr)
                {
                    continue;
                }
                maxTextureWidth_ = std::max(maxTextureWidth_, static_cast<DesignDimension>(negativeTexture->w));
                maxTextureHeight_ = std::max(maxTextureHeight_, static_cast<DesignDimension>(negativeTexture->h));
            }
        }

        constexpr uint8_t gridColumns = 6;
        constexpr DesignDimension gridPadding = 4.0F;
        const uint8_t actualColumns = matchingCount_ > 0 ? std::min(matchingCount_, gridColumns) : 1;
        const DesignDimension gridWidth = static_cast<float>(actualColumns) * (ITEM_SLOT_SIZE + gridPadding) -
            gridPadding;
        const DesignDimension panelWidth = maxTextureWidth_ + gridWidth + 2.0F * panelPadding;

        // Calculate required heights for tag area and item slot grid, then take the larger one.
        // 计算标签区域和物品槽网格所需的高度，取较大值。
        const DesignDimension tagAreaHeight = static_cast<DesignDimension>(tagRuntimeDataMap_.size()) * (
                maxTextureHeight_ +
                cellPadding) +
            2.0F * panelPadding - cellPadding;
        const uint32_t gridRows = matchingCount_ > 0
                                      ? (matchingCount_ + gridColumns - 1) / gridColumns
                                      : 1;
        const DesignDimension gridAreaHeight = static_cast<DesignDimension>(gridRows) * (ITEM_SLOT_SIZE + gridPadding) -
            gridPadding + 2.0F * panelPadding;
        const DesignDimension panelHeight = std::max(tagAreaHeight, gridAreaHeight);
        panelWidth_ = panelWidth;
        panelHeight_ = panelHeight;

        const DesignDimension panelOffsetX = (static_cast<DesignDimension>(windowWidth_) / uiScale_ - panelWidth) *
            0.5F;
        const DesignDimension panelOffsetY = (static_cast<DesignDimension>(windowHeight_) / uiScale_ - panelHeight) *
            0.5F;

        const DesignVector2D gridStartPosition{
            panelOffsetX + maxTextureWidth_ + panelPadding, panelOffsetY + panelPadding
        };

        for (int i = 0; i < capacity; i++)
        {
            const Item* item = itemContainer->GetItem(i);
            if (item == nullptr)
            {
                continue;
            }
            bool hasTag = false;
            for (const auto& tagRuntimeDataPair : tagRuntimeDataMap_)
            {
                if (item->HasTag(tagRuntimeDataPair.first))
                {
                    hasTag = true;
                    break;
                }
            }
            if (!hasTag)
            {
                continue;
            }
            ItemSlotQuantityComponent* itemSlotQuantityComponent = nullptr;
            if (nextIndex < itemSlotQuantityVector_.size())
            {
                itemSlotQuantityComponent = itemSlotQuantityVector_[nextIndex];
            }
            else
            {
                itemSlotQuantityComponent = entityManager_->AddComponent<
                    ItemSlotQuantityComponent>(entityManager_->AddEntity());
                itemSlotQuantityComponent->SetOnSelectQuantityChanged(
                    [this](uint8_t slotIndex, Item* item, uint8_t selectQuantity)
                    {
                        SelectedItemRuntimeData* selectedItemRuntimeData = nullptr;
                        for (auto& selectedItem : selectedItemVector_)
                        {
                            if (selectedItem == nullptr)
                            {
                                continue;
                            }
                            SelectedItemRuntimeData* temporarySelectedItemRuntimeData = selectedItem.get();
                            if (temporarySelectedItemRuntimeData == nullptr)
                            {
                                continue;
                            }
                            uint8_t selectedSlotIndex = temporarySelectedItemRuntimeData->GetSlotIndex();
                            if (selectedSlotIndex == slotIndex)
                            {
                                selectedItemRuntimeData = temporarySelectedItemRuntimeData;
                                break;
                            }
                        }
                        if (selectedItemRuntimeData == nullptr)
                        {
                            auto selectedItemRuntimeDataUnique = std::make_unique<
                                SelectedItemRuntimeData>();
                            selectedItemRuntimeDataUnique->SetSlotIndex(slotIndex);
                            selectedItemRuntimeDataUnique->SetItem(item);
                            selectedItemVector_.emplace_back(std::move(selectedItemRuntimeDataUnique));
                            selectedItemRuntimeData = selectedItemVector_.back().get();
                        }
                        //Local variable selectedItemRuntimeData may point to memory which is out of scope
                        selectedItemRuntimeData->SetSelectedAmount(selectQuantity);
                        //When the selected item changes, we recalculate the tag.
                        //当选择的Item改变时，我们重新计算标签。
                        for (auto& tagPair : tagRuntimeDataMap_)
                        {
                            tagPair.second->SetActualValue(0);
                        }
                        for (auto& selectedItem : selectedItemVector_)
                        {
                            const uint8_t selectAmount = selectedItem->GetSelectedAmount();
                            if (selectAmount == 0)
                            {
                                continue;
                            }
                            for (const auto& tagRuntimeDataPair : tagRuntimeDataMap_)
                            {
                                Item* selectedItemPtr = selectedItem->GetItem();
                                if (selectedItemPtr == nullptr)
                                {
                                    continue;
                                }
                                uint8_t value = selectedItemPtr->GetTagValue(tagRuntimeDataPair.first);
                                if (value == 0)
                                {
                                    //This tag is not included.
                                    //不包含此标签。
                                    continue;
                                }
                                tagRuntimeDataPair.second->AddActualValue(value * selectAmount);
                            }
                        }
                    });
                itemSlotQuantityVector_.emplace_back(itemSlotQuantityComponent);
            }
            if (itemSlotQuantityComponent == nullptr)
            {
                worldContext_->PopGuiSystemType();
                return;
            }
            itemSlotQuantityComponent->Show();
            itemSlotQuantityComponent->SetSelectQuantity(0);
            itemSlotQuantityComponent->SetSize({ITEM_SLOT_SIZE, ITEM_SLOT_SIZE});
            itemSlotQuantityComponent->SetItemContainer(itemContainer);
            itemSlotQuantityComponent->SetSlotIndex(i);
            nextIndex++;
        }
        for (int i = nextIndex; i < itemSlotQuantityVector_.size(); i++)
        {
            itemSlotQuantityVector_[i]->Hide();
        }

        UpdateItemSlotPositions();

        if (buttonComponent_ == nullptr)
        {
            buttonComponent_ = entityManager_->AddComponent<ButtonComponent>(entityManager_->AddEntity());
            buttonComponent_->SetText(appContext_, langsResources_->craft);
            buttonComponent_->SetClickCallback([this]
            {
                //Carry out synthesis
                //进行合成
                //Deduct the items
                //扣除物品
                ItemContainerComponent* itemContainerComponent = entityShortCut_->GetItemContainerComponent();
                if (itemContainerComponent == nullptr)
                {
                    worldContext_->PopGuiSystemType();
                    return;
                }
                ItemContainer* itemContainer = itemContainerComponent->GetItemContainer();
                if (itemContainer == nullptr)
                {
                    worldContext_->PopGuiSystemType();
                    return;
                }
                for (auto& selectedItem : selectedItemVector_)
                {
                    uint8_t expectedQuantity = selectedItem->GetSelectedAmount();
                    uint8_t actualQuantity = itemContainer->
                        RemoveItemAt(selectedItem->GetSlotIndex(), expectedQuantity);
                    if (expectedQuantity != actualQuantity)
                    {
                        LogCat::w("Failed to remove the item. At ", selectedItem->GetSlotIndex());
                    }
                }
                //Give the player items
                //给予玩家物品
                ResourceLocator* resourceLocator = appContext_->GetResourceLocator();
                if (resourceLocator == nullptr)
                {
                    return;
                }
                std::unique_ptr<Item> outputItem = resourceLocator->FindItem(worldContext_, recipeResource_->output);
                if (outputItem != nullptr)
                {
                    std::unique_ptr<Item> returnItem = itemContainer->AddItem(std::move(outputItem));
                    if (returnItem != nullptr)
                    {
                        LogCat::w("Failed to add the item. ");
                    }
                }
                worldContext_->PopGuiSystemType();
            });
        }
        buttonComponent_->Show();
    }
    else
    {
        for (int i = 0; i < itemSlotQuantityVector_.size(); i++)
        {
            itemSlotQuantityVector_[i]->Hide();
        }
        if (buttonComponent_ != nullptr)
        {
            buttonComponent_->Hide();
        }
    }
}

void glimmer::MaterialSelectCraftUISystem::OnConfigChanged(const Config* config)
{
    uiScale_ = config->window.uiScale;
    panelInnerPadding_ = basePanelInnerPadding_ * uiScale_;
}

void glimmer::MaterialSelectCraftUISystem::OnWindowSizeChanged(int width, int height)
{
    windowHeight_ = height;
    windowWidth_ = width;
    UpdateItemSlotPositions();
}

void glimmer::MaterialSelectCraftUISystem::UpdateItemSlotPositions() const
{
    if (itemSlotQuantityVector_.empty())
    {
        return;
    }

    constexpr DesignDimension panelPadding = 8.0F;
    constexpr uint8_t gridColumns = 6;
    constexpr DesignDimension gridPadding = 4.0F;

    const DesignDimension panelOffsetX = (static_cast<DesignDimension>(windowWidth_) / uiScale_ - panelWidth_) *
        0.5F;
    const DesignDimension panelOffsetY = (static_cast<DesignDimension>(windowHeight_) / uiScale_ - panelHeight_) *
        0.5F;

    const DesignVector2D gridStartPosition{
        panelOffsetX + maxTextureWidth_ + panelPadding, panelOffsetY + panelPadding
    };

    uint8_t itemIndex = 0;
    GridLayoutStepper gridStepper(ITEM_SLOT_SIZE, gridStartPosition, gridColumns, gridPadding,
                                  itemSlotQuantityVector_.size());
    for (auto& itemSlotQuantityComponent : itemSlotQuantityVector_)
    {
        if (itemSlotQuantityComponent == nullptr || !itemSlotQuantityComponent->IsVisible())
        {
            continue;
        }
        const DesignVector2D position = gridStepper.Next();
        itemSlotQuantityComponent->SetPosition(position);
        itemIndex++;
    }
}

void glimmer::MaterialSelectCraftUISystem::Render(SDL_Renderer* renderer)
{
    if (panelBackGroundTexture_ == nullptr)
    {
        ResourceRef panelBGResourceRef;
        panelBGResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
        panelBGResourceRef.SetResourceType(RESOURCE_TEXTURE);
        panelBGResourceRef.SetResourceKey("gui/panel_bg");
        panelBackGroundTexture_ = worldContext_->GetAppContext()->GetResourceLocator()->
                                                 FindTexture(&panelBGResourceRef);
    }
    if (subPanelBackGroundTexture_ == nullptr)
    {
        ResourceRef panelBGResourceRef;
        panelBGResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
        panelBGResourceRef.SetResourceType(RESOURCE_TEXTURE);
        panelBGResourceRef.SetResourceKey("gui/sub_panel_bg");
        subPanelBackGroundTexture_ = worldContext_->GetAppContext()->GetResourceLocator()->
                                                    FindTexture(&panelBGResourceRef);
    }
    if (tagRuntimeDataMap_.empty() || preloadColors_ == nullptr)
    {
        return;
    }

    // Find the maximum texture size so that the panel can accommodate all tag textures.
    // 获取纹理的最大尺寸，使面板能够容纳所有标签纹理。
    DesignDimension maxTextureWidth = 0.0F;
    DesignDimension maxTextureHeight = 0.0F;
    for (const auto& tagRuntimeDataPair : tagRuntimeDataMap_)
    {
        const auto& tagRuntimeData = tagRuntimeDataPair.second;
        if (tagRuntimeData == nullptr)
        {
            continue;
        }
        const auto tagRuntimeDataPtr = tagRuntimeData.get();
        if (tagRuntimeDataPtr == nullptr)
        {
            continue;
        }

        if (tagRuntimeDataPtr->Matched())
        {
            auto positiveTexture = tagRuntimeDataPtr->GetPositiveTexture();
            if (positiveTexture == nullptr)
            {
                continue;
            }
            maxTextureWidth = std::max(maxTextureWidth, static_cast<DesignDimension>(positiveTexture->w));
            maxTextureHeight = std::max(maxTextureHeight, static_cast<DesignDimension>(positiveTexture->h));
        }
        else
        {
            auto negativeTexture = tagRuntimeDataPtr->GetNegativeTexture();
            if (negativeTexture == nullptr)
            {
                continue;
            }
            maxTextureWidth = std::max(maxTextureWidth, static_cast<DesignDimension>(negativeTexture->w));
            maxTextureHeight = std::max(maxTextureHeight, static_cast<DesignDimension>(negativeTexture->h));
        }
    }
    if (maxTextureWidth <= 0.0F || maxTextureHeight <= 0.0F)
    {
        return;
    }

    // Panel layout parameters (design coordinates).
    // 面板布局参数（设计坐标）。
    constexpr DesignDimension panelPadding = 8.0F;
    constexpr DesignDimension cellPadding = 4.0F;
    const auto dataLength = static_cast<uint32_t>(tagRuntimeDataMap_.size());

    // Panel size in design coordinates.
    // 面板尺寸（设计坐标）。
    const DesignDimension panelWidth = panelWidth_;
    const DesignDimension panelHeight = panelHeight_;

    // Convert to screen coordinates and center the panel.
    // 转换到屏幕坐标并使面板居中。
    const float scaledPanelWidth = panelWidth * uiScale_;
    const float scaledPanelHeight = panelHeight * uiScale_;
    const float panelX = (static_cast<float>(windowWidth_) - scaledPanelWidth) * 0.5F;
    const float panelY = (static_cast<float>(windowHeight_) - scaledPanelHeight) * 0.5F;
    const SDL_FRect panelRect{panelX, panelY, scaledPanelWidth, scaledPanelHeight};
    SDL_RenderTexture(renderer, panelBackGroundTexture_.get(), nullptr, &panelRect);

    // Draw sub-panel background for the tag area (left side).
    // 绘制左侧标签区域的子面板背景。
    if (subPanelBackGroundTexture_ != nullptr && dataLength > 0)
    {
        const DesignDimension tagAreaHeight = static_cast<DesignDimension>(dataLength) * (maxTextureHeight +
                cellPadding) +
            2.0F * panelPadding - cellPadding;
        const float scaledTagSubPanelWidth = maxTextureWidth * uiScale_;
        const float scaledTagSubPanelHeight = std::max(tagAreaHeight,
                                                       panelHeight_ - 2.0F * panelInnerPadding_ / uiScale_) * uiScale_;
        const SDL_FRect tagSubPanelRect{
            panelX + panelInnerPadding_,
            panelY + panelInnerPadding_,
            scaledTagSubPanelWidth,
            scaledTagSubPanelHeight
        };
        SDL_RenderTexture(renderer, subPanelBackGroundTexture_.get(), nullptr, &tagSubPanelRect);

        // Draw sub-panel background for the item slot grid (right side).
        // 绘制右侧物品槽网格的子面板背景。
        const DesignDimension gridSubPanelWidth = panelWidth_ - maxTextureWidth - 2.0F * panelPadding;
        const float scaledGridSubPanelWidth = gridSubPanelWidth * uiScale_;
        const float gridSubPanelX = panelX + (maxTextureWidth + panelPadding) * uiScale_;
        const SDL_FRect gridSubPanelRect{
            gridSubPanelX,
            panelY + panelInnerPadding_,
            scaledGridSubPanelWidth,
            scaledTagSubPanelHeight
        };
        SDL_RenderTexture(renderer, subPanelBackGroundTexture_.get(), nullptr, &gridSubPanelRect);
    }

    // Place the tag textures vertically on the left side of the panel using VerticalLayoutStepper.
    // 使用VerticalLayoutStepper将标签纹理垂直排在面板的左侧。
    DesignVector2D startPosition{panelPadding, panelPadding};
    VerticalLayoutStepper stepper(maxTextureHeight, startPosition, cellPadding, dataLength);
    for (const auto& tagRuntimeDataPair : tagRuntimeDataMap_)
    {
        const auto& tagRuntimeData = tagRuntimeDataPair.second;
        if (tagRuntimeData == nullptr)
        {
            continue;
        }
        const auto tagRuntimeDataPtr = tagRuntimeData.get();
        if (tagRuntimeDataPtr == nullptr)
        {
            continue;
        }
        const DesignVector2D position = stepper.Next();
        SDL_Texture* texture = nullptr;
        if (tagRuntimeDataPtr->Matched())
        {
            texture = tagRuntimeDataPtr->GetPositiveTexture();
        }
        else
        {
            texture = tagRuntimeDataPtr->GetNegativeTexture();
        }
        if (texture == nullptr)
        {
            continue;
        }
        const SDL_FRect dstRect{
            panelX + position.x * uiScale_,
            panelY + position.y * uiScale_,
            static_cast<float>(texture->w) * uiScale_,
            static_cast<float>(texture->h) * uiScale_
        };
        SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
    }

    AppContext::RestoreColorRenderer(renderer);
}

uint8_t glimmer::MaterialSelectCraftUISystem::GetRenderOrder()
{
    return RENDER_ORDER_MATERIAL_SELECT_CRAFT_UI;
}

glimmer::GameSystemType glimmer::MaterialSelectCraftUISystem::GetGameSystemType() const
{
    return GameSystemType::MaterialSelectCraftUISystem;
}
