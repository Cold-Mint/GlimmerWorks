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

#include <memory>

#include "core/mod/dataPack/BiomesManager.h"
#include "core/mod/dataPack/ItemManager.h"
#include "core/mod/dataPack/TileResourceManager.h"
#include "core/mod/dataPack/BiomeDecoratorResourcesManager.h"
#include "core/lootTable/LootTableManager.h"
#include "core/mod/dataPack/MobManager.h"
#include "core/mod/dataPack/RecipeManager.h"
#include "core/mod/dataPack/StructureManager.h"
#include "core/mod/dataPack/StringManager.h"
#include "core/mod/dataPack/DataPackManager.h"
#include "core/mod/TomlTemplateExpander.h"
#include "core/mod/StructurePlacementConditionsManager.h"
#include "core/contributor/ContributorManager.h"
#include "core/inventory/InitialInventoryManager.h"
#include "core/world/generator/BiomeDecoratorManager.h"
#include "core/world/structure/StructureGeneratorManager.h"
#include "core/shape/ShapeManager.h"

namespace glimmer
{
    class VirtualFileSystem;
    class ResourceLocator;
    class LangsResources;

    class ModContext
    {
        std::unique_ptr<DataPackManager> dataPackManager_;
        std::unique_ptr<StringManager> stringManager_;
        std::unique_ptr<TileResourceManager> tileResourceManager_;
        std::unique_ptr<BiomesManager> biomesManager_;
        std::unique_ptr<BiomeDecoratorManager> biomeDecoratorManager_;
        std::unique_ptr<BiomeDecoratorResourcesManager> biomeDecoratorResourcesManager_;
        std::unique_ptr<ItemManager> itemManager_;
        std::unique_ptr<RecipeManager> recipeManager_;
        std::unique_ptr<MobManager> mobManager_;
        std::unique_ptr<StructureManager> structureManager_;
        std::unique_ptr<StructureGeneratorManager> structureGeneratorManager_;
        std::unique_ptr<StructurePlacementConditionsManager> structurePlacementConditionsManager_;
        std::unique_ptr<LootTableManager> lootTableManager_;
        std::unique_ptr<InitialInventoryManager> initialInventoryManager_;
        std::unique_ptr<ContributorManager> contributorManager_;
        std::unique_ptr<TomlTemplateExpander> tomlTemplateExpander_;
        std::unique_ptr<ShapeManager> shapeManager_;

    public:
        ModContext();
        ~ModContext();

        void Init(VirtualFileSystem* vfs, ResourceLocator* resourceLocator, LangsResources* langsResources,
                  const toml::spec& tomlVersion);

        [[nodiscard]] DataPackManager* GetDataPackManager() const;
        [[nodiscard]] StringManager* GetStringManager() const;
        [[nodiscard]] TileResourceManager* GetTileResourceManager() const;
        [[nodiscard]] BiomesManager* GetBiomesManager() const;
        [[nodiscard]] BiomeDecoratorManager* GetBiomeDecoratorManager() const;
        [[nodiscard]] BiomeDecoratorResourcesManager* GetBiomeDecoratorResourcesManager() const;
        [[nodiscard]] ItemManager* GetItemManager() const;
        [[nodiscard]] RecipeManager* GetRecipeManager() const;
        [[nodiscard]] MobManager* GetMobManager() const;
        [[nodiscard]] StructureManager* GetStructureManager() const;
        [[nodiscard]] StructureGeneratorManager* GetStructureGeneratorManager() const;
        [[nodiscard]] StructurePlacementConditionsManager* GetStructurePlacementConditionsManager() const;
        [[nodiscard]] LootTableManager* GetLootTableManager() const;
        [[nodiscard]] InitialInventoryManager* GetInitialInventoryManager() const;
        [[nodiscard]] ContributorManager* GetContributorManager() const;
        [[nodiscard]] TomlTemplateExpander* GetTomlTemplateExpander() const;
        [[nodiscard]] ShapeManager* GetShapeManager() const;
    };
}
