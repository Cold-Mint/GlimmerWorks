//
// Created by Cold-Mint on 2025/11/27.
//

#include "AssetViewerCommand.h"

#include "../../Constants.h"
#include "../../mod/dataPack/StringManager.h"
#include "../../scene/AppContext.h"

glimmer::AssetViewerCommand::AssetViewerCommand(AppContext *appContext)
    : Command(appContext) {
}

void glimmer::AssetViewerCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    suggestionsTree.AddChild("string");
    suggestionsTree.AddChild("texture");
    suggestionsTree.AddChild("tile");
    suggestionsTree.AddChild("biomes");
    suggestionsTree.AddChild("composableItems");
    suggestionsTree.AddChild("abilityItems");
    suggestionsTree.AddChild("lootTables");
    suggestionsTree.AddChild("structures");
    suggestionsTree.AddChild("startinv");
    suggestionsTree.AddChild("mobs");
    suggestionsTree.AddChild("shapes");
}

std::string glimmer::AssetViewerCommand::GetName() const {
    return ASSET_VIEWER_COMMAND_NAME;
}

bool glimmer::AssetViewerCommand::RequiresWorldContext() const {
    return false;
}

void glimmer::AssetViewerCommand::
PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
    strings.emplace_back("[asset type:string]");
}

bool glimmer::AssetViewerCommand::Execute(const CommandArgs commandArgs,
                                          const std::function<void(const std::string &text)> onMessage) {
    if (appContext_ == nullptr) {
        return false;
    }
    const auto type = commandArgs.AsString(1);
    if (type == "string") {
        const StringManager *stringManager = appContext_->GetStringManager();
        if (stringManager == nullptr) {
            return false;
        }
        onMessage(stringManager->ListStrings());
        return true;
    }

    if (type == "texture") {
        const ResourcePackManager *resourcePackManager = appContext_->GetResourcePackManager();
        if (resourcePackManager == nullptr) {
            return false;
        }
        onMessage(resourcePackManager->ListTextureCache());
        return true;
    }

    if (type == "tile") {
        const TileManager *tileManager = appContext_->GetTileManager();
        if (tileManager == nullptr) {
            return false;
        }
        onMessage(tileManager->ListTiles());
        return true;
    }

    if (type == "biomes") {
        const BiomesManager *biomesManager = appContext_->GetBiomesManager();
        if (biomesManager == nullptr) {
            return false;
        }
        onMessage(biomesManager->ListBiomes());
        return true;
    }

    if (type == "composableItems") {
        const ItemManager *itemManager = appContext_->GetItemManager();
        if (itemManager == nullptr) {
            return false;
        }
        onMessage(itemManager->ListComposableItems());
        return true;
    }

    if (type == "abilityItems") {
        const ItemManager *itemManager = appContext_->GetItemManager();
        if (itemManager == nullptr) {
            return false;
        }
        onMessage(itemManager->ListAbilityItems());
        return true;
    }

    if (type == "lootTables") {
        const LootTableManager *lootTableManager = appContext_->GetLootTableManager();
        if (lootTableManager == nullptr) {
            return false;
        }
        onMessage(lootTableManager->ListLootTables());
        return true;
    }

    if (type == "startinv") {
        const InitialInventoryManager *initialInventoryManager = appContext_->GetInitialInventoryManager();
        if (initialInventoryManager == nullptr) {
            return false;
        }
        onMessage(initialInventoryManager->ListInitialInventory());
        return true;
    }

    if (type == "structures") {
        const StructureManager *structureManager = appContext_->GetStructureManager();
        if (structureManager == nullptr) {
            return false;
        }
        onMessage(structureManager->ListStructures());
        return true;
    }

    if (type == "mobs") {
        const MobManager *mobManager = appContext_->GetMobManager();
        if (mobManager == nullptr) {
            return false;
        }
        onMessage(mobManager->ListMobs());
        return true;
    }

    if (type == "shapes") {
        const ShapeManager *shapeManager = appContext_->GetShapeManager();
        if (shapeManager == nullptr) {
            return false;
        }
        onMessage(shapeManager->ListShapes());
        return true;
    }
    const LangsResources *langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr) {
        return false;
    }
    onMessage(langsResources->unknownAssetType);
    return false;
}
