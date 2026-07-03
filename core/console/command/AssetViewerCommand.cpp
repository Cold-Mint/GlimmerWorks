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
#if  !defined(NDEBUG)
#include "AssetViewerCommand.h"

#include "core/Constants.h"
#include "core/mod/dataPack/StringManager.h"
#include "core/scene/AppContext.h"

void glimmer::AssetViewerCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
    if (suggestionsTree == nullptr)
    {
        return;
    }
    suggestionsTree->AddChild("string");
    suggestionsTree->AddChild("texture");
    suggestionsTree->AddChild("tile");
    suggestionsTree->AddChild("biomes");
    suggestionsTree->AddChild("composableItems");
    suggestionsTree->AddChild("abilityItems");
    suggestionsTree->AddChild("materialItems");
    suggestionsTree->AddChild("lootTables");
    suggestionsTree->AddChild("structures");
    suggestionsTree->AddChild("startinv");
    suggestionsTree->AddChild("mobs");
    suggestionsTree->AddChild("shapes");
    suggestionsTree->AddChild("biomeDecors");
    suggestionsTree->AddChild("fixedColor");
    suggestionsTree->AddChild("lightMask");
    suggestionsTree->AddChild("lightSource");
    suggestionsTree->AddChild("recipes");
}

glimmer::AssetViewerCommand::AssetViewerCommand(AppContext* appContext)
    : Command(appContext)
{
}

std::string glimmer::AssetViewerCommand::GetName() const
{
    return ASSET_VIEWER_COMMAND_NAME;
}

bool glimmer::AssetViewerCommand::RequiresWorldContext() const
{
    return false;
}

//skipcq: CXX-C2014
void glimmer::AssetViewerCommand::
PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
{
    if (strings == nullptr)
    {
        return;
    }
    strings->emplace_back("[asset type:string]");
}

bool glimmer::AssetViewerCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                          const std::function<void(const std::string& text)>* onMessage)
{
    if (appContext_ == nullptr || commandArgs == nullptr || onMessage == nullptr)
    {
        return false;
    }
    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    const auto type = commandArgs->AsString(1);
    if (type == "string")
    {
        const StringManager* stringManager = appContext_->GetStringManager();
        if (stringManager == nullptr)
        {
            return false;
        }
        onMessageRef(stringManager->ListStrings());
        return true;
    }

    if (type == "texture")
    {
        const ResourcePackManager* resourcePackManager = appContext_->GetResourcePackManager();
        if (resourcePackManager == nullptr)
        {
            return false;
        }
        onMessageRef(resourcePackManager->ListTextureCache());
        return true;
    }

    if (type == "tile")
    {
        const TileResourceManager* tileResourceManager = appContext_->GetTileResourceManager();
        if (tileResourceManager == nullptr)
        {
            return false;
        }
        onMessageRef(tileResourceManager->ListTiles());
        return true;
    }

    if (type == "biomes")
    {
        const BiomesManager* biomesManager = appContext_->GetBiomesManager();
        if (biomesManager == nullptr)
        {
            return false;
        }
        onMessageRef(biomesManager->ListBiomes());
        return true;
    }

    if (type == "composableItems")
    {
        const ItemManager* itemManager = appContext_->GetItemManager();
        if (itemManager == nullptr)
        {
            return false;
        }
        onMessageRef(itemManager->ListComposableItems());
        return true;
    }

    if (type == "abilityItems")
    {
        const ItemManager* itemManager = appContext_->GetItemManager();
        if (itemManager == nullptr)
        {
            return false;
        }
        onMessageRef(itemManager->ListAbilityItems());
        return true;
    }
    if (type == "materialItems")
    {
        const ItemManager* itemManager = appContext_->GetItemManager();
        if (itemManager == nullptr)
        {
            return false;
        }
        onMessageRef(itemManager->ListMaterialItems());
        return true;
    }

    if (type == "lootTables")
    {
        const LootTableManager* lootTableManager = appContext_->GetLootTableManager();
        if (lootTableManager == nullptr)
        {
            return false;
        }
        onMessageRef(lootTableManager->ListLootTables());
        return true;
    }

    if (type == "startinv")
    {
        const InitialInventoryManager* initialInventoryManager = appContext_->GetInitialInventoryManager();
        if (initialInventoryManager == nullptr)
        {
            return false;
        }
        onMessageRef(initialInventoryManager->ListInitialInventory());
        return true;
    }

    if (type == "structures")
    {
        const StructureManager* structureManager = appContext_->GetStructureManager();
        if (structureManager == nullptr)
        {
            return false;
        }
        onMessageRef(structureManager->ListStructures());
        return true;
    }

    if (type == "mobs")
    {
        const MobManager* mobManager = appContext_->GetMobManager();
        if (mobManager == nullptr)
        {
            return false;
        }
        onMessageRef(mobManager->ListMobs());
        return true;
    }

    if (type == "shapes")
    {
        const ShapeManager* shapeManager = appContext_->GetShapeManager();
        if (shapeManager == nullptr)
        {
            return false;
        }
        onMessageRef(shapeManager->ListShapes());
        return true;
    }
    if (type == "biomeDecors")
    {
        const BiomeDecoratorResourcesManager* decoratorResourcesManager = appContext_->
            GetBiomeDecoratorResourcesManager();
        if (decoratorResourcesManager == nullptr)
        {
            return false;
        }
        onMessageRef(decoratorResourcesManager->ListBiomeDecorators());
        return true;
    }
    if (type == "fixedColor")
    {
        const FixedColorManager* fixedColorManager = appContext_->GetFixedColorManager();
        if (fixedColorManager == nullptr)
        {
            return false;
        }
        onMessageRef(fixedColorManager->ListFixedColorResources());
        return true;
    }
    if (type == "lightMask")
    {
        const LightMaskManager* lightMaskManager = appContext_->GetLightMaskManager();
        if (lightMaskManager == nullptr)
        {
            return false;
        }
        onMessageRef(lightMaskManager->ListLightMaskResource());
        return true;
    }

    if (type == "lightSource")
    {
        const LightSourceManager* lightSourceManager = appContext_->GetLightSourceManager();
        if (lightSourceManager == nullptr)
        {
            return false;
        }
        onMessageRef(lightSourceManager->ListLightSourceResource());
        return true;
    }
    if (type == "recipes")
    {
        const RecipeManager* recipeManager = appContext_->GetRecipeManager();
        if (recipeManager == nullptr)
        {
            return false;
        }
        onMessageRef(recipeManager->ListRecipeResources());
        return true;
    }

    const LangsResources* langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr)
    {
        return false;
    }
    onMessageRef(langsResources->unknownAssetType);
    return false;
}
#endif
