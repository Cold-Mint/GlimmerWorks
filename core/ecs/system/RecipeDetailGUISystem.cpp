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

#include "InventoryGUISystem.h"
#include "core/context/AppContext.h"
#include "core/context/ModContext.h"
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
    canCraft_ = false;
    currentRecipe_ = nullptr;

    WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        return;
    }
    SystemScheduler* scheduler = worldContext->GetSystemScheduler();
    if (scheduler == nullptr)
    {
        return;
    }
    auto* inventorySystem = dynamic_cast<InventoryGUISystem*>(
        scheduler->GetGameSystem(GameSystemType::InventoryGUISystem));
    if (inventorySystem == nullptr)
    {
        return;
    }
    currentRecipe_ = inventorySystem->GetSelectedRecipe();
    if (currentRecipe_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "currentRecipe_ == nullptr");
        return;
    }

    // 获取输出物品信息
    ResourceLocator* resourceLocator = worldContext->GetAppContext()->GetResourceLocator();
    if (resourceLocator != nullptr)
    {
        auto outputItem = resourceLocator->FindItem(worldContext, currentRecipe_->output);
        if (outputItem != nullptr)
        {
            outputName_ = outputItem->GetName();
            const ResourceRef* iconRef = outputItem->GetIconResourceRef();
            if (iconRef != nullptr)
            {
                outputImage_ = StringUtils::MakeTextureUrl(
                    Resource::GenerateId(iconRef->GetPackageId(), iconRef->GetResourceKey()));
            }
            else
            {
                outputImage_ = "";
            }
        }
        else
        {
            outputName_ = "";
            outputImage_ = "";
        }
    }
    outputAmount_ = static_cast<int>(currentRecipe_->output.amount);

    // 构建标签进度列表
    tagProgress_.reserve(currentRecipe_->input.size());
    for (const auto& requiredTag : currentRecipe_->input)
    {
        TagProgressDataModel tp;
        tp.tagName = requiredTag.requiredTag;
        tp.requiredWeight = static_cast<int>(requiredTag.requiredWeight);
        tp.currentWeight = 0;
        tp.satisfied = false;
        tagProgress_.push_back(std::move(tp));
    }

    // 构建可选物品列表
    if (itemContainer_ != nullptr)
    {
        // 收集所有需要的 cachedTagId
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

            // 检查物品是否包含任何所需标签，并构建 tagInfo
            std::string tagInfo;
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
                if (!tagInfo.empty())
                {
                    tagInfo += ",";
                }
                tagInfo += tagRes->name + "+" + std::to_string(tagRes->value);
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
            choice.tagInfo = std::move(tagInfo);
            const ItemStackModule* stackModule = item->GetStackModule();
            choice.backpackAmount = stackModule != nullptr ? static_cast<int>(stackModule->GetAmount()) : 1;
            choice.investedAmount = 0;
            itemChoices_.push_back(std::move(choice));
        }
    }

    DirtyAllVariables();
}

void glimmer::RecipeDetailGUISystem::UpdateTagProgress()
{
    if (currentRecipe_ == nullptr || itemContainer_ == nullptr)
    {
        return;
    }
    for (size_t i = 0; i < tagProgress_.size() && i < currentRecipe_->input.size(); ++i)
    {
        const uint64_t tagId = currentRecipe_->input[i].cachedTagId;
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
        tagProgress_[i].currentWeight = currentWeight;
        const bool exactMatch = currentRecipe_->input[i].exactMatch;
        if (exactMatch)
        {
            tagProgress_[i].satisfied = currentWeight == tagProgress_[i].requiredWeight;
        }
        else
        {
            tagProgress_[i].satisfied = currentWeight >= tagProgress_[i].requiredWeight;
        }
    }
}

void glimmer::RecipeDetailGUISystem::UpdateCanCraft()
{
    canCraft_ = true;
    for (const auto& tp : tagProgress_)
    {
        if (!tp.satisfied)
        {
            canCraft_ = false;
            break;
        }
    }
}

void glimmer::RecipeDetailGUISystem::ExecuteCraft()
{
    if (!canCraft_ || currentRecipe_ == nullptr || itemContainer_ == nullptr)
    {
        return;
    }
    WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        return;
    }
    // 消耗投入的物品
    for (const auto& [slotIndex, investedCount] : investedItems_)
    {
        itemContainer_->TakeItem(slotIndex, investedCount);
    }
    investedItems_.clear();
    // 创建输出物品并添加到背包
    ResourceLocator* resourceLocator = worldContext->GetAppContext()->GetResourceLocator();
    if (resourceLocator != nullptr)
    {
        auto outputItem = resourceLocator->FindItem(worldContext, currentRecipe_->output);
        if (outputItem != nullptr)
        {
            itemContainer_->AddItem(std::move(outputItem));
        }
    }
    // 关闭面板
    SystemScheduler* scheduler = worldContext->GetSystemScheduler();
    if (scheduler != nullptr)
    {
        scheduler->PopGuiSystemType();
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
    DirtyAllVariables();
}

void glimmer::RecipeDetailGUISystem::DirtyAllVariables()
{
    if (constructor_ != nullptr)
    {
        constructor_->GetModelHandle().DirtyVariable("tag_progress");
        constructor_->GetModelHandle().DirtyVariable("item_choices");
        constructor_->GetModelHandle().DirtyVariable("output_image");
        constructor_->GetModelHandle().DirtyVariable("output_name");
        constructor_->GetModelHandle().DirtyVariable("output_amount");
        constructor_->GetModelHandle().DirtyVariable("can_craft");
    }
}

glimmer::RecipeDetailGUISystem::~RecipeDetailGUISystem() = default;

glimmer::RecipeDetailGUISystem::RecipeDetailGUISystem(WorldContext* worldContext)
    : GuiStackGameSystem(worldContext)
{
    WatchComponent(COMPONENT_ITEM_CONTAINER);
    Init();
}

void glimmer::RecipeDetailGUISystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType,
                                                                uint32_t count)
{
    if (gameComponentType != COMPONENT_ITEM_CONTAINER)
    {
        return;
    }
    if (itemContainer_ != nullptr)
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
        return;
    }
    itemContainer_ = itemContainerComponent->GetItemContainer();
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

    constructor_->BindEventCallback("on_back_click", &RecipeDetailGUISystem::OnBackClick, this);
    constructor_->BindEventCallback("on_craft_click", &RecipeDetailGUISystem::OnCraftClick, this);
    constructor_->BindEventCallback("on_reset_click", &RecipeDetailGUISystem::OnResetClick, this);
    constructor_->BindEventCallback("on_item_increase", &RecipeDetailGUISystem::OnItemIncrease, this);
    constructor_->BindEventCallback("on_item_decrease", &RecipeDetailGUISystem::OnItemDecrease, this);
}

void glimmer::RecipeDetailGUISystem::OnActivationChanged(bool activeStatus)
{
    GuiStackGameSystem::OnActivationChanged(activeStatus);
    if (activeStatus)
    {
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
    WorldContext* worldContext = GetWorldContext();
    if (worldContext != nullptr)
    {
        SystemScheduler* scheduler = worldContext->GetSystemScheduler();
        if (scheduler != nullptr)
        {
            scheduler->PopGuiSystemType();
        }
    }
}

void glimmer::RecipeDetailGUISystem::OnCraftClick(Rml::DataModelHandle handle, Rml::Event& event,
                                                   const Rml::VariantList& args)
{
    ExecuteCraft();
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
    if (choice->investedAmount >= choice->backpackAmount)
    {
        return;
    }
    uint8_t slotIndex = static_cast<uint8_t>(inventoryIndex);
    investedItems_[slotIndex]++;
    choice->investedAmount++;
    UpdateTagProgress();
    UpdateCanCraft();
    DirtyAllVariables();
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
    if (choice == nullptr || choice->investedAmount <= 0)
    {
        return;
    }
    uint8_t slotIndex = static_cast<uint8_t>(inventoryIndex);
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
    UpdateTagProgress();
    UpdateCanCraft();
    DirtyAllVariables();
}
