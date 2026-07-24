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
#include "HotBarGUISystem.h"
#include "core/log/LogCat.h"
#include "core/rmi/dataModel/ItemSlotDataModel.h"
#include "core/scene/Scene.h"
#include "core/world/WorldContext.h"
#include "core/world/SystemScheduler.h"

glimmer::ItemSlotDataModel* glimmer::HotBarGUISystem::GetItemSlotDataModel(const uint8_t index)
{
    const uint8_t size = itemSlots_.size();
    if (index >= size)
    {
        LogCat::w(std::source_location::current(), "itemSlotDataModel == nullptr");
        return nullptr;
    }
    return &itemSlots_[index];
}

glimmer::HotBarGUISystem::HotBarGUISystem(WorldContext* worldContext) : GuiGameSystem(worldContext)
{
    WatchComponent(COMPONENT_ITEM_CONTAINER);
    Init();
    itemSlots_.resize(HOT_BAR_SIZE);
    LogCat::i("HotBarGUISystem created");
}

glimmer::HotBarGUISystem::~HotBarGUISystem()
{
    if (itemContainer_ != nullptr && callback_ != nullptr)
    {
        itemContainer_->RemoveOnContentChanged(callback_);
    }
}

void glimmer::HotBarGUISystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
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

    callback_ = itemContainer_->AddOnContentChanged([this](uint8_t index, Item* item, ContainerChangeType changeType)
    {
        LogCat::i("AddOnContentChanged index=", std::to_string(index));
        auto dataModel = GetItemSlotDataModel(index);
        if (dataModel == nullptr)
        {
            return;
        }
        const ResourceRef* iconResourceRef = item->GetIconResourceRef();
        dataModel->image = iconResourceRef->GetResourceKey();
        if (constructor_ != nullptr)
        {
            constructor_->GetModelHandle().DirtyVariable("item_slots");
        }
    });
}

void glimmer::HotBarGUISystem::LoadDocuments(IDocumentRegistry* documentRegistry)
{
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("pages/hotbar/hotbar");
    elementDocument_ = documentRegistry->LoadSingleDocument(&resourceRef);
}

void glimmer::HotBarGUISystem::OnCreateDataModels(IDocumentRegistry* documentRegistry)
{
    constructor_ = documentRegistry->CreateDataModel("hotbar");
    if (auto linkStruct = constructor_->RegisterStruct<ItemSlotDataModel>())
    {
        linkStruct.RegisterMember("image", &ItemSlotDataModel::image);
        linkStruct.RegisterMember("amount", &ItemSlotDataModel::amount);
        linkStruct.RegisterMember("background", &ItemSlotDataModel::background);
        linkStruct.RegisterMember("selected_background", &ItemSlotDataModel::selectedBackground);
        constructor_->RegisterArray<std::vector<ItemSlotDataModel>>();
    }
    constructor_->Bind("item_slots", &itemSlots_);
}

bool glimmer::HotBarGUISystem::HandleEvent(const SDL_Event& event)
{
    if (itemContainer_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "itemContainer == nullptr");
        return false;
    }
    if (event.type == SDL_EVENT_MOUSE_WHEEL)
    {
        uint8_t beforeIndex = itemContainer_->GetSelectIndex();
        uint8_t nextIndex = beforeIndex;
        if (event.wheel.y > 0)
        {
            nextIndex -= 1;
        }
        else if (event.wheel.y < 0)
        {
            nextIndex += 1;
        }

        if (nextIndex < 0)
        {
            nextIndex = HOT_BAR_SIZE - 1;
        }
        if (nextIndex >= HOT_BAR_SIZE)
        {
            nextIndex = 0;
        }
        auto beforeDataModel = GetItemSlotDataModel(beforeIndex);
        if (beforeDataModel != nullptr)
        {
            beforeDataModel->selected = false;
        }
        auto nextDataModel = GetItemSlotDataModel(nextIndex);
        if (nextDataModel != nullptr)
        {
            nextDataModel->selected = false;
        }
        if (constructor_ != nullptr)
        {
            constructor_->GetModelHandle().DirtyVariable("item_slots");
        }
        return true;
    }

    return false;
}


glimmer::GameSystemType glimmer::HotBarGUISystem::GetGameSystemType() const
{
    return GameSystemType::HotBarGUISystem;
}
