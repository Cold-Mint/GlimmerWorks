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
#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "core/ecs/GuiStackGameSystem.h"
#include "core/rmi/dataModel/ItemSlotDataModel.h"

namespace Rml {
    class ElementFormControlInput;
}

namespace glimmer {
    class Item;
    class ItemContainer;
    class ItemToolTipComponent;
    class InventoryDragListener;
    enum class ContainerChangeType;

    /**
     * Storage GUI system
     * 存储GUI系统
     * Shows two inventories side by side: the player backpack and the currently
     * opened storage block (e.g. a chest), allowing the player to move items between them.
     * 并排显示两个库存：玩家背包与当前打开的存储方块（例如箱子），允许玩家在两者间移动物品。
     */
    class StorageGUISystem : public GuiStackGameSystem {
        std::vector<ItemSlotDataModel> playerSlots_;
        std::vector<ItemSlotDataModel> chestSlots_;
        std::vector<ItemSlotDataModel> lootSlots_;
        bool showLoot_ = false;
        Rml::DataModelConstructor *constructor_ = nullptr;
        Rml::ElementFormControlInput *searchInputElement_ = nullptr;
        ItemContainer *playerContainer_ = nullptr;
        ItemContainer *openedContainer_ = nullptr;
        //The materialized loot container of the currently opened loot box (owned by this system).
        //当前打开的战利品箱的物化战利品容器（由本系统持有）。
        std::unique_ptr<ItemContainer> lootContainer_;
        std::string lootBoxUuid_;
        ItemToolTipComponent *itemToolTipComponent_ = nullptr;
        std::shared_ptr<std::function<void(uint8_t, Item *, ContainerChangeType)> > playerCallback_;
        std::shared_ptr<std::function<void(uint8_t, Item *, ContainerChangeType)> > chestCallback_;
        std::shared_ptr<std::function<void(uint8_t, Item *, ContainerChangeType)> > lootCallback_;
        std::unique_ptr<InventoryDragListener> dragListener_;

        static void MoveStack(ItemContainer *from, uint8_t index, ItemContainer *to);

        void RefreshSlots(const ItemContainer *container, std::vector<ItemSlotDataModel> &slots) const;

        void RefreshPlayerSlots();

        void RefreshChestSlots();

        void RefreshLootSlots();

        void BindCallbacks();

        void UnbindCallbacks();

        void SetupDragAndDrop();

        void DepositAll();

        void QuickStack();

        void LootAll();

        void TakeLootAll();

        void Restock();

        void ApplyFilter();

        void OnBackClick(Rml::DataModelHandle handle, Rml::Event &event, const Rml::VariantList &args);

        void OnDepositAllClick(Rml::DataModelHandle handle, Rml::Event &event, const Rml::VariantList &args);

        void OnQuickStackClick(Rml::DataModelHandle handle, Rml::Event &event, const Rml::VariantList &args);

        void OnLootAllClick(Rml::DataModelHandle handle, Rml::Event &event, const Rml::VariantList &args);

        void OnLootTakeAllClick(Rml::DataModelHandle handle, Rml::Event &event, const Rml::VariantList &args);

        void OnRestockClick(Rml::DataModelHandle handle, Rml::Event &event, const Rml::VariantList &args);

        void OnSortPlayerClick(Rml::DataModelHandle handle, Rml::Event &event, const Rml::VariantList &args);

        void OnSortChestClick(Rml::DataModelHandle handle, Rml::Event &event, const Rml::VariantList &args);

        void OnPlayerItemHover(Rml::DataModelHandle handle, Rml::Event &event, const Rml::VariantList &args);

        void OnChestItemHover(Rml::DataModelHandle handle, Rml::Event &event, const Rml::VariantList &args);

        void OnLootItemHover(Rml::DataModelHandle handle, Rml::Event &event, const Rml::VariantList &args);

        void OnItemOut(Rml::DataModelHandle handle, Rml::Event &event, const Rml::VariantList &args);

    public:
        ~StorageGUISystem() override;

        explicit StorageGUISystem(WorldContext *worldContext);

        void OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) override;

        void OnCreateDataModels(IDocumentRegistry *documentRegistry) override;

        void OnActivationChanged(bool activeStatus) override;

        [[nodiscard]] GameSystemType GetGameSystemType() const override;

        void LoadDocuments(IDocumentRegistry *documentRegistry) override;
    };
}
