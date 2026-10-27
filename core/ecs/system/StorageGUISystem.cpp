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
#include "StorageGUISystem.h"

#include <algorithm>

#include "RmlUi/Core/Elements/ElementFormControlInput.h"
#include "core/ecs/EntityShortCut.h"
#include "core/ecs/component/ItemContainerComponent.h"
#include "core/inventory/Item.h"
#include "core/inventory/ItemContainer.h"
#include "core/inventory/ItemDurabilityModule.h"
#include "core/log/LogCat.h"
#include "core/rmi/InventoryDragListener.h"
#include "core/utils/StringUtils.h"
#include "core/world/SystemScheduler.h"
#include "core/world/WorldContext.h"


void glimmer::StorageGUISystem::MoveStack(ItemContainer *from, const uint8_t index, ItemContainer *to) {
    if (from == nullptr || to == nullptr) {
        return;
    }
    std::unique_ptr<Item> item = from->TakeAllItem(index);
    if (item == nullptr) {
        return;
    }
    std::unique_ptr<Item> leftover = to->AddItem(std::move(item));
    if (leftover != nullptr) {
        (void) from->AddItem(std::move(leftover));
    }
}

void glimmer::StorageGUISystem::RefreshSlots(const ItemContainer *container, std::vector<ItemSlotDataModel> &slots) const {
    slots.clear();
    if (container == nullptr) {
        return;
    }
    const uint8_t capacity = container->GetCapacity();
    slots.reserve(capacity);
    for (uint8_t i = 0; i < capacity; ++i) {
        const Item *item = container->GetItem(i);
        ItemSlotDataModel slot;
        slot.index = static_cast<int>(i);
        slot.selected = false;
        if (item == nullptr) {
            //Skip empty slots while filtering so the grid shows a compacted result.
            //过滤时跳过空格，使网格显示紧凑结果。
            if (!filter_.empty()) {
                continue;
            }
            slot.image = "";
            slot.amount = 0;
            slot.durability = -1;
            slots.push_back(std::move(slot));
            continue;
        }
        if (!filter_.empty() && !StringUtils::ContainsIgnoreCase(item->GetName(), filter_)) {
            continue;
        }
        const ResourceRef *iconRef = item->GetIconResourceRef();
        if (iconRef != nullptr && iconRef->IsValid()) {
            slot.image = StringUtils::MakeTextureUrl(
                Resource::GenerateId(iconRef->GetPackageId(), iconRef->GetResourceKey()));
        } else {
            slot.image = "";
        }
        const ItemStackModule *stackModule = item->GetStackModule();
        slot.amount = stackModule != nullptr ? stackModule->GetAmount() : 1;
        const ItemDurabilityModule *durabilityModule = item->GetDurabilityModule();
        slot.durability = ItemSlotDataModel::CalculateDurabilityPercentage(
            durabilityModule->GetMaxDurability(), durabilityModule->GetUsedDurability(),
            durabilityModule->IsUnbreakable());
        slots.push_back(std::move(slot));
    }
}

void glimmer::StorageGUISystem::RefreshPlayerSlots() {
    RefreshSlots(playerContainer_, playerSlots_);
}

void glimmer::StorageGUISystem::RefreshChestSlots() {
    RefreshSlots(openedContainer_, chestSlots_);
}

void glimmer::StorageGUISystem::BindCallbacks() {
    UnbindCallbacks();
    if (playerContainer_ != nullptr) {
        playerCallback_ = playerContainer_->AddOnContentChanged(
            [this](const uint8_t, Item *, const ContainerChangeType) {
                RefreshPlayerSlots();
                if (constructor_ != nullptr) {
                    constructor_->GetModelHandle().DirtyVariable("player_slots");
                }
            });
    }
    if (openedContainer_ != nullptr) {
        chestCallback_ = openedContainer_->AddOnContentChanged(
            [this](const uint8_t, Item *, const ContainerChangeType) {
                RefreshChestSlots();
                if (constructor_ != nullptr) {
                    constructor_->GetModelHandle().DirtyVariable("chest_slots");
                }
            });
    }
}

void glimmer::StorageGUISystem::UnbindCallbacks() {
    if (playerContainer_ != nullptr && playerCallback_ != nullptr) {
        playerContainer_->RemoveOnContentChanged(playerCallback_);
    }
    if (openedContainer_ != nullptr && chestCallback_ != nullptr) {
        openedContainer_->RemoveOnContentChanged(chestCallback_);
    }
    playerCallback_.reset();
    chestCallback_.reset();
}

void glimmer::StorageGUISystem::SetupDragAndDrop() {
    Rml::ElementDocument *document = GetElementDocument();
    if (document == nullptr) {
        return;
    }
    Rml::Element *playerContainerElement = document->GetElementById("player_container");
    Rml::Element *chestContainerElement = document->GetElementById("chest_container");
    if (playerContainerElement != nullptr && playerContainer_ != nullptr) {
        dragListener_->RegisterContainer(playerContainerElement, playerContainer_);
    }
    if (chestContainerElement != nullptr && openedContainer_ != nullptr) {
        dragListener_->RegisterContainer(chestContainerElement, openedContainer_);
    }
}

void glimmer::StorageGUISystem::DepositAll() {
    if (playerContainer_ == nullptr || openedContainer_ == nullptr) {
        return;
    }
    for (uint8_t i = 0; i < playerContainer_->GetCapacity(); ++i) {
        if (playerContainer_->GetItem(i) == nullptr) {
            continue;
        }
        MoveStack(playerContainer_, i, openedContainer_);
    }
}

void glimmer::StorageGUISystem::QuickStack() {
    if (playerContainer_ == nullptr || openedContainer_ == nullptr) {
        return;
    }
    for (uint8_t i = 0; i < playerContainer_->GetCapacity(); ++i) {
        const Item *item = playerContainer_->GetItem(i);
        if (item == nullptr) {
            continue;
        }
        if (!openedContainer_->ContainsId(item->GetId())) {
            continue;
        }
        MoveStack(playerContainer_, i, openedContainer_);
    }
}

void glimmer::StorageGUISystem::LootAll() {
    if (playerContainer_ == nullptr || openedContainer_ == nullptr) {
        return;
    }
    for (uint8_t i = 0; i < openedContainer_->GetCapacity(); ++i) {
        if (openedContainer_->GetItem(i) == nullptr) {
            continue;
        }
        MoveStack(openedContainer_, i, playerContainer_);
    }
}

void glimmer::StorageGUISystem::Restock() {
    if (playerContainer_ == nullptr || openedContainer_ == nullptr) {
        return;
    }
    const uint16_t maxStack = playerContainer_->GetMaxStackPerSlot();
    for (uint8_t pi = 0; pi < playerContainer_->GetCapacity(); ++pi) {
        const Item *playerItem = playerContainer_->GetItem(pi);
        if (playerItem == nullptr) {
            continue;
        }
        const ItemStackModule *playerStack = playerItem->GetStackModule();
        if (playerStack == nullptr || !playerStack->IsStackable()) {
            continue;
        }
        const uint16_t amount = playerStack->GetAmount();
        if (amount >= maxStack) {
            continue;
        }
        uint16_t space = static_cast<uint16_t>(maxStack - amount);
        for (uint8_t ci = 0; ci < openedContainer_->GetCapacity() && space > 0; ++ci) {
            const Item *chestItem = openedContainer_->GetItem(ci);
            if (chestItem == nullptr || chestItem->GetId() != playerItem->GetId()) {
                continue;
            }
            const ItemStackModule *chestStack = chestItem->GetStackModule();
            const uint16_t take = std::min(space, chestStack->GetAmount());
            if (take == 0) {
                continue;
            }
            std::unique_ptr<Item> taken = openedContainer_->TakeItem(ci, take);
            if (taken == nullptr) {
                continue;
            }
            std::unique_ptr<Item> leftover = playerContainer_->AddItem(std::move(taken));
            if (leftover != nullptr) {
                (void) openedContainer_->AddItem(std::move(leftover));
            }
            space = static_cast<uint16_t>(space - take);
        }
    }
}

void glimmer::StorageGUISystem::ApplyFilter() {
    RefreshPlayerSlots();
    RefreshChestSlots();
    if (constructor_ != nullptr) {
        constructor_->GetModelHandle().DirtyVariable("player_slots");
        constructor_->GetModelHandle().DirtyVariable("chest_slots");
    }
}

glimmer::StorageGUISystem::~StorageGUISystem() {
    UnbindCallbacks();
    if (dragListener_ != nullptr) {
        dragListener_->UnregisterContainer();
        dragListener_.reset();
    }
}

glimmer::StorageGUISystem::StorageGUISystem(WorldContext *worldContext)
    : GuiStackGameSystem(worldContext) {
    WatchComponent(COMPONENT_ITEM_CONTAINER);
    dragListener_ = std::make_unique<InventoryDragListener>(nullptr);
    Init();
}

void glimmer::StorageGUISystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType,
                                                          uint32_t count) {
    if (gameComponentType != COMPONENT_ITEM_CONTAINER) {
        return;
    }
    const EntityShortCut *entityShortCut = GetEntityShortCut();
    if (entityShortCut == nullptr) {
        return;
    }
    const ItemContainerComponent *itemContainerComponent = entityShortCut->GetItemContainerComponent();
    if (itemContainerComponent == nullptr) {
        return;
    }
    playerContainer_ = itemContainerComponent->GetItemContainer();
}

void glimmer::StorageGUISystem::OnCreateDataModels(IDocumentRegistry *documentRegistry) {
    constructor_ = documentRegistry->CreateDataModel("storage");
    if (constructor_ == nullptr) {
        return;
    }
    if (auto slotStruct = constructor_->RegisterStruct<ItemSlotDataModel>()) {
        slotStruct.RegisterMember("image", &ItemSlotDataModel::image);
        slotStruct.RegisterMember("amount", &ItemSlotDataModel::amount);
        slotStruct.RegisterMember("selected", &ItemSlotDataModel::selected);
        slotStruct.RegisterMember("index", &ItemSlotDataModel::index);
        slotStruct.RegisterMember("durability", &ItemSlotDataModel::durability);
        constructor_->RegisterArray<std::vector<ItemSlotDataModel> >();
    }
    constructor_->Bind("player_slots", &playerSlots_);
    constructor_->Bind("chest_slots", &chestSlots_);
    constructor_->BindEventCallback("on_back_click", &StorageGUISystem::OnBackClick, this);
    constructor_->BindEventCallback("on_deposit_all_click", &StorageGUISystem::OnDepositAllClick, this);
    constructor_->BindEventCallback("on_quick_stack_click", &StorageGUISystem::OnQuickStackClick, this);
    constructor_->BindEventCallback("on_loot_all_click", &StorageGUISystem::OnLootAllClick, this);
    constructor_->BindEventCallback("on_restock_click", &StorageGUISystem::OnRestockClick, this);
    constructor_->BindEventCallback("on_sort_player_click", &StorageGUISystem::OnSortPlayerClick, this);
    constructor_->BindEventCallback("on_sort_chest_click", &StorageGUISystem::OnSortChestClick, this);
    constructor_->BindEventCallback("on_filter_change", &StorageGUISystem::OnFilterChange, this);
}

void glimmer::StorageGUISystem::OnActivationChanged(bool activeStatus) {
    GuiStackGameSystem::OnActivationChanged(activeStatus);
    if (activeStatus) {
        const EntityShortCut *entityShortCut = GetEntityShortCut();
        if (entityShortCut == nullptr) {
            return;
        }
        const ItemContainerComponent *itemContainerComponent = entityShortCut->GetItemContainerComponent();
        if (itemContainerComponent != nullptr) {
            playerContainer_ = itemContainerComponent->GetItemContainer();
        }
        openedContainer_ = entityShortCut->GetOpenedStorageContainer();
        if (openedContainer_ == nullptr) {
            LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "opened_storage_container_null",
                      "StorageGUISystem: opened storage container is null");
            return;
        }
        filter_.clear();
        if (searchInputElement_ != nullptr) {
            searchInputElement_->SetValue("");
        }
        RefreshPlayerSlots();
        RefreshChestSlots();
        BindCallbacks();
        SetupDragAndDrop();
        if (constructor_ != nullptr) {
            constructor_->GetModelHandle().DirtyVariable("player_slots");
            constructor_->GetModelHandle().DirtyVariable("chest_slots");
        }
    } else {
        UnbindCallbacks();
        if (dragListener_ != nullptr) {
            dragListener_->UnregisterContainer();
        }
        openedContainer_ = nullptr;
        playerContainer_ = nullptr;
    }
}

SDL_Scancode glimmer::StorageGUISystem::GetHotKey() const {
    return SDL_SCANCODE_UNKNOWN;
}

glimmer::GameSystemType glimmer::StorageGUISystem::GetGameSystemType() const {
    return GameSystemType::StorageGUISystem;
}

void glimmer::StorageGUISystem::LoadDocuments(IDocumentRegistry *documentRegistry) {
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("storage/storage");
    SetAndHideElementDocument(documentRegistry->LoadSingleDocument(&resourceRef));
    Rml::ElementDocument *document = GetElementDocument();
    if (document != nullptr) {
        if (Rml::Element *searchInput = document->GetElementById("search_input"); searchInput != nullptr) {
            searchInputElement_ = rmlui_dynamic_cast<Rml::ElementFormControlInput *>(searchInput);
        }
    }
}

void glimmer::StorageGUISystem::OnBackClick(Rml::DataModelHandle handle, Rml::Event &event,
                                            const Rml::VariantList &args) {
    SystemScheduler *scheduler = GetWorldContext()->GetSystemScheduler();
    if (scheduler != nullptr) {
        scheduler->PopGuiSystemType();
    }
}

void glimmer::StorageGUISystem::OnDepositAllClick(Rml::DataModelHandle handle, Rml::Event &event,
                                                  const Rml::VariantList &args) {
    DepositAll();
}

void glimmer::StorageGUISystem::OnQuickStackClick(Rml::DataModelHandle handle, Rml::Event &event,
                                                  const Rml::VariantList &args) {
    QuickStack();
}

void glimmer::StorageGUISystem::OnLootAllClick(Rml::DataModelHandle handle, Rml::Event &event,
                                               const Rml::VariantList &args) {
    LootAll();
}

void glimmer::StorageGUISystem::OnRestockClick(Rml::DataModelHandle handle, Rml::Event &event,
                                               const Rml::VariantList &args) {
    Restock();
}

void glimmer::StorageGUISystem::OnSortPlayerClick(Rml::DataModelHandle handle, Rml::Event &event,
                                                  const Rml::VariantList &args) {
    if (playerContainer_ != nullptr) {
        playerContainer_->Sort();
    }
}

void glimmer::StorageGUISystem::OnSortChestClick(Rml::DataModelHandle handle, Rml::Event &event,
                                                 const Rml::VariantList &args) {
    if (openedContainer_ != nullptr) {
        openedContainer_->Sort();
    }
}

void glimmer::StorageGUISystem::OnFilterChange(Rml::DataModelHandle handle, Rml::Event &event,
                                               const Rml::VariantList &args) {
    if (searchInputElement_ == nullptr) {
        return;
    }
    filter_ = searchInputElement_->GetAttribute<Rml::String>("value", "");
    ApplyFilter();
}
