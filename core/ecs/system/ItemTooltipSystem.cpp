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
#include "ItemTooltipSystem.h"

#include <fmt/format.h>

#include "RmlUi/Core/ElementDocument.h"
#include "core/ecs/EntityShortCut.h"
#include "core/ecs/component/ItemToolTipComponent.h"
#include "core/inventory/Item.h"
#include "core/scene/ConsoleOverlay.h"

void glimmer::ItemTooltipSystem::OnItemChanged(const Item* item)
{
    itemTooltipDataModel_.tooltipName = item->GetName();
    const auto& description = item->GetDescription();
    itemTooltipDataModel_.tooltipDesc = description.has_value() ? *description : "";
    dataModelHandle_.DirtyVariable("tooltip_name");
    dataModelHandle_.DirtyVariable("tooltip_desc");
}

glimmer::ItemTooltipSystem::ItemTooltipSystem(WorldContext* worldContext)
    : GuiGameSystem(worldContext)
{
    WatchComponent(COMPONENT_ITEM_TOOL_TIP);
    Init();
}

void glimmer::ItemTooltipSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    if (gameComponentType == COMPONENT_ITEM_TOOL_TIP)
    {
        itemToolTipComponent_ = GetEntityShortCut()->GetItemToolTipComponent();
        if (itemToolTipComponent_ == nullptr)
        {
            LogCat::e(std::source_location::current(), "itemToolTipComponent_ == nullptr");
        }
    }
}

glimmer::GameSystemType glimmer::ItemTooltipSystem::GetGameSystemType() const
{
    return GameSystemType::ItemToolTipSystem;
}

void glimmer::ItemTooltipSystem::LoadDocuments(IDocumentRegistry* documentRegistry)
{
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("tooltip/tooltip");
    document_ = documentRegistry->LoadSingleDocument(&resourceRef);
    SetElementDocument(document_);
    if (document_ != nullptr)
    {
        document_->Hide();
    }
}

void glimmer::ItemTooltipSystem::OnCreateDataModels(IDocumentRegistry* documentRegistry)
{
    Rml::DataModelConstructor* constructor = documentRegistry->CreateDataModel("tooltip");
    if (constructor == nullptr)
    {
        return;
    }
    constructor->Bind("tooltip_name", &itemTooltipDataModel_.tooltipName);
    constructor->Bind("tooltip_desc", &itemTooltipDataModel_.tooltipDesc);
    constructor->Bind("tooltip_left", &itemTooltipDataModel_.tooltipLeft);
    constructor->Bind("tooltip_top", &itemTooltipDataModel_.tooltipTop);
    dataModelHandle_ = constructor->GetModelHandle();
}

void glimmer::ItemTooltipSystem::Update(float delta)
{
    if (itemToolTipComponent_ == nullptr || document_ == nullptr)
    {
        return;
    }
    const Item* item = itemToolTipComponent_->GetItem();
    if (item == nullptr)
    {
        document_->Hide();
        return;
    }
    float mouseX = 0.0F;
    float mouseY = 0.0F;
    SDL_GetMouseState(&mouseX, &mouseY);
    constexpr float offset = 0.0F;
    itemTooltipDataModel_.tooltipLeft = fmt::format("{}px", mouseX + offset);
    itemTooltipDataModel_.tooltipTop = fmt::format("{}px", mouseY + offset);
    dataModelHandle_.DirtyVariable("tooltip_left");
    dataModelHandle_.DirtyVariable("tooltip_top");
    if (currentItem_ != item)
    {
        OnItemChanged(item);
        currentItem_ = item;
        //Jump to the next frame for display to prevent position flickering.
        //跳转到下一帧显示，以避免出现位置闪烁。
        return;
    }
    document_->Show();
}
