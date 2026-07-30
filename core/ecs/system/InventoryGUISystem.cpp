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
#include "InventoryGUISystem.h"

#include "core/utils/StringUtils.h"

glimmer::ItemSlotDataModel* glimmer::InventoryGUISystem::GetItemSlotDataModel(uint8_t index)
{
    const uint8_t size = itemSlots_.size();
    if (index >= size)
    {
        LogCat::w(std::source_location::current(), "itemSlotDataModel == nullptr");
        return nullptr;
    }
    return &itemSlots_[index];
}

void glimmer::InventoryGUISystem::LoadInitialItems()
{
    if (itemContainer_ == nullptr)
    {
        LogCat::i("LoadInitialHotbarItems: itemContainer_ is nullptr");
        return;
    }
    LogCat::i("Loading initial hotbar items after data model created...");
    uint8_t capacity = itemContainer_->GetCapacity();
    for (uint8_t i = 0; i < capacity; ++i)
    {
        const Item* item = itemContainer_->GetItem(i);
        auto dataModel = GetItemSlotDataModel(i);
        if (dataModel == nullptr)
        {
            continue;
        }
        dataModel->selected = i == itemContainer_->GetSelectIndex();
        if (item == nullptr)
        {
            dataModel->image = "";
            dataModel->amount = 0;
        }
        else
        {
            const ResourceRef* iconResourceRef = item->GetIconResourceRef();
            dataModel->image = StringUtils::MakeTextureUrl(Resource::GenerateId(iconResourceRef->GetPackageId(),
                iconResourceRef->GetResourceKey()));
            const ItemStackModule* stackModule = item->GetStackModule();
            dataModel->amount = stackModule != nullptr ? stackModule->GetAmount() : 1;
        }
    }
    if (constructor_ != nullptr)
    {
        constructor_->GetModelHandle().DirtyVariable("item_slots");
        LogCat::i("item_slots variable dirtied");
    }
    // itemContainer_->GetTotalTags();
}

glimmer::InventoryGUISystem::~InventoryGUISystem()
{
    if (itemContainer_ != nullptr && callback_ != nullptr)
    {
        itemContainer_->RemoveOnContentChanged(callback_);
    }
}

void glimmer::InventoryGUISystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    if (gameComponentType != COMPONENT_ITEM_CONTAINER)
    {
        return;
    }
    if (itemContainer_ != nullptr)
    {
        LogCat::i("itemContainer already set, skipping");
        return;
    }
    const EntityShortCut* entityShortCut = GetEntityShortCut();
    if (entityShortCut == nullptr)
    {
        LogCat::w(std::source_location::current(), "entityShortCut == nullptr");
        return;
    }

    const ItemContainerComponent* itemContainerComponent = entityShortCut->GetItemContainerComponent();
    if (itemContainerComponent == nullptr)
    {
        LogCat::w(std::source_location::current(), "itemContainerComponent == nullptr");
        return;
    }
    itemContainer_ = itemContainerComponent->GetItemContainer();
    if (itemContainer_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "itemContainer == nullptr");
        return;
    }
    itemSlots_.resize(itemContainer_->GetCapacity());
    callback_ = itemContainer_->AddOnContentChanged(
        [this](const uint8_t index, const Item* item, ContainerChangeType changeType)
        {
            const auto dataModel = GetItemSlotDataModel(index);
            if (dataModel == nullptr)
            {
                LogCat::w(std::source_location::current(), "dataModel == nullptr");
                return;
            }
            const auto stackModule = item->GetStackModule();
            auto amount = 0;
            if (stackModule != nullptr)
            {
                amount = stackModule->GetAmount();
            }
            if (amount == 0)
            {
                dataModel->image = "";
                dataModel->amount = 0;
            }
            else
            {
                const ResourceRef* iconResourceRef = item->GetIconResourceRef();
                dataModel->image = StringUtils::MakeTextureUrl(
                    Resource::GenerateId(iconResourceRef->GetPackageId(), iconResourceRef->GetResourceKey()));
                dataModel->amount = amount;
            }
            if (constructor_ != nullptr)
            {
                constructor_->GetModelHandle().DirtyVariable("item_slots");
            }
        });
}

glimmer::InventoryGUISystem::InventoryGUISystem(WorldContext* worldContext)
    : GuiStackGameSystem(worldContext)
{
    WatchComponent(COMPONENT_ITEM_CONTAINER);
    Init();
}

void glimmer::InventoryGUISystem::OnCreateDataModels(IDocumentRegistry* documentRegistry)
{
    constructor_ = documentRegistry->CreateDataModel("inventory");
    if (auto linkStruct = constructor_->RegisterStruct<ItemSlotDataModel>())
    {
        linkStruct.RegisterMember("image", &ItemSlotDataModel::image);
        linkStruct.RegisterMember("amount", &ItemSlotDataModel::amount);
        linkStruct.RegisterMember("selected", &ItemSlotDataModel::selected);
        constructor_->RegisterArray<std::vector<ItemSlotDataModel>>();
    }
    constructor_->Bind("item_slots", &itemSlots_);
    LoadInitialItems();
}

SDL_Scancode glimmer::InventoryGUISystem::GetHotKey() const
{
    return SDL_SCANCODE_E;
}

glimmer::GameSystemType glimmer::InventoryGUISystem::GetGameSystemType() const
{
    return GameSystemType::InventoryGUISystem;
}

void glimmer::InventoryGUISystem::LoadDocuments(IDocumentRegistry* documentRegistry)
{
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("inventory/inventory");
    SetAndHideElementDocument(documentRegistry->LoadSingleDocument(&resourceRef));
}
