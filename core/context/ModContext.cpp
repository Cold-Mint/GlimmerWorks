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


glimmer::ModContext::ModContext() = default;

glimmer::ModContext::~ModContext() = default;

void glimmer::ModContext::Init(VirtualFileSystem* vfs, const LangsResources* langsResources)
{
    LogCat::i("Initializing ModContext");

    LogCat::d("Creating ContributorManager");
    contributorManager_ = std::make_unique<ContributorManager>();

    LogCat::d("Creating RecipeManager");
    recipeManager_ = std::make_unique<RecipeManager>();

    LogCat::d("Creating MobManager");
    mobManager_ = std::make_unique<MobManager>();

    LogCat::d("Creating TomlTemplateExpander");
    tomlTemplateExpander_ = std::make_unique<TomlTemplateExpander>();
    tomlTemplateExpander_->Register(std::make_unique<InsertTemplateCommand>());
    tomlTemplateExpander_->Register(std::make_unique<SetTemplateCommand>());
    tomlTemplateExpander_->Register(std::make_unique<UnSetTemplateCommand>());

    LogCat::d("Creating DataPackManager");
    dataPackManager_ = std::make_unique<DataPackManager>(vfs, tomlTemplateExpander_.get());

    LogCat::d("Creating StringManager and loading language strings");
    stringManager_ = std::make_unique<StringManager>();
    stringManager_->LoadLangsString(langsResources);

    LogCat::d("Creating BiomesManager");
    biomesManager_ = std::make_unique<BiomesManager>();

    LogCat::d("Creating TileResourceManager");
    tileResourceManager_ = std::make_unique<TileResourceManager>();

    LogCat::d("Creating StructurePlacementConditionsManager");
    structurePlacementConditionsProcessorManager_ = std::make_unique<StructurePlacementConditionsProcessorManager>();
    structurePlacementConditionsProcessorManager_->AddConditionProcessor(
        std::make_unique<SurfaceStructureConditionProcessor>());
    structurePlacementConditionsProcessorManager_->AddConditionProcessor(
        std::make_unique<HeightStructureConditionProcessor>());
    structurePlacementConditionsProcessorManager_->AddConditionProcessor(
        std::make_unique<HorizontalSpacingStructureConditionProcessor>());
    structurePlacementConditionsProcessorManager_->AddConditionProcessor(
        std::make_unique<BiomeStructureConditionProcessor>());

    structurePlacementConditionsResourceManager_ = std::make_unique<StructurePlacementConditionsResourceManager>();

    LogCat::d("Creating InitialInventoryManager");
    initialInventoryManager_ = std::make_unique<InitialInventoryManager>();

    LogCat::d("Initializing built-in tiles");
    tileResourceManager_->InitBuiltinTiles();

    LogCat::d("Creating BiomeDecoratorManager");
    biomeDecoratorManager_ = std::make_unique<BiomeDecoratorManager>();

    LogCat::d("Creating BiomeDecoratorResourcesManager");
    biomeDecoratorResourcesManager_ = std::make_unique<BiomeDecoratorResourcesManager>();

    LogCat::d("Creating ItemManager");
    abilityItemManager_ = std::make_unique<AbilityItemManager>();
    composableItemManager_ = std::make_unique<ComposableItemManager>();
    materialItemManager_ = std::make_unique<MaterialItemManager>();

    LogCat::d("Registering biome decorators");
    biomeDecoratorManager_->RegisterBiomeDecorator(std::make_unique<FillBiomeDecorator>());
    biomeDecoratorManager_->RegisterBiomeDecorator(std::make_unique<SurfaceBiomeDecorator>());
    biomeDecoratorManager_->RegisterBiomeDecorator(std::make_unique<MineralBiomeDecorator>());

    LogCat::d("Creating LootTableManager");
    lootTableManager_ = std::make_unique<LootTableManager>();

    LogCat::d("Creating StructureGeneratorManager");
    structureGeneratorManager_ = std::make_unique<StructureGeneratorManager>();
    structureGeneratorManager_->RegisterStructureGenerator(std::make_unique<StaticStructureGenerator>());
    structureGeneratorManager_->RegisterStructureGenerator(std::make_unique<TreeStructureGenerator>());

    LogCat::d("Creating StructureManager");
    structureManager_ = std::make_unique<StructureManager>();

    LogCat::d("Creating ShapeManager");
    shapeManager_ = std::make_unique<ShapeManager>();

    LogCat::i("ModContext initialization completed");
}

glimmer::DataPackManager* glimmer::ModContext::GetDataPackManager() const
{
    return dataPackManager_.get();
}

glimmer::StringManager* glimmer::ModContext::GetStringManager() const
{
    return stringManager_.get();
}

glimmer::TileResourceManager* glimmer::ModContext::GetTileResourceManager() const
{
    return tileResourceManager_.get();
}

glimmer::BiomesManager* glimmer::ModContext::GetBiomesManager() const
{
    return biomesManager_.get();
}

glimmer::BiomeDecoratorManager* glimmer::ModContext::GetBiomeDecoratorManager() const
{
    return biomeDecoratorManager_.get();
}

glimmer::BiomeDecoratorResourcesManager* glimmer::ModContext::GetBiomeDecoratorResourcesManager() const
{
    return biomeDecoratorResourcesManager_.get();
}

glimmer::AbilityItemManager* glimmer::ModContext::GetAbilityItemManager() const
{
    return abilityItemManager_.get();
}

glimmer::ComposableItemManager* glimmer::ModContext::GetComposableItemManager() const
{
    return composableItemManager_.get();
}

glimmer::MaterialItemManager* glimmer::ModContext::GetMaterialItemManager() const
{
    return materialItemManager_.get();
}


glimmer::RecipeManager* glimmer::ModContext::GetRecipeManager() const
{
    return recipeManager_.get();
}

glimmer::MobManager* glimmer::ModContext::GetMobManager() const
{
    return mobManager_.get();
}

glimmer::StructureManager* glimmer::ModContext::GetStructureManager() const
{
    return structureManager_.get();
}

glimmer::StructureGeneratorManager* glimmer::ModContext::GetStructureGeneratorManager() const
{
    return structureGeneratorManager_.get();
}

glimmer::StructurePlacementConditionsProcessorManager*
glimmer::ModContext::GetStructurePlacementConditionsProcessorManager() const
{
    return structurePlacementConditionsProcessorManager_.get();
}

glimmer::StructurePlacementConditionsResourceManager* glimmer::ModContext::
GetStructurePlacementConditionsResourceManager() const
{
    return structurePlacementConditionsResourceManager_.get();
}

glimmer::LootTableManager* glimmer::ModContext::GetLootTableManager() const
{
    return lootTableManager_.get();
}

glimmer::InitialInventoryManager* glimmer::ModContext::GetInitialInventoryManager() const
{
    return initialInventoryManager_.get();
}

glimmer::ContributorManager* glimmer::ModContext::GetContributorManager() const
{
    return contributorManager_.get();
}

glimmer::TomlTemplateExpander* glimmer::ModContext::GetTomlTemplateExpander() const
{
    return tomlTemplateExpander_.get();
}

glimmer::ShapeManager* glimmer::ModContext::GetShapeManager() const
{
    return shapeManager_.get();
}
