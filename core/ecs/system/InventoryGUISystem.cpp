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

#include "RecipeDetailGUISystem.h"
#include "core/context/AppContext.h"
#include "core/context/ModContext.h"
#include "core/ecs/EntityManager.h"
#include "core/ecs/component/ItemToolTipComponent.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/log/LogCat.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/dataPack/RecipeManager.h"
#include "core/rmi/InventoryDragListener.h"
#include "core/utils/StringUtils.h"
#include "core/world/SystemScheduler.h"
#include "core/world/WorldContext.h"

glimmer::ItemSlotDataModel *glimmer::InventoryGUISystem::GetItemSlotDataModel(uint8_t index) {
    const uint8_t size = itemSlots_.size();
    if (index >= size) {
        LogCat::w(std::source_location::current(), "itemSlotDataModel == nullptr");
        return nullptr;
    }
    return &itemSlots_[index];
}

void glimmer::InventoryGUISystem::LoadInitialItems() {
    if (itemContainer_ == nullptr) {
        LogCat::i("LoadInitialHotbarItems: itemContainer_ is nullptr");
        return;
    }
    LogCat::i("Loading initial hotbar items after data model created...");
    uint8_t capacity = itemContainer_->GetCapacity();
    for (uint8_t i = 0; i < capacity; ++i) {
        const Item *item = itemContainer_->GetItem(i);
        auto dataModel = GetItemSlotDataModel(i);
        if (dataModel == nullptr) {
            continue;
        }
        dataModel->selected = i == itemContainer_->GetSelectIndex();
        if (item == nullptr) {
            dataModel->image = "";
            dataModel->amount = 0;
        } else {
            const ResourceRef *iconResourceRef = item->GetIconResourceRef();
            dataModel->image = StringUtils::MakeTextureUrl(Resource::GenerateId(iconResourceRef->GetPackageId(),
                iconResourceRef->GetResourceKey()));
            const ItemStackModule *stackModule = item->GetStackModule();
            dataModel->amount = stackModule != nullptr ? stackModule->GetAmount() : 1;
        }
    }
    if (constructor_ != nullptr) {
        constructor_->GetModelHandle().DirtyVariable("item_slots");
        LogCat::i("item_slots variable dirtied");
    }
}

void glimmer::InventoryGUISystem::RefreshRecipeList() {
    recipeSlots_.clear();
    recipeOutputItems_.clear();
    WorldContext *worldContext = GetWorldContext();
    if (worldContext == nullptr) {
        return;
    }
    const AppContext *appContext = worldContext->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    const ModContext *modContext = appContext->GetModContext();
    if (modContext == nullptr) {
        return;
    }
    const RecipeManager *recipeManager = modContext->GetRecipeManager();
    if (recipeManager == nullptr) {
        return;
    }
    ResourceLocator *resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator == nullptr) {
        return;
    }
    const EntityShortCut *entityShortCut = GetEntityShortCut();
    if (entityShortCut == nullptr) {
        return;
    }
    EntityManager *entityManager = worldContext->GetEntityManager();
    if (entityManager == nullptr) {
        return;
    }
    const GameEntityID playerEntity = entityShortCut->GetPlayer();
    if (WorldContext::IsEmptyEntityId(playerEntity)) {
        return;
    }
    auto *playerComponent = entityManager->GetComponent<PlayerComponent>(playerEntity);
    if (playerComponent == nullptr) {
        return;
    }
    const PlayerTechnologyHandler *techHandler = playerComponent->GetTechnologyHandler();
    if (techHandler == nullptr) {
        return;
    }
    if (itemContainer_ == nullptr) {
        return;
    }
    unlockedRecipes_ = recipeManager->FindUnlockedRecipes(techHandler->GetTechnologyMap(),
                                                          itemContainer_->GetTotalTags());
    recipeOutputItems_.resize(unlockedRecipes_.size());
    recipeSlots_.reserve(unlockedRecipes_.size());
    const uint32_t maxSize = unlockedRecipes_.size();
    for (uint32_t i = 0; i < maxSize; ++i) {
        const auto *recipe = unlockedRecipes_[i];
        if (recipe == nullptr) {
            continue;
        }
        std::unique_ptr<Item> item = resourceLocator->FindItem(worldContext, recipe->output);
        recipeOutputItems_[i] = std::move(item); //Process finished with exit code 134 (interrupted by signal 6:SIGABRT)
        const Item *itemPtr = recipeOutputItems_[i].get();
        ItemSlotDataModel slot;
        slot.selected = false;
        slot.index = static_cast<int>(i);
        if (itemPtr != nullptr) {
            const ResourceRef *iconRef = itemPtr->GetIconResourceRef();
            if (iconRef != nullptr) {
                slot.image = StringUtils::MakeTextureUrl(
                    Resource::GenerateId(iconRef->GetPackageId(), iconRef->GetResourceKey()));
            } else {
                slot.image = "";
            }
        } else {
            slot.image = "";
        }
        slot.amount = static_cast<int>(recipe->output.amount);
        recipeSlots_.push_back(std::move(slot));
    }
    if (constructor_ != nullptr) {
        constructor_->GetModelHandle().DirtyVariable("recipe_slots");
    }
}

glimmer::InventoryGUISystem::~InventoryGUISystem() {
    if (dragListener_ != nullptr) {
        dragListener_->UnregisterContainer();
        dragListener_.reset();
    }
    if (itemContainer_ != nullptr && callback_ != nullptr) {
        itemContainer_->RemoveOnContentChanged(callback_);
    }
}

void glimmer::InventoryGUISystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType,
                                                            uint32_t count) {
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
    itemSlots_.resize(itemContainer_->GetCapacity());
    callback_ = itemContainer_->AddOnContentChanged(
        [this](const uint8_t index, const Item *item, const ContainerChangeType changeType) {
            const auto dataModel = GetItemSlotDataModel(index);
            if (dataModel == nullptr) {
                LogCat::w(std::source_location::current(), "dataModel == nullptr");
                return;
            }
            if (changeType == ContainerChangeType::STACK_DESTROY) {
                dataModel->image = "";
                dataModel->amount = 0;
            } else {
                const auto stackModule = item->GetStackModule();
                auto amount = 0;
                if (stackModule != nullptr) {
                    amount = stackModule->GetAmount();
                }
                if (amount == 0) {
                    dataModel->image = "";
                    dataModel->amount = 0;
                } else {
                    const ResourceRef *iconResourceRef = item->GetIconResourceRef();
                    dataModel->image = StringUtils::MakeTextureUrl(
                        Resource::GenerateId(iconResourceRef->GetPackageId(), iconResourceRef->GetResourceKey()));
                    dataModel->amount = amount;
                }
            }
            if (constructor_ != nullptr) {
                constructor_->GetModelHandle().DirtyVariable("item_slots");
            }
            RefreshRecipeList();
        });
}

glimmer::InventoryGUISystem::InventoryGUISystem(WorldContext *worldContext)
    : GuiStackGameSystem(worldContext) {
    WatchComponent(COMPONENT_ITEM_CONTAINER);
    WatchComponent(COMPONENT_ITEM_TOOL_TIP);
    Init();
}

void glimmer::InventoryGUISystem::OnCreateDataModels(IDocumentRegistry *documentRegistry) {
    constructor_ = documentRegistry->CreateDataModel("inventory");
    if (auto linkStruct = constructor_->RegisterStruct<ItemSlotDataModel>()) {
        linkStruct.RegisterMember("image", &ItemSlotDataModel::image);
        linkStruct.RegisterMember("amount", &ItemSlotDataModel::amount);
        linkStruct.RegisterMember("selected", &ItemSlotDataModel::selected);
        linkStruct.RegisterMember("index", &ItemSlotDataModel::index);
        constructor_->RegisterArray<std::vector<ItemSlotDataModel> >();
    }
    constructor_->Bind("item_slots", &itemSlots_);
    constructor_->Bind("recipe_slots", &recipeSlots_);
    constructor_->BindEventCallback("on_recipe_click", &InventoryGUISystem::OnRecipeClick, this);
    constructor_->BindEventCallback("on_item_hover", &InventoryGUISystem::OnItemHover, this);
    constructor_->BindEventCallback("on_item_out", &InventoryGUISystem::OnItemOut, this);
    constructor_->BindEventCallback("on_recipe_hover", &InventoryGUISystem::OnRecipeHover, this);
    constructor_->BindEventCallback("on_recipe_out", &InventoryGUISystem::OnItemOut, this);
    LoadInitialItems();
}

void glimmer::InventoryGUISystem::SetupDragAndDrop() {
    if (dragListener_ != nullptr) {
        LogCat::e(std::source_location::current(), "dragListener_ == nullptr");
        return;
    }
    if (itemContainer_ == nullptr) {
        LogCat::e(std::source_location::current(), "itemContainer_ is nullptr, cannot setup drag and drop");
        return;
    }

    Rml::ElementDocument* document = GetElementDocument();
    if (document == nullptr) {
        LogCat::e(std::source_location::current(), "document is nullptr, cannot setup drag and drop");
        return;
    }

    Rml::Element* container = document->GetElementById("item_container");
    if (container == nullptr) {
        container = document->GetFirstChild();
        while (container != nullptr && !container->IsClassSet("item_container")) {
            container = container->GetNextSibling();
        }
    }

    if (container == nullptr) {
        LogCat::w(std::source_location::current(), "item_container element not found in document");
        return;
    }

    dragListener_ = std::make_unique<InventoryDragListener>(itemContainer_);
    dragListener_->RegisterContainer(container);
    LogCat::i("Inventory drag and drop setup complete");
}

void glimmer::InventoryGUISystem::OnActivationChanged(bool activeStatus) {
    GuiStackGameSystem::OnActivationChanged(activeStatus);
    if (activeStatus) {
        RefreshRecipeList();
    }
}

SDL_Scancode glimmer::InventoryGUISystem::GetHotKey() const {
    return SDL_SCANCODE_E;
}

glimmer::GameSystemType glimmer::InventoryGUISystem::GetGameSystemType() const {
    return GameSystemType::InventoryGUISystem;
}

void glimmer::InventoryGUISystem::LoadDocuments(IDocumentRegistry *documentRegistry) {
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("inventory/inventory");
    SetAndHideElementDocument(documentRegistry->LoadSingleDocument(&resourceRef));
    SetupDragAndDrop();
}

void glimmer::InventoryGUISystem::OnRecipeClick(Rml::DataModelHandle handle, Rml::Event &event,
                                                const Rml::VariantList &args) {
    if (args.empty()) {
        return;
    }
    int index = args[0].Get<int>();
    if (index < 0 || index >= static_cast<int>(unlockedRecipes_.size())) {
        LogCat::w(std::source_location::current(), "invalid recipe index: {}", index);
        return;
    }
    WorldContext *worldContext = GetWorldContext();
    if (worldContext == nullptr) {
        return;
    }
    SystemScheduler *scheduler = worldContext->GetSystemScheduler();
    if (scheduler == nullptr) {
        return;
    }
    EntityShortCut *entityShortCut = worldContext->GetEntityShortCut();
    if (entityShortCut == nullptr) {
        return;
    }
    RecipeSelectionComponent *recipeSelectionComponent = entityShortCut->GetRecipeSelectionComponent();
    if (recipeSelectionComponent == nullptr) {
        return;
    }
    recipeSelectionComponent->SetRecipeResource(unlockedRecipes_[index]);
    scheduler->PushGuiSystemType(GameSystemType::RecipeDetailGUISystem);
}

void glimmer::InventoryGUISystem::OnItemHover(Rml::DataModelHandle handle, Rml::Event &event,
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

void glimmer::InventoryGUISystem::OnItemOut(Rml::DataModelHandle handle, Rml::Event &event,
                                            const Rml::VariantList &args) {
    if (itemToolTipComponent_ == nullptr) {
        return;
    }
    itemToolTipComponent_->ResetItem();
}

void glimmer::InventoryGUISystem::OnRecipeHover(Rml::DataModelHandle handle, Rml::Event &event,
                                                const Rml::VariantList &args) {
    if (args.empty() || itemToolTipComponent_ == nullptr) {
        return;
    }
    const int index = args[0].Get<int>();
    if (index < 0 || index >= static_cast<int>(recipeOutputItems_.size())) {
        return;
    }
    if (recipeOutputItems_[index] == nullptr) {
        return;
    }
    itemToolTipComponent_->SetItem(recipeOutputItems_[index].get());
}
