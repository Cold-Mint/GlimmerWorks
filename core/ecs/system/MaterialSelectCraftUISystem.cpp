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
#include "core/Constants.h"
#include "core/layout/GridLayoutStepper.h"
#include "core/layout/VerticalLayoutStepper.h"
#include "core/world/WorldContext.h"
#include "core/world/SystemScheduler.h"

glimmer::MaterialSelectCraftUISystem::MaterialSelectCraftUISystem(WorldContext* worldContext)
    : GUISystem(worldContext)
{
    WorldContext* worldContextPtr = GetWorldContext();
    resources_.appContext_ = worldContextPtr->GetAppContext();
    if (resources_.appContext_ == nullptr)
    {
        return;
    }
    resources_.langsResources_ = resources_.appContext_->GetLangsResources();
    resources_.resourceLocator_ = resources_.appContext_->GetResourceLocator();
    resources_.stringManager_ = resources_.appContext_->GetModContext()->GetStringManager();
    resources_.resourcePackManager_ = resources_.appContext_->GetResourcePackManager();
    resources_.preloadColors_ = resources_.appContext_->GetGraphicsContext()->GetPreloadColors();
    Init();
}

void glimmer::MaterialSelectCraftUISystem::OnActivationChanged(bool activeStatus)
{
    if (activeStatus)
    {
        if (!InitializeActivation())
        {
            return;
        }
        InitializeTagRuntimeData();
        CountMatchingItems();
        CalculateMaxTextureSizeForActivation();
        CalculatePanelDimensions();
        SetupItemSlots();
        SetupButton();
    }
    else
    {
        DeactivateUI();
    }
}

bool glimmer::MaterialSelectCraftUISystem::InitializeActivation()
{
    const WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        return false;
    }
    const EntityShortCut* entityShortCut = GetEntityShortCut();
    if (entityShortCut == nullptr)
    {
        return false;
    }
    SystemScheduler* systemScheduler = worldContext->GetSystemScheduler();
    if (systemScheduler == nullptr)
    {
        return false;
    }
    const CraftPreviewSlotComponent* slotComponent = entityShortCut->GetSelectedCraftPreviewSlotComponent();
    if (slotComponent == nullptr)
    {
        systemScheduler->PopGuiSystemType();
        return false;
    }

    RecipeResource* recipeResource = slotComponent->GetRecipeResource();
    if (recipeResource == nullptr)
    {
        systemScheduler->PopGuiSystemType();
        return false;
    }

    recipeResource_ = recipeResource;
    if (recipeResource_->input.empty())
    {
        systemScheduler->PopGuiSystemType();
        return false;
    }

    if (resources_.stringManager_ == nullptr)
    {
        return false;
    }

    tagRuntimeDataMap_.clear();
    selectedItemVector_.clear();
    return true;
}

void glimmer::MaterialSelectCraftUISystem::InitializeTagRuntimeData()
{
    std::vector<RequiredTag>& input = recipeResource_->input;
    for (auto& requiredTag : input)
    {
        std::stringstream stringStream;
        uint64_t cachedTagId = requiredTag.cachedTagId;
        std::optional<std::string> tagTranslateOptional = resources_.stringManager_->GetTagTranslate(cachedTagId);
        stringStream << tagTranslateOptional.value_or(requiredTag.requiredTag);
        stringStream << '*';
        stringStream << static_cast<int>(requiredTag.requiredWeight);
        std::string text = stringStream.str();
        auto tagRuntimeData = std::make_unique<TagRuntimeData>();
        tagRuntimeData->SetText(text);
        tagRuntimeData->SetRequiredWeight(requiredTag.requiredWeight);
        tagRuntimeData->SetPositiveTexture(
            resources_.resourcePackManager_->CreateStringTexture(
                text, &resources_.preloadColors_->game.positiveAttributeColor));
        tagRuntimeData->SetNegativeTexture(
            resources_.resourcePackManager_->CreateStringTexture(
                text, &resources_.preloadColors_->game.negativeAttributeColor));
        tagRuntimeDataMap_[cachedTagId] = std::move(tagRuntimeData);
    }
}

bool glimmer::MaterialSelectCraftUISystem::ItemHasMatchingTag(const Item* item) const
{
    if (item == nullptr)
    {
        return false;
    }
    const ItemTagModule* itemTagModule = item->GetTagModule();
    if (itemTagModule == nullptr)
    {
        return false;
    }
    return std::ranges::any_of(tagRuntimeDataMap_, [itemTagModule](const auto& pair)
    {
        return itemTagModule->HasTag(pair.first);
    });
}

void glimmer::MaterialSelectCraftUISystem::CountMatchingItems()
{
    const WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        return;
    }
    SystemScheduler* systemScheduler = worldContext->GetSystemScheduler();
    if (systemScheduler == nullptr)
    {
        return;
    }
    const EntityShortCut* entityShortCut = GetEntityShortCut();
    if (entityShortCut == nullptr)
    {
        return;
    }
    const ItemContainerComponent* itemContainerComponent = entityShortCut->GetItemContainerComponent();
    if (itemContainerComponent == nullptr)
    {
        systemScheduler->PopGuiSystemType();
        return;
    }

    const ItemContainer* itemContainer = itemContainerComponent->GetItemContainer();
    if (itemContainer == nullptr)
    {
        systemScheduler->PopGuiSystemType();
        return;
    }

    const uint8_t capacity = itemContainer->GetCapacity();
    layout_.matchingCount_ = 0;
    for (int i = 0; i < capacity; i++)
    {
        if (const Item* item = itemContainer->GetItem(i); ItemHasMatchingTag(item))
        {
            layout_.matchingCount_++;
        }
    }
}

void glimmer::MaterialSelectCraftUISystem::CalculateMaxTextureSizeForActivation()
{
    layout_.maxTextureWidth_ = 0.0F;
    layout_.maxTextureHeight_ = 0.0F;
    for (const auto& [tagId, tagRuntimeData] : tagRuntimeDataMap_)
    {
        if (tagRuntimeData == nullptr)
        {
            continue;
        }
        SDL_Texture* texture = tagRuntimeData->Matched()
                                   ? tagRuntimeData->GetPositiveTexture()
                                   : tagRuntimeData->GetNegativeTexture();
        if (texture == nullptr)
        {
            continue;
        }
        layout_.maxTextureWidth_ = std::max(layout_.maxTextureWidth_, static_cast<DesignDimension>(texture->w));
        layout_.maxTextureHeight_ = std::max(layout_.maxTextureHeight_, static_cast<DesignDimension>(texture->h));
    }
}

void glimmer::MaterialSelectCraftUISystem::CalculatePanelDimensions()
{
    constexpr DesignDimension panelPadding = 8.0F;
    constexpr DesignDimension cellPadding = 4.0F;
    constexpr uint8_t gridColumns = 6;
    constexpr DesignDimension gridPadding = 4.0F;

    const uint8_t actualColumns = layout_.matchingCount_ > 0 ? std::min(layout_.matchingCount_, gridColumns) : 1;
    const DesignDimension gridWidth = static_cast<float>(actualColumns) * (ITEM_SLOT_SIZE + gridPadding) -
        gridPadding;
    const DesignDimension contentPanelWidth = layout_.maxTextureWidth_ + gridWidth + 2.0F * panelPadding;
    const DesignDimension panelWidth = std::max(contentPanelWidth, MATERIAL_SELECT_CRAFT_PANEL_MIN_WIDTH);

    const DesignDimension tagAreaHeight = static_cast<DesignDimension>(tagRuntimeDataMap_.size()) * (
            layout_.maxTextureHeight_ +
            cellPadding) +
        2.0F * panelPadding - cellPadding;
    const uint32_t gridRows = layout_.matchingCount_ > 0
                                  ? (layout_.matchingCount_ + gridColumns - 1) / gridColumns
                                  : 1;
    const DesignDimension gridAreaHeight = static_cast<DesignDimension>(gridRows) * (ITEM_SLOT_SIZE + gridPadding) -
        gridPadding + 2.0F * panelPadding;
    const DesignDimension contentPanelHeight = std::max(tagAreaHeight, gridAreaHeight);
    const DesignDimension totalPanelHeight = contentPanelHeight + MATERIAL_SELECT_CRAFT_BUTTON_HEIGHT +
        panelPadding;
    layout_.panelWidth_ = panelWidth;
    layout_.panelHeight_ = std::max(totalPanelHeight, MATERIAL_SELECT_CRAFT_PANEL_MIN_HEIGHT);
}

void glimmer::MaterialSelectCraftUISystem::SetupItemSlots()
{
    const WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        return;
    }
    SystemScheduler* systemScheduler = worldContext->GetSystemScheduler();
    if (systemScheduler == nullptr)
    {
        return;
    }
    EntityManager* entityManager = GetEntityManager();
    const EntityShortCut* entityShortCut = GetEntityShortCut();

    const ItemContainerComponent* itemContainerComponent = entityShortCut->GetItemContainerComponent();
    if (itemContainerComponent == nullptr)
    {
        systemScheduler->PopGuiSystemType();
        return;
    }

    ItemContainer* itemContainer = itemContainerComponent->GetItemContainer();
    if (itemContainer == nullptr)
    {
        systemScheduler->PopGuiSystemType();
        return;
    }

    uint8_t capacity = itemContainer->GetCapacity();
    uint8_t nextIndex = 0;

    for (int i = 0; i < capacity; i++)
    {
        const Item* item = itemContainer->GetItem(i);
        if (!ItemHasMatchingTag(item))
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
            itemSlotQuantityComponent = entityManager->AddComponent<
                ItemSlotQuantityComponent>(entityManager->AddEntity());
            itemSlotQuantityComponent->SetOnSelectQuantityChanged(
                [this](uint8_t slotIndex, Item* item, uint8_t selectQuantity)
                {
                    HandleSelectQuantityChanged(slotIndex, item, selectQuantity);
                });
            itemSlotQuantityVector_.emplace_back(itemSlotQuantityComponent);
        }

        if (itemSlotQuantityComponent == nullptr)
        {
            systemScheduler->PopGuiSystemType();
            return;
        }

        itemSlotQuantityComponent->Show();
        itemSlotQuantityComponent->SetSelectQuantity(0);
        itemSlotQuantityComponent->SetSize({ITEM_SLOT_SIZE, ITEM_SLOT_SIZE});
        itemSlotQuantityComponent->SetItemContainer(itemContainer);
        itemSlotQuantityComponent->SetSlotIndex(i);
        nextIndex++;
    }

    for (size_t i = nextIndex; i < itemSlotQuantityVector_.size(); i++)
    {
        itemSlotQuantityVector_[i]->Hide();
    }

    UpdateItemSlotPositions();
}

glimmer::SelectedItemRuntimeData* glimmer::MaterialSelectCraftUISystem::FindSelectedItemBySlotIndex(
    const uint8_t slotIndex) const
{
    for (const auto& selectedItem : selectedItemVector_)
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
        if (temporarySelectedItemRuntimeData->GetSlotIndex() == slotIndex)
        {
            return temporarySelectedItemRuntimeData;
        }
    }
    return nullptr;
}

void glimmer::MaterialSelectCraftUISystem::RecalculateTagActualValues()
{
    for (const auto& [tagId, tagRuntimeData] : tagRuntimeDataMap_)
    {
        tagRuntimeData->SetActualValue(0);
    }

    for (const auto& selectedItem : selectedItemVector_)
    {
        const uint8_t selectAmount = selectedItem->GetSelectedAmount();
        if (selectAmount == 0)
        {
            continue;
        }

        const Item* selectedItemPtr = selectedItem->GetItem();
        if (selectedItemPtr == nullptr)
        {
            continue;
        }

        const ItemTagModule* itemTagModule = selectedItemPtr->GetTagModule();
        if (itemTagModule == nullptr)
        {
            continue;
        }
        for (const auto& [tagId, tagRuntimeData] : tagRuntimeDataMap_)
        {
            const ItemTagResource* itemTagResource = itemTagModule->GetItemTagResource(tagId);
            if (itemTagResource == nullptr)
            {
                continue;
            }
            tagRuntimeData->AddActualValue(itemTagResource->value * selectAmount);
        }
    }
}

void glimmer::MaterialSelectCraftUISystem::UpdateButtonEnabledState()
{
    bool allMatched = true;
    for (auto& [tagId, tagRuntimeData] : tagRuntimeDataMap_)
    {
        if (tagRuntimeData == nullptr)
        {
            continue;
        }
        if (!tagRuntimeData->Matched())
        {
            allMatched = false;
            break;
        }
    }

    if (allMatched)
    {
        buttonComponent_->Enable();
    }
    else
    {
        buttonComponent_->Disable();
    }
}

void glimmer::MaterialSelectCraftUISystem::HandleSelectQuantityChanged(uint8_t slotIndex, Item* item,
                                                                       uint8_t selectQuantity)
{
    SelectedItemRuntimeData* selectedItemRuntimeData = FindSelectedItemBySlotIndex(slotIndex);

    if (selectedItemRuntimeData == nullptr)
    {
        auto selectedItemRuntimeDataUnique = std::make_unique<SelectedItemRuntimeData>();
        selectedItemRuntimeDataUnique->SetSlotIndex(slotIndex);
        selectedItemRuntimeDataUnique->SetItem(item);
        selectedItemVector_.emplace_back(std::move(selectedItemRuntimeDataUnique));
        selectedItemRuntimeData = selectedItemVector_.back().get();
    }

    selectedItemRuntimeData->SetSelectedAmount(selectQuantity);
    RecalculateTagActualValues();
    UpdateButtonEnabledState();
}

void glimmer::MaterialSelectCraftUISystem::SetupButton()
{
    EntityManager* entityManager = GetEntityManager();

    if (buttonComponent_ == nullptr)
    {
        buttonComponent_ = entityManager->AddComponent<ButtonComponent>(entityManager->AddEntity());
        buttonComponent_->SetText(resources_.appContext_, resources_.langsResources_->craft);
        buttonComponent_->SetClickCallback([this]
        {
            HandleCraftButtonClick();
        });
    }

    buttonComponent_->Disable();
    buttonComponent_->Show();
    UpdateButtonPosition();
}

void glimmer::MaterialSelectCraftUISystem::HandleCraftButtonClick() const
{
    WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        return;
    }
    SystemScheduler* systemScheduler = worldContext->GetSystemScheduler();
    if (systemScheduler == nullptr)
    {
        return;
    }
    EntityShortCut* entityShortCut = GetEntityShortCut();
    if (entityShortCut == nullptr)
    {
        return;
    }
    ItemContainerComponent* itemContainerComponent = entityShortCut->GetItemContainerComponent();
    if (itemContainerComponent == nullptr)
    {
        systemScheduler->PopGuiSystemType();
        return;
    }

    ItemContainer* itemContainer = itemContainerComponent->GetItemContainer();
    if (itemContainer == nullptr)
    {
        systemScheduler->PopGuiSystemType();
        return;
    }

    for (auto& selectedItem : selectedItemVector_)
    {
        uint8_t expectedQuantity = selectedItem->GetSelectedAmount();
        uint8_t actualQuantity = itemContainer->RemoveItemAt(selectedItem->GetSlotIndex(), expectedQuantity);
        if (expectedQuantity != actualQuantity)
        {
            LogCat::w("Failed to remove the item. At ", selectedItem->GetSlotIndex());
        }
    }

    if (resources_.resourceLocator_ == nullptr)
    {
        return;
    }

    std::unique_ptr<Item> outputItem = resources_.resourceLocator_->FindItem(worldContext, recipeResource_->output);
    if (outputItem != nullptr)
    {
        std::unique_ptr<Item> returnItem = itemContainer->AddItem(std::move(outputItem));
        if (returnItem != nullptr)
        {
            LogCat::w("Failed to add the item. ");
        }
    }
    systemScheduler->PopGuiSystemType();
}

void glimmer::MaterialSelectCraftUISystem::DeactivateUI() const
{
    for (auto itemSlotQuantityComponent : itemSlotQuantityVector_)
    {
        itemSlotQuantityComponent->Hide();
    }
    if (buttonComponent_ != nullptr)
    {
        buttonComponent_->Hide();
    }
}

void glimmer::MaterialSelectCraftUISystem::OnConfigChanged(const Config* config)
{
    layout_.uiScale_ = config->window.uiScale;
    layout_.panelInnerPadding_ = layout_.basePanelInnerPadding_ * layout_.uiScale_;
}

void glimmer::MaterialSelectCraftUISystem::OnWindowSizeChanged(const int& width, const int& height)
{
    layout_.windowHeight_ = height;
    layout_.windowWidth_ = width;
    UpdateItemSlotPositions();
    UpdateButtonPosition();
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

    const DesignDimension panelOffsetX = (static_cast<DesignDimension>(layout_.windowWidth_) / layout_.uiScale_ -
            layout_.panelWidth_) *
        0.5F;
    const DesignDimension panelOffsetY = (static_cast<DesignDimension>(layout_.windowHeight_) / layout_.uiScale_ -
            layout_.panelHeight_) *
        0.5F;

    const DesignVector2D gridStartPosition{
        panelOffsetX + layout_.maxTextureWidth_ + panelPadding, panelOffsetY + panelPadding
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

void glimmer::MaterialSelectCraftUISystem::UpdateButtonPosition() const
{
    if (buttonComponent_ == nullptr)
    {
        return;
    }

    constexpr DesignDimension panelPadding = 8.0F;

    const DesignDimension panelOffsetX = (static_cast<DesignDimension>(layout_.windowWidth_) / layout_.uiScale_ -
            layout_.panelWidth_) *
        0.5F;
    const DesignDimension panelOffsetY = (static_cast<DesignDimension>(layout_.windowHeight_) / layout_.uiScale_ -
            layout_.panelHeight_) *
        0.5F;

    const DesignVector2D buttonPosition{
        panelOffsetX + (layout_.panelWidth_ - buttonComponent_->GetSize().x) * 0.5F,
        panelOffsetY + layout_.panelHeight_ - MATERIAL_SELECT_CRAFT_BUTTON_HEIGHT - panelPadding
    };
    buttonComponent_->SetPosition(buttonPosition);
}

void glimmer::MaterialSelectCraftUISystem::Render(SDL_Renderer* renderer)
{
    LoadBackgroundTextures();

    if (tagRuntimeDataMap_.empty() || resources_.preloadColors_ == nullptr)
    {
        return;
    }

    DesignDimension maxTextureWidth = 0.0F;
    DesignDimension maxTextureHeight = 0.0F;
    CalculateMaxTextureSizeForRender(maxTextureWidth, maxTextureHeight);

    if (maxTextureWidth <= 0.0F || maxTextureHeight <= 0.0F)
    {
        return;
    }

    const float scaledPanelWidth = layout_.panelWidth_ * layout_.uiScale_;
    const float scaledPanelHeight = layout_.panelHeight_ * layout_.uiScale_;
    const float panelX = (static_cast<float>(layout_.windowWidth_) - scaledPanelWidth) * 0.5F;
    const float panelY = (static_cast<float>(layout_.windowHeight_) - scaledPanelHeight) * 0.5F;

    RenderPanelBackground(renderer, panelX, panelY, maxTextureWidth, maxTextureHeight);
    RenderTagTextures(renderer, panelX, panelY, maxTextureWidth, maxTextureHeight);

    AppContext::RestoreColorRenderer(renderer);
}

void glimmer::MaterialSelectCraftUISystem::LoadBackgroundTextures()
{
    if (resources_.resourceLocator_ == nullptr)
    {
        return;
    }

    if (panelBackGroundTextureResult_ == nullptr)
    {
        ResourceRef panelBGResourceRef;
        panelBGResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
        panelBGResourceRef.SetResourceType(RESOURCE_TEXTURE);
        panelBGResourceRef.SetResourceKey("gui/panel_bg");
        panelBackGroundTextureResult_ = resources_.resourceLocator_->FindTexture(&panelBGResourceRef);
    }

    if (subPanelBackGroundTextureResult_ == nullptr)
    {
        ResourceRef panelBGResourceRef;
        panelBGResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
        panelBGResourceRef.SetResourceType(RESOURCE_TEXTURE);
        panelBGResourceRef.SetResourceKey("gui/sub_panel_bg");
        subPanelBackGroundTextureResult_ = resources_.resourceLocator_->FindTexture(&panelBGResourceRef);
    }
}

void glimmer::MaterialSelectCraftUISystem::CalculateMaxTextureSizeForRender(DesignDimension& maxWidth,
                                                                            DesignDimension& maxHeight) const
{
    maxWidth = 0.0F;
    maxHeight = 0.0F;
    for (const auto& [tagId, tagRuntimeData] : tagRuntimeDataMap_)
    {
        if (tagRuntimeData == nullptr)
        {
            continue;
        }
        const auto tagRuntimeDataPtr = tagRuntimeData.get();
        if (tagRuntimeDataPtr == nullptr)
        {
            continue;
        }

        SDL_Texture* texture = tagRuntimeDataPtr->Matched()
                                   ? tagRuntimeDataPtr->GetPositiveTexture()
                                   : tagRuntimeDataPtr->GetNegativeTexture();
        if (texture == nullptr)
        {
            continue;
        }
        maxWidth = std::max(maxWidth, static_cast<DesignDimension>(texture->w));
        maxHeight = std::max(maxHeight, static_cast<DesignDimension>(texture->h));
    }
}

void glimmer::MaterialSelectCraftUISystem::RenderPanelBackground(SDL_Renderer* renderer, float panelX, float panelY,
                                                                 DesignDimension maxTextureWidth,
                                                                 DesignDimension maxTextureHeight) const
{
    constexpr DesignDimension panelPadding = 8.0F;
    constexpr DesignDimension cellPadding = 4.0F;
    const auto dataLength = static_cast<uint32_t>(tagRuntimeDataMap_.size());

    const float scaledPanelWidth = layout_.panelWidth_ * layout_.uiScale_;
    const float scaledPanelHeight = layout_.panelHeight_ * layout_.uiScale_;
    const SDL_FRect panelRect{panelX, panelY, scaledPanelWidth, scaledPanelHeight};

    if (panelBackGroundTextureResult_ != nullptr)
    {
        SDL_Texture* panelBackGroundTexture = panelBackGroundTextureResult_->GetResource();
        if (panelBackGroundTexture != nullptr)
        {
            SDL_RenderTexture(renderer, panelBackGroundTexture, nullptr, &panelRect);
        }
    }

    if (subPanelBackGroundTextureResult_ == nullptr)
    {
        return;
    }

    SDL_Texture* subPanelBackGroundTexture = subPanelBackGroundTextureResult_->GetResource();
    if (subPanelBackGroundTexture == nullptr || dataLength == 0)
    {
        return;
    }

    const DesignDimension contentAreaHeight = layout_.panelHeight_ - MATERIAL_SELECT_CRAFT_BUTTON_HEIGHT - panelPadding;
    const DesignDimension tagAreaHeight = static_cast<DesignDimension>(dataLength) * (maxTextureHeight +
            cellPadding) +
        2.0F * panelPadding - cellPadding;
    const float scaledTagSubPanelWidth = maxTextureWidth * layout_.uiScale_;
    const float scaledContentAreaHeight = std::max(tagAreaHeight, contentAreaHeight) * layout_.uiScale_;
    const SDL_FRect tagSubPanelRect{
        panelX + layout_.panelInnerPadding_,
        panelY + layout_.panelInnerPadding_,
        scaledTagSubPanelWidth,
        scaledContentAreaHeight
    };
    SDL_RenderTexture(renderer, subPanelBackGroundTexture, nullptr, &tagSubPanelRect);

    const DesignDimension gridSubPanelWidth = layout_.panelWidth_ - maxTextureWidth - 2.0F * panelPadding;
    const float scaledGridSubPanelWidth = gridSubPanelWidth * layout_.uiScale_;
    const float gridSubPanelX = panelX + (maxTextureWidth + panelPadding) * layout_.uiScale_;
    const SDL_FRect gridSubPanelRect{
        gridSubPanelX,
        panelY + layout_.panelInnerPadding_,
        scaledGridSubPanelWidth,
        scaledContentAreaHeight
    };
    SDL_RenderTexture(renderer, subPanelBackGroundTexture, nullptr, &gridSubPanelRect);
}

void glimmer::MaterialSelectCraftUISystem::RenderTagTextures(SDL_Renderer* renderer, float panelX, float panelY,
                                                             DesignDimension maxTextureWidth,
                                                             DesignDimension maxTextureHeight) const
{
    constexpr DesignDimension panelPadding = 8.0F;
    constexpr DesignDimension cellPadding = 4.0F;
    const auto dataLength = static_cast<uint32_t>(tagRuntimeDataMap_.size());

    DesignVector2D startPosition{panelPadding, panelPadding};
    VerticalLayoutStepper stepper(maxTextureHeight, startPosition, cellPadding, dataLength);

    for (const auto& [tagId, tagRuntimeData] : tagRuntimeDataMap_)
    {
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
        SDL_Texture* texture = tagRuntimeDataPtr->Matched()
                                   ? tagRuntimeDataPtr->GetPositiveTexture()
                                   : tagRuntimeDataPtr->GetNegativeTexture();
        if (texture == nullptr)
        {
            continue;
        }

        const SDL_FRect dstRect{
            panelX + position.x * layout_.uiScale_,
            panelY + position.y * layout_.uiScale_,
            static_cast<float>(texture->w) * layout_.uiScale_,
            static_cast<float>(texture->h) * layout_.uiScale_
        };
        SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
    }
}

uint8_t glimmer::MaterialSelectCraftUISystem::GetRenderOrder()
{
    return RENDER_ORDER_MATERIAL_SELECT_CRAFT_UI;
}

glimmer::GameSystemType glimmer::MaterialSelectCraftUISystem::GetGameSystemType() const
{
    return GameSystemType::MaterialSelectCraftUISystem;
}
