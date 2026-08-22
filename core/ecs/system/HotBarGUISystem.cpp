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

#include "core/ecs/component/ItemToolTipComponent.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/inventory/ItemDurabilityModule.h"
#include "core/log/LogCat.h"
#include "core/mod/Resource.h"
#include "core/rmi/dataModel/ItemSlotDataModel.h"
#include "core/scene/Scene.h"
#include "core/utils/StringUtils.h"
#include "core/world/WorldContext.h"
#include "core/world/SystemScheduler.h"

glimmer::ItemSlotDataModel *glimmer::HotBarGUISystem::GetItemSlotDataModel(const uint8_t index) {
    const uint8_t size = itemSlots_.size();
    if (index >= size) {
        LogCat::w(std::source_location::current(), "itemSlotDataModel == nullptr");
        return nullptr;
    }
    return &itemSlots_[index];
}

glimmer::HotBarGUISystem::HotBarGUISystem(WorldContext *worldContext) : GuiGameSystem(worldContext) {
    itemSlots_.resize(HOT_BAR_SIZE);
    WatchComponent(COMPONENT_ITEM_TOOL_TIP);
    WatchComponent(COMPONENT_ITEM_CONTAINER);
    Init();
}

glimmer::HotBarGUISystem::~HotBarGUISystem() {
    if (itemContainer_ != nullptr && callback_ != nullptr) {
        itemContainer_->RemoveOnContentChanged(callback_);
    }
}

void glimmer::HotBarGUISystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) {
    if (gameComponentType == COMPONENT_ITEM_TOOL_TIP) {
        itemToolTipComponent_ = GetEntityShortCut()->GetItemToolTipComponent();
        return;
    }
    if (gameComponentType != COMPONENT_ITEM_CONTAINER) {
        return;
    }
    if (itemContainer_ != nullptr) {
        LogCat::i("itemContainer already set, skipping");
        return;
    }
    const EntityShortCut *entityShortCut = GetEntityShortCut();
    if (entityShortCut == nullptr) {
        LogCat::w(std::source_location::current(), "entityShortCut == nullptr");
        return;
    }

    const ItemContainerComponent *itemContainerComponent = entityShortCut->GetItemContainerComponent();
    if (itemContainerComponent == nullptr) {
        LogCat::w(std::source_location::current(), "itemContainerComponent == nullptr");
        return;
    }
    itemContainer_ = itemContainerComponent->GetItemContainer();
    if (itemContainer_ == nullptr) {
        LogCat::w(std::source_location::current(), "itemContainer == nullptr");
        return;
    }
    callback_ = itemContainer_->AddOnContentChanged(
        [this](const uint8_t index, const Item *item, const ContainerChangeType changeType) {
            const auto dataModel = GetItemSlotDataModel(index);
            if (dataModel == nullptr) {
                LogCat::w(std::source_location::current(), "dataModel == nullptr");
                return;
            }
            if (changeType == ContainerChangeType::STACK_DESTROY ||
                changeType == ContainerChangeType::STACK_AMOUNT_EXHAUSTED ||
                changeType == ContainerChangeType::STACK_DURABILITY_EXHAUSTED) {
                dataModel->image = "";
                dataModel->amount = 0;
                dataModel->durability = -1;
            } else {
                const auto stackModule = item->GetStackModule();
                auto amount = 0;
                if (stackModule != nullptr) {
                    amount = stackModule->GetAmount();
                }
                if (amount == 0) {
                    dataModel->image = "";
                    dataModel->amount = 0;
                    dataModel->durability = -1;
                } else {
                    const ResourceRef *iconResourceRef = item->GetIconResourceRef();
                    dataModel->image = StringUtils::MakeTextureUrl(
                        Resource::GenerateId(iconResourceRef->GetPackageId(), iconResourceRef->GetResourceKey()));
                    dataModel->amount = amount;
                    const ItemDurabilityModule *durabilityModule = item->GetDurabilityModule();
                    dataModel->durability = ItemSlotDataModel::CalculateDurabilityPercentage(
                        durabilityModule->GetMaxDurability(), durabilityModule->GetUsedDurability(),
                        durabilityModule->IsUnbreakable());
                }
            }
            if (constructor_ != nullptr) {
                constructor_->GetModelHandle().DirtyVariable("item_slots");
            }
        });
}

void glimmer::HotBarGUISystem::LoadDocuments(IDocumentRegistry *documentRegistry) {
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("hotbar/hotbar");
    SetElementDocument(documentRegistry->LoadSingleDocument(&resourceRef));
}

void glimmer::HotBarGUISystem::OnCreateDataModels(IDocumentRegistry *documentRegistry) {
    LogCat::i("OnCreateDataModels called");
    constructor_ = documentRegistry->CreateDataModel("hotbar");
    LogCat::i("DataModel 'hotbar' created");
    if (auto linkStruct = constructor_->RegisterStruct<ItemSlotDataModel>()) {
        linkStruct.RegisterMember("image", &ItemSlotDataModel::image);
        linkStruct.RegisterMember("amount", &ItemSlotDataModel::amount);
        linkStruct.RegisterMember("selected", &ItemSlotDataModel::selected);
        linkStruct.RegisterMember("index", &ItemSlotDataModel::index);
        linkStruct.RegisterMember("durability", &ItemSlotDataModel::durability);
        constructor_->RegisterArray<std::vector<ItemSlotDataModel> >();
        LogCat::i("Struct members registered: image, amount, selected");
    }
    constructor_->Bind("item_slots", &itemSlots_);
    LogCat::i("item_slots bound to data model");
    constructor_->BindEventCallback("on_item_hover", &HotBarGUISystem::OnItemHover, this);
    constructor_->BindEventCallback("on_item_out", &HotBarGUISystem::OnItemOut, this);
    LoadInitialHotbarItems();
}

void glimmer::HotBarGUISystem::LoadInitialHotbarItems() {
    if (itemContainer_ == nullptr) {
        LogCat::i("LoadInitialHotbarItems: itemContainer_ is nullptr");
        return;
    }
    LogCat::i("Loading initial hotbar items after data model created...");
    for (uint8_t i = 0; i < HOT_BAR_SIZE; ++i) {
        const Item *item = itemContainer_->GetItem(i);
        auto dataModel = GetItemSlotDataModel(i);
        if (dataModel == nullptr) {
            continue;
        }
        dataModel->selected = i == itemContainer_->GetSelectIndex();
        if (item == nullptr) {
            dataModel->image = "";
            dataModel->amount = 0;
            dataModel->durability = -1;
        } else {
            const ResourceRef *iconResourceRef = item->GetIconResourceRef();
            dataModel->image = StringUtils::MakeTextureUrl(Resource::GenerateId(iconResourceRef->GetPackageId(),
                iconResourceRef->GetResourceKey()));
            const ItemStackModule *stackModule = item->GetStackModule();
            dataModel->amount = stackModule != nullptr ? stackModule->GetAmount() : 1;
            const ItemDurabilityModule *durabilityModule = item->GetDurabilityModule();
            dataModel->durability = ItemSlotDataModel::CalculateDurabilityPercentage(
                durabilityModule->GetMaxDurability(), durabilityModule->GetUsedDurability(),
                durabilityModule->IsUnbreakable());
        }
    }
    if (constructor_ != nullptr) {
        constructor_->GetModelHandle().DirtyVariable("item_slots");
        LogCat::i("item_slots variable dirtied");
    }
}

void glimmer::HotBarGUISystem::UpdateSelectedSlot(const uint8_t beforeIndex, const uint8_t nextIndex) {
    auto beforeDataModel = GetItemSlotDataModel(beforeIndex);
    if (beforeDataModel != nullptr) {
        beforeDataModel->selected = false;
    }

    auto nextDataModel = GetItemSlotDataModel(nextIndex);
    if (nextDataModel != nullptr) {
        nextDataModel->selected = true;
    }
    itemContainer_->SetSelectIndex(nextIndex);
    if (constructor_ != nullptr) {
        constructor_->GetModelHandle().DirtyVariable("item_slots");
    }
}

bool glimmer::HotBarGUISystem::HandleEvent(const SDL_Event &event) {
    if (itemContainer_ == nullptr) {
        LogCat::w(std::source_location::current(), "itemContainer == nullptr");
        return false;
    }
    if (event.type == SDL_EVENT_MOUSE_WHEEL) {
        uint8_t beforeIndex = itemContainer_->GetSelectIndex();
        uint8_t nextIndex = beforeIndex;
        if (event.wheel.y > 0) {
            nextIndex = nextIndex == 0 ? HOT_BAR_SIZE - 1 : nextIndex - 1;
        } else if (event.wheel.y < 0) {
            nextIndex = nextIndex >= HOT_BAR_SIZE - 1 ? 0 : nextIndex + 1;
        }
        UpdateSelectedSlot(beforeIndex, nextIndex);
        return true;
    }

    if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
        if (event.key.scancode >= SDL_SCANCODE_1 && event.key.scancode <= SDL_SCANCODE_9) {
            const uint8_t beforeIndex = itemContainer_->GetSelectIndex();
            const uint8_t nextIndex = event.key.scancode - SDL_SCANCODE_1;
            UpdateSelectedSlot(beforeIndex, nextIndex);
            return true;
        }
    }

    return false;
}


glimmer::GameSystemType glimmer::HotBarGUISystem::GetGameSystemType() const {
    return GameSystemType::HotBarGUISystem;
}

void glimmer::HotBarGUISystem::OnItemHover(Rml::DataModelHandle handle, Rml::Event &event,
                                           const Rml::VariantList &args) {
    if (args.empty() || itemContainer_ == nullptr || itemToolTipComponent_ == nullptr) {
        return;
    }
    const int index = args[0].Get<int>();
    if (index < 0 || index >= static_cast<int>(itemContainer_->GetCapacity())) {
        return;
    }
    itemToolTipComponent_->SetItem(itemContainer_->GetItem(static_cast<uint8_t>(index)));
}

void glimmer::HotBarGUISystem::OnItemOut(Rml::DataModelHandle handle, Rml::Event &event,
                                         const Rml::VariantList &args) {
    if (itemToolTipComponent_ == nullptr) {
        return;
    }
    itemToolTipComponent_->ResetItem();
}
