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
#include "ModContext.h"

#include "core/log/LogCat.h"
#include "core/mod/templateCommand/InsertTemplateCommand.h"
#include "core/mod/templateCommand/SetTemplateCommand.h"
#include "core/mod/templateCommand/UnSetTemplateCommand.h"
#include "core/world/generator/FillBiomeDecorator.h"
#include "core/world/generator/MineralBiomeDecorator.h"
#include "core/world/generator/SurfaceBiomeDecorator.h"
#include "core/world/structure/BiomeStructureConditionProcessor.h"
#include "core/world/structure/HeightStructureConditionProcessor.h"
#include "core/world/structure/HorizontalSpacingStructureConditionProcessor.h"
#include "core/world/structure/StaticStructureGenerator.h"
#include "core/world/structure/TreeStructureGenerator.h"
#include "core/world/structure/SurfaceStructureConditionProcessor.h"
#include "core/world/PreloadColors.h"
#include "core/mod/ResourceLocator.h"

namespace glimmer
{
    ModContext::ModContext() = default;

    ModContext::~ModContext() = default;

    void ModContext::Init(VirtualFileSystem* vfs, ResourceLocator* resourceLocator, LangsResources* langsResources,
                          const toml::spec& tomlVersion)
    {
        contributorManager_ = std::make_unique<ContributorManager>();
        recipeManager_ = std::make_unique<RecipeManager>();
        mobManager_ = std::make_unique<MobManager>();
        tomlTemplateExpander_ = std::make_unique<TomlTemplateExpander>();
        tomlTemplateExpander_->Register(std::make_unique<InsertTemplateCommand>());
        tomlTemplateExpander_->Register(std::make_unique<SetTemplateCommand>());
        tomlTemplateExpander_->Register(std::make_unique<UnSetTemplateCommand>());
        dataPackManager_ = std::make_unique<DataPackManager>(vfs, tomlTemplateExpander_.get());
        stringManager_ = std::make_unique<StringManager>();
        stringManager_->LoadLangsString(langsResources);
        biomesManager_ = std::make_unique<BiomesManager>();
        tileResourceManager_ = std::make_unique<TileResourceManager>();
        structurePlacementConditionsManager_ = std::make_unique<StructurePlacementConditionsManager>();
        structurePlacementConditionsManager_->AddConditionProcessor(std::make_unique<SurfaceStructureConditionProcessor>());
        structurePlacementConditionsManager_->AddConditionProcessor(std::make_unique<HeightStructureConditionProcessor>());
        structurePlacementConditionsManager_->AddConditionProcessor(
            std::make_unique<HorizontalSpacingStructureConditionProcessor>());
        structurePlacementConditionsManager_->AddConditionProcessor(std::make_unique<BiomeStructureConditionProcessor>());
        initialInventoryManager_ = std::make_unique<InitialInventoryManager>();
        tileResourceManager_->InitBuiltinTiles();
        biomeDecoratorManager_ = std::make_unique<BiomeDecoratorManager>();
        biomeDecoratorResourcesManager_ = std::make_unique<BiomeDecoratorResourcesManager>();
        itemManager_ = std::make_unique<ItemManager>();
        biomeDecoratorManager_->RegisterBiomeDecorator(std::make_unique<FillBiomeDecorator>());
        biomeDecoratorManager_->RegisterBiomeDecorator(std::make_unique<SurfaceBiomeDecorator>());
        biomeDecoratorManager_->RegisterBiomeDecorator(std::make_unique<MineralBiomeDecorator>());
        lootTableManager_ = std::make_unique<LootTableManager>();
        structureGeneratorManager_ = std::make_unique<StructureGeneratorManager>();
        structureGeneratorManager_->RegisterStructureGenerator(std::make_unique<StaticStructureGenerator>());
        structureGeneratorManager_->RegisterStructureGenerator(std::make_unique<TreeStructureGenerator>());
        structureManager_ = std::make_unique<StructureManager>();
        shapeManager_ = std::make_unique<ShapeManager>();
    }

    DataPackManager* ModContext::GetDataPackManager() const
    {
        return dataPackManager_.get();
    }

    StringManager* ModContext::GetStringManager() const
    {
        return stringManager_.get();
    }

    TileResourceManager* ModContext::GetTileResourceManager() const
    {
        return tileResourceManager_.get();
    }

    BiomesManager* ModContext::GetBiomesManager() const
    {
        return biomesManager_.get();
    }

    BiomeDecoratorManager* ModContext::GetBiomeDecoratorManager() const
    {
        return biomeDecoratorManager_.get();
    }

    BiomeDecoratorResourcesManager* ModContext::GetBiomeDecoratorResourcesManager() const
    {
        return biomeDecoratorResourcesManager_.get();
    }

    ItemManager* ModContext::GetItemManager() const
    {
        return itemManager_.get();
    }

    RecipeManager* ModContext::GetRecipeManager() const
    {
        return recipeManager_.get();
    }

    MobManager* ModContext::GetMobManager() const
    {
        return mobManager_.get();
    }

    StructureManager* ModContext::GetStructureManager() const
    {
        return structureManager_.get();
    }

    StructureGeneratorManager* ModContext::GetStructureGeneratorManager() const
    {
        return structureGeneratorManager_.get();
    }

    StructurePlacementConditionsManager* ModContext::GetStructurePlacementConditionsManager() const
    {
        return structurePlacementConditionsManager_.get();
    }

    LootTableManager* ModContext::GetLootTableManager() const
    {
        return lootTableManager_.get();
    }

    InitialInventoryManager* ModContext::GetInitialInventoryManager() const
    {
        return initialInventoryManager_.get();
    }

    ContributorManager* ModContext::GetContributorManager() const
    {
        return contributorManager_.get();
    }

    TomlTemplateExpander* ModContext::GetTomlTemplateExpander() const
    {
        return tomlTemplateExpander_.get();
    }

    ShapeManager* ModContext::GetShapeManager() const
    {
        return shapeManager_.get();
    }
}
