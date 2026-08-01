/*
* Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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
 */
#include "RecipeDetailGUISystem.h"

#include <algorithm>

#include "InventoryGUISystem.h"
#include "core/context/AppContext.h"
#include "core/ecs/EntityManager.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/log/LogCat.h"
#include "core/mod/ResourceLocator.h"
#include "core/utils/StringUtils.h"
#include "core/world/SystemScheduler.h"
#include "core/world/WorldContext.h"

void glimmer::RecipeDetailGUISystem::LoadRecipeDetail()
{
    investedItems_.clear();
    tagProgress_.clear();
    itemChoices_.clear();
    originalRequiredWeights_.clear();
    canCraft_ = false;
    craftCount_ = 1;
    maxCraftCount_ = 1;
    if (currentRecipe_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "currentRecipe_ == nullptr");
        return;
    }

    WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        LogCat::w(std::source_location::current(), "worldContext == nullptr");
        return;
    }

    AppContext* appContext = worldContext->GetAppContext();
    if (appContext == nullptr)
    {
        LogCat::w(std::source_location::current(), "appContext == nullptr");
        return;
    }
    ModContext* modContext = appContext->GetModContext();
    if (modContext == nullptr)
    {
        LogCat::w(std::source_location::current(), "modContext == nullptr");
        return;
    }
    StringManager* stringManager = modContext->GetStringManager();
    if (stringManager == nullptr)
    {
        LogCat::w(std::source_location::current(), "stringManager == nullptr");
        return;
    }
    ResourceLocator* resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator == nullptr)
    {
        LogCat::w(std::source_location::current(), "resourceLocator == nullptr");
        return;
    }
    auto outputItem = resourceLocator->FindItem(worldContext, currentRecipe_->output);
    if (outputItem == nullptr)
    {
        LogCat::w(std::source_location::current(), "outputItem == nullptr");
        return;
    }
    outputName_ = outputItem->GetName();
    const ResourceRef* iconRef = outputItem->GetIconResourceRef();
    if (iconRef == nullptr)
    {
        outputImage_ = "";
    }
    else
    {
        outputImage_ = StringUtils::MakeTextureUrl(
            Resource::GenerateId(iconRef->GetPackageId(), iconRef->GetResourceKey()));
    }
    const ItemStackModule* stackModule = outputItem->GetStackModule();
    if (stackModule == nullptr)
    {
        LogCat::w(std::source_location::current(), "stackModule == nullptr");
        return;
    }
    outputAmount_ = static_cast<int>(currentRecipe_->output.amount);
    if (uint8_t maxStack = stackModule->GetMaxStack(); maxStack > 0 && outputAmount_ > 0)
    {
        maxCraftCount_ = static_cast<int>(maxStack) / outputAmount_;
        if (maxCraftCount_ < 1)
        {
            maxCraftCount_ = 1;
        }
    }
    outputCount_ = outputAmount_ * craftCount_;
    tagProgress_.reserve(currentRecipe_->input.size());
    originalRequiredWeights_.reserve(currentRecipe_->input.size());
    for (const auto& requiredTag : currentRecipe_->input)
    {
        TagProgressDataModel tp;
        if (auto optional = stringManager->GetTagTranslate(requiredTag.cachedTagId); optional.has_value())
        {
            tp.tagName = optional.value();
        }
        else
        {
            tp.tagName = requiredTag.requiredTag;
        }
        tp.requiredWeight = static_cast<int>(requiredTag.requiredWeight);
        tp.currentWeight = 0;
        tp.satisfied = false;
        tp.progressWidth = "0%";
        originalRequiredWeights_.push_back(tp.requiredWeight);
        tagProgress_.push_back(std::move(tp));
    }
    if (itemContainer_ != nullptr)
    {
        std::unordered_set<uint64_t> requiredTagIds;
        for (const auto& requiredTag : currentRecipe_->input)
        {
            requiredTagIds.insert(requiredTag.cachedTagId);
        }

        uint8_t capacity = itemContainer_->GetCapacity();
        for (uint8_t i = 0; i < capacity; ++i)
        {
            const Item* item = itemContainer_->GetItem(i);
            if (item == nullptr)
            {
                continue;
            }
            const ItemLockModule* lockModule = item->GetLockModule();
            if (lockModule != nullptr && lockModule->IsLocked())
            {
                continue;
            }
            const ItemTagModule* tagModule = item->GetTagModule();
            if (tagModule == nullptr)
            {
                continue;
            }
            const std::vector<uint64_t>& tags = tagModule->GetTags();

            std::stringstream tagInfoStringStream;
            bool tagInfoStringStreamEmpty = true;
            bool hasAnyRequiredTag = false;
            for (uint64_t tagId : tags)
            {
                if (requiredTagIds.find(tagId) == requiredTagIds.end())
                {
                    continue;
                }
                const ItemTagResource* tagRes = tagModule->GetItemTagResource(tagId);
                if (tagRes == nullptr)
                {
                    continue;
                }
                hasAnyRequiredTag = true;
                if (!tagInfoStringStreamEmpty)
                {
                    tagInfoStringStream << ",";
                }

                if (auto optional = stringManager->GetTagTranslate(tagRes->cachedTagId); optional.has_value())
                {
                    tagInfoStringStream << optional.value();
                }
                else
                {
                    tagInfoStringStream << tagRes->name;
                }
                if (tagRes->value > 1)
                {
                    tagInfoStringStream << "+" << tagRes->value;
                }
                tagInfoStringStreamEmpty = false;
            }
            if (!hasAnyRequiredTag)
            {
                continue;
            }

            RecipeItemChoiceDataModel choice;
            choice.inventoryIndex = static_cast<int>(i);
            choice.name = item->GetName();
            const ResourceRef* iconRef = item->GetIconResourceRef();
            if (iconRef != nullptr)
            {
                choice.image = StringUtils::MakeTextureUrl(
                    Resource::GenerateId(iconRef->GetPackageId(), iconRef->GetResourceKey()));
            }
            choice.tagInfo = tagInfoStringStream.str();
            const ItemStackModule* stackModule = item->GetStackModule();
            choice.backpackAmount = stackModule != nullptr ? static_cast<int>(stackModule->GetAmount()) : 1;
            choice.investedAmount = 0;
            itemChoices_.push_back(std::move(choice));
        }
    }

    UpdateTagDisplayWeights();
    if (constructor_ != nullptr)
    {
        constructor_->GetModelHandle().DirtyAllVariables();
    }
}

void glimmer::RecipeDetailGUISystem::UpdateTagProgress()
{
    if (currentRecipe_ == nullptr || itemContainer_ == nullptr)
    {
        return;
    }
    for (size_t i = 0; i < tagProgress_.size() && i < currentRecipe_->input.size(); ++i)
    {
        RequiredTag& requiredTag = currentRecipe_->input[i];
        const uint64_t tagId = requiredTag.cachedTagId;
        int currentWeight = 0;
        for (const auto& [slotIndex, investedCount] : investedItems_)
        {
            const Item* item = itemContainer_->GetItem(slotIndex);
            if (item == nullptr)
            {
                continue;
            }
            const ItemTagModule* tagModule = item->GetTagModule();
            if (tagModule == nullptr || !tagModule->HasTag(tagId))
            {
                continue;
            }
            const ItemTagResource* tagRes = tagModule->GetItemTagResource(tagId);
            if (tagRes == nullptr)
            {
                continue;
            }
            currentWeight += static_cast<int>(tagRes->value) * static_cast<int>(investedCount);
        }
        TagProgressDataModel& tagProgressDataModel = tagProgress_[i];
        tagProgressDataModel.currentWeight = currentWeight;
        const int scaledRequired = tagProgressDataModel.requiredWeight;
        if (requiredTag.exactMatch)
        {
            tagProgressDataModel.satisfied = currentWeight == scaledRequired;
        }
        else
        {
            tagProgressDataModel.satisfied = currentWeight >= scaledRequired;
        }
        int percent = 0;
        if (scaledRequired > 0)
        {
            percent = currentWeight * 100 / scaledRequired;
            if (percent > 100)
            {
                percent = 100;
            }
            if (percent < 0)
            {
                percent = 0;
            }
        }
        tagProgressDataModel.progressWidth = std::to_string(percent) + "%";
    }
    if (constructor_ != nullptr)
    {
        constructor_->GetModelHandle().DirtyVariable("tag_progress");
        constructor_->GetModelHandle().DirtyVariable("item_choices");
    }
}

void glimmer::RecipeDetailGUISystem::UpdateTagDisplayWeights()
{
    for (size_t i = 0; i < tagProgress_.size() && i < originalRequiredWeights_.size(); ++i)
    {
        tagProgress_[i].requiredWeight = originalRequiredWeights_[i] * craftCount_;
    }
    if (constructor_ != nullptr)
    {
        constructor_->GetModelHandle().DirtyVariable("tag_progress");
    }
}

void glimmer::RecipeDetailGUISystem::UpdateCanCraft()
{
    canCraft_ = true;
    for (const auto& tagProgress : tagProgress_)
    {
        if (!tagProgress.satisfied)
        {
            canCraft_ = false;
            break;
        }
    }

    if (constructor_ != nullptr)
    {
        constructor_->GetModelHandle().DirtyVariable("can_craft");
    }
}

void glimmer::RecipeDetailGUISystem::ExecuteCraftBatch(int count)
{
    if (!canCraft_ || currentRecipe_ == nullptr || itemContainer_ == nullptr || count <= 0)
    {
        return;
    }
    WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        return;
    }
    for (const auto& [slotIndex, investedCount] : investedItems_)
    {
        (void)itemContainer_->TakeItem(slotIndex, investedCount * count);
    }
    investedItems_.clear();
    ResourceLocator* resourceLocator = worldContext->GetAppContext()->GetResourceLocator();
    if (resourceLocator != nullptr)
    {
        auto outputItem = resourceLocator->FindItem(worldContext, currentRecipe_->output);
        if (outputItem != nullptr)
        {
            const ItemStackModule* stackModule = outputItem->GetStackModule();
            if (stackModule != nullptr)
            {
                const_cast<ItemStackModule*>(stackModule)->SetAmount(
                    static_cast<uint8_t>(outputAmount_ * count));
            }
            (void)itemContainer_->AddItem(std::move(outputItem));
        }
    }
    if (systemScheduler_ != nullptr)
    {
        systemScheduler_->PopGuiSystemType();
    }
}

void glimmer::RecipeDetailGUISystem::AutoFill()
{
    if (currentRecipe_ == nullptr || itemContainer_ == nullptr)
    {
        return;
    }
    ResetInvestment();

    struct Candidate
    {
        int choiceIndex;
        int tagValue;
    };

    for (size_t tagIdx = 0; tagIdx < tagProgress_.size() && tagIdx < currentRecipe_->input.size(); ++tagIdx)
    {
        const uint64_t tagId = currentRecipe_->input[tagIdx].cachedTagId;
        const int scaledRequired = tagProgress_[tagIdx].requiredWeight;
        int needed = scaledRequired - tagProgress_[tagIdx].currentWeight;
        if (needed <= 0)
        {
            continue;
        }
        const bool exactMatch = currentRecipe_->input[tagIdx].exactMatch;

        std::vector<Candidate> candidates;
        for (size_t ci = 0; ci < itemChoices_.size(); ++ci)
        {
            const auto& choice = itemChoices_[ci];
            const Item* item = itemContainer_->GetItem(choice.inventoryIndex);
            if (item == nullptr)
            {
                continue;
            }
            const ItemTagModule* tagModule = item->GetTagModule();
            if (tagModule == nullptr || !tagModule->HasTag(tagId))
            {
                continue;
            }
            const ItemTagResource* tagRes = tagModule->GetItemTagResource(tagId);
            if (tagRes == nullptr)
            {
                continue;
            }
            int available = choice.backpackAmount - choice.investedAmount;
            if (available <= 0)
            {
                continue;
            }
            candidates.push_back({static_cast<int>(ci), static_cast<int>(tagRes->value)});
        }

        std::sort(candidates.begin(), candidates.end(),
                  [](const Candidate& a, const Candidate& b)
                  {
                      return a.tagValue > b.tagValue;
                  });

        for (const auto& cand : candidates)
        {
            if (needed <= 0)
            {
                break;
            }
            auto& choice = itemChoices_[cand.choiceIndex];
            const int slotIndex = choice.inventoryIndex;
            int available = choice.backpackAmount - choice.investedAmount;
            if (available <= 0)
            {
                continue;
            }
            int unitsNeeded = (needed + cand.tagValue - 1) / cand.tagValue;
            if (unitsNeeded > available)
            {
                unitsNeeded = available;
            }
            if (unitsNeeded <= 0)
            {
                continue;
            }
            if (exactMatch)
            {
                int overflow = unitsNeeded * cand.tagValue - needed;
                if (overflow >= cand.tagValue)
                {
                    continue;
                }
                if (overflow > 0)
                {
                    unitsNeeded -= (overflow + cand.tagValue - 1) / cand.tagValue;
                    if (unitsNeeded <= 0)
                    {
                        continue;
                    }
                }
            }
            choice.investedAmount += unitsNeeded;
            investedItems_[static_cast<uint8_t>(slotIndex)] += static_cast<uint8_t>(unitsNeeded);
            needed -= unitsNeeded * cand.tagValue;
        }
    }

    UpdateTagProgress();
    UpdateCanCraft();
    if (constructor_ != nullptr)
    {
        constructor_->GetModelHandle().DirtyAllVariables();
    }
}

void glimmer::RecipeDetailGUISystem::ResetInvestment()
{
    investedItems_.clear();
    for (auto& choice : itemChoices_)
    {
        choice.investedAmount = 0;
    }
    UpdateTagProgress();
    UpdateCanCraft();
    if (constructor_ != nullptr)
    {
        constructor_->GetModelHandle().DirtyAllVariables();
    }
}

glimmer::RecipeDetailGUISystem::~RecipeDetailGUISystem() = default;

glimmer::RecipeDetailGUISystem::RecipeDetailGUISystem(WorldContext* worldContext)
    : GuiStackGameSystem(worldContext)
{
    WatchComponent(COMPONENT_ITEM_CONTAINER);
    WatchComponent(COMPONENT_RECIPE_SELECTION);
    Init();
    systemScheduler_ = worldContext->GetSystemScheduler();
}

void glimmer::RecipeDetailGUISystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType,
                                                               uint32_t count)
{
    if (gameComponentType == COMPONENT_ITEM_CONTAINER && itemContainer_ == nullptr)
    {
        const EntityShortCut* entityShortCut = GetEntityShortCut();
        if (entityShortCut == nullptr)
        {
            return;
        }
        const ItemContainerComponent* itemContainerComponent = entityShortCut->GetItemContainerComponent();
        if (itemContainerComponent == nullptr)
        {
            return;
        }
        itemContainer_ = itemContainerComponent->GetItemContainer();
    }
    else if (gameComponentType == COMPONENT_RECIPE_SELECTION)
    {
        const EntityShortCut* entityShortCut = GetEntityShortCut();
        if (entityShortCut == nullptr)
        {
            return;
        }
        recipeSelectionComponent_ = entityShortCut->GetRecipeSelectionComponent();
    }
}

void glimmer::RecipeDetailGUISystem::OnCreateDataModels(IDocumentRegistry* documentRegistry)
{
    constructor_ = documentRegistry->CreateDataModel("recipe_detail");
    if (constructor_ == nullptr)
    {
        return;
    }
    if (auto tagStruct = constructor_->RegisterStruct<TagProgressDataModel>())
    {
        tagStruct.RegisterMember("tagName", &TagProgressDataModel::tagName);
        tagStruct.RegisterMember("requiredWeight", &TagProgressDataModel::requiredWeight);
        tagStruct.RegisterMember("currentWeight", &TagProgressDataModel::currentWeight);
        tagStruct.RegisterMember("satisfied", &TagProgressDataModel::satisfied);
        tagStruct.RegisterMember("progressWidth", &TagProgressDataModel::progressWidth);
        constructor_->RegisterArray<std::vector<TagProgressDataModel>>();
    }
    if (auto choiceStruct = constructor_->RegisterStruct<RecipeItemChoiceDataModel>())
    {
        choiceStruct.RegisterMember("inventoryIndex", &RecipeItemChoiceDataModel::inventoryIndex);
        choiceStruct.RegisterMember("image", &RecipeItemChoiceDataModel::image);
        choiceStruct.RegisterMember("name", &RecipeItemChoiceDataModel::name);
        choiceStruct.RegisterMember("tagInfo", &RecipeItemChoiceDataModel::tagInfo);
        choiceStruct.RegisterMember("investedAmount", &RecipeItemChoiceDataModel::investedAmount);
        choiceStruct.RegisterMember("backpackAmount", &RecipeItemChoiceDataModel::backpackAmount);
        constructor_->RegisterArray<std::vector<RecipeItemChoiceDataModel>>();
    }
    constructor_->Bind("tag_progress", &tagProgress_);
    constructor_->Bind("item_choices", &itemChoices_);
    constructor_->Bind("output_image", &outputImage_);
    constructor_->Bind("output_name", &outputName_);
    constructor_->Bind("output_amount", &outputAmount_);
    constructor_->Bind("can_craft", &canCraft_);
    constructor_->Bind("craft_count", &craftCount_);
    constructor_->Bind("output_count", &outputCount_);
    constructor_->Bind("max_craft_count", &maxCraftCount_);

    constructor_->BindEventCallback("on_back_click", &RecipeDetailGUISystem::OnBackClick, this);
    constructor_->BindEventCallback("on_craft_click", &RecipeDetailGUISystem::OnCraftClick, this);
    constructor_->BindEventCallback("on_reset_click", &RecipeDetailGUISystem::OnResetClick, this);
    constructor_->BindEventCallback("on_auto_fill_click", &RecipeDetailGUISystem::OnAutoFillClick, this);
    constructor_->BindEventCallback("on_craft_count_increase", &RecipeDetailGUISystem::OnCraftCountIncrease, this);
    constructor_->BindEventCallback("on_craft_count_decrease", &RecipeDetailGUISystem::OnCraftCountDecrease, this);
    constructor_->BindEventCallback("on_item_increase", &RecipeDetailGUISystem::OnItemIncrease, this);
    constructor_->BindEventCallback("on_item_decrease", &RecipeDetailGUISystem::OnItemDecrease, this);
}

void glimmer::RecipeDetailGUISystem::OnActivationChanged(bool activeStatus)
{
    GuiStackGameSystem::OnActivationChanged(activeStatus);
    if (activeStatus)
    {
        currentRecipe_ = recipeSelectionComponent_->GetRecipeResource();
        LoadRecipeDetail();
    }
    else
    {
        investedItems_.clear();
    }
}

SDL_Scancode glimmer::RecipeDetailGUISystem::GetHotKey() const
{
    return SDL_SCANCODE_UNKNOWN;
}

glimmer::GameSystemType glimmer::RecipeDetailGUISystem::GetGameSystemType() const
{
    return GameSystemType::RecipeDetailGUISystem;
}

void glimmer::RecipeDetailGUISystem::LoadDocuments(IDocumentRegistry* documentRegistry)
{
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("recipe_detail/recipe_detail");
    SetAndHideElementDocument(documentRegistry->LoadSingleDocument(&resourceRef));
}

void glimmer::RecipeDetailGUISystem::OnBackClick(Rml::DataModelHandle handle, Rml::Event& event,
                                                 const Rml::VariantList& args)
{
    ResetInvestment();
    if (systemScheduler_ != nullptr)
    {
        systemScheduler_->PopGuiSystemType();
    }
}

void glimmer::RecipeDetailGUISystem::OnCraftClick(Rml::DataModelHandle handle, Rml::Event& event,
                                                  const Rml::VariantList& args)
{
    ExecuteCraftBatch(craftCount_);
}

void glimmer::RecipeDetailGUISystem::OnAutoFillClick(Rml::DataModelHandle handle, Rml::Event& event,
                                                     const Rml::VariantList& args)
{
    AutoFill();
}

void glimmer::RecipeDetailGUISystem::OnCraftCountIncrease(Rml::DataModelHandle handle, Rml::Event& event,
                                                          const Rml::VariantList& args)
{
    if (craftCount_ >= maxCraftCount_)
    {
        craftCount_ = 1;
    }
    else
    {
        craftCount_++;
    }
    UpdateTagDisplayWeights();
    UpdateTagProgress();
    UpdateCanCraft();
    outputCount_ = outputAmount_ * craftCount_;
    if (constructor_ != nullptr)
    {
        constructor_->GetModelHandle().DirtyVariable("output_count");
        constructor_->GetModelHandle().DirtyVariable("craft_count");
    }
}

void glimmer::RecipeDetailGUISystem::OnCraftCountDecrease(Rml::DataModelHandle handle, Rml::Event& event,
                                                          const Rml::VariantList& args)
{
    if (craftCount_ <= 1)
    {
        craftCount_ = maxCraftCount_;
    }
    else
    {
        craftCount_--;
    }
    UpdateTagDisplayWeights();
    UpdateTagProgress();
    UpdateCanCraft();
    outputCount_ = outputAmount_ * craftCount_;
    if (constructor_ != nullptr)
    {
        constructor_->GetModelHandle().DirtyVariable("output_count");
        constructor_->GetModelHandle().DirtyVariable("craft_count");
    }
}

void glimmer::RecipeDetailGUISystem::OnResetClick(Rml::DataModelHandle handle, Rml::Event& event,
                                                  const Rml::VariantList& args)
{
    ResetInvestment();
}

void glimmer::RecipeDetailGUISystem::OnItemIncrease(Rml::DataModelHandle handle, Rml::Event& event,
                                                    const Rml::VariantList& args)
{
    if (args.empty())
    {
        return;
    }
    int inventoryIndex = args[0].Get<int>();
    // 查找对应的 itemChoice
    RecipeItemChoiceDataModel* choice = nullptr;
    for (auto& c : itemChoices_)
    {
        if (c.inventoryIndex == inventoryIndex)
        {
            choice = &c;
            break;
        }
    }
    if (choice == nullptr)
    {
        return;
    }
    auto slotIndex = static_cast<uint8_t>(inventoryIndex);
    if (choice->investedAmount >= choice->backpackAmount)
    {
        choice->investedAmount = 0;
        investedItems_.erase(slotIndex);
    }
    else
    {
        choice->investedAmount++;
        investedItems_[slotIndex]++;
    }
    UpdateTagProgress();
    UpdateCanCraft();
}

void glimmer::RecipeDetailGUISystem::OnItemDecrease(Rml::DataModelHandle handle, Rml::Event& event,
                                                    const Rml::VariantList& args)
{
    if (args.empty())
    {
        return;
    }
    int inventoryIndex = args[0].Get<int>();
    RecipeItemChoiceDataModel* choice = nullptr;
    for (auto& c : itemChoices_)
    {
        if (c.inventoryIndex == inventoryIndex)
        {
            choice = &c;
            break;
        }
    }
    if (choice == nullptr)
    {
        return;
    }
    auto slotIndex = static_cast<uint8_t>(inventoryIndex);
    if (choice->investedAmount <= 0)
    {
        choice->investedAmount = choice->backpackAmount;
        investedItems_[slotIndex] = static_cast<uint8_t>(choice->backpackAmount);
    }
    else
    {
        choice->investedAmount--;
        auto it = investedItems_.find(slotIndex);
        if (it != investedItems_.end())
        {
            if (it->second > 0)
            {
                it->second--;
            }
            if (it->second == 0)
            {
                investedItems_.erase(it);
            }
        }
    }
    UpdateTagProgress();
    UpdateCanCraft();
}
