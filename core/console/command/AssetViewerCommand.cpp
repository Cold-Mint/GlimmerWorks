//
// Created by Cold-Mint on 2025/11/27.
//

#include "AssetViewerCommand.h"

#include "../../Constants.h"
#include "../../mod/dataPack/StringManager.h"
#include "../../scene/AppContext.h"

void glimmer::AssetViewerCommand::InitSuggestions(NodeTree<std::string> *suggestionsTree) {
    if (suggestionsTree == nullptr) {
        return;
    }
    suggestionsTree->AddChild("string");
    suggestionsTree->AddChild("texture");
    suggestionsTree->AddChild("tile");
    suggestionsTree->AddChild("biomes");
    suggestionsTree->AddChild("composableItems");
    suggestionsTree->AddChild("abilityItems");
    suggestionsTree->AddChild("lootTables");
    suggestionsTree->AddChild("structures");
    suggestionsTree->AddChild("startinv");
    suggestionsTree->AddChild("mobs");
    suggestionsTree->AddChild("shapes");
    suggestionsTree->AddChild("biomeDecors");
    suggestionsTree->AddChild("fixedColor");
    suggestionsTree->AddChild("lightMask");
    suggestionsTree->AddChild("lightSource");
}

glimmer::AssetViewerCommand::AssetViewerCommand(AppContext *appContext)
    : Command(appContext) {
}

std::string glimmer::AssetViewerCommand::GetName() const {
    return ASSET_VIEWER_COMMAND_NAME;
}

bool glimmer::AssetViewerCommand::RequiresWorldContext() const {
    return false;
}

void glimmer::AssetViewerCommand::
PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) {
    if (strings == nullptr) {
        return;
    }
    strings->emplace_back("[asset type:string]");
}

bool glimmer::AssetViewerCommand::Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                                          const std::function<void(const std::string &text)> *onMessage) {
    if (appContext_ == nullptr || commandArgs == nullptr || onMessage == nullptr) {
        return false;
    }
    const std::function<void(const std::string &text)> &onMessageRef = *onMessage;
    const auto type = commandArgs->AsString(1);
    if (type == "string") {
        const StringManager *stringManager = appContext_->GetStringManager();
        if (stringManager == nullptr) {
            return false;
        }
        onMessageRef(stringManager->ListStrings());
        return true;
    }

    if (type == "texture") {
        const ResourcePackManager *resourcePackManager = appContext_->GetResourcePackManager();
        if (resourcePackManager == nullptr) {
            return false;
        }
        onMessageRef(resourcePackManager->ListTextureCache());
        return true;
    }

    if (type == "tile") {
        const TileManager *tileManager = appContext_->GetTileManager();
        if (tileManager == nullptr) {
            return false;
        }
        onMessageRef(tileManager->ListTiles());
        return true;
    }

    if (type == "biomes") {
        const BiomesManager *biomesManager = appContext_->GetBiomesManager();
        if (biomesManager == nullptr) {
            return false;
        }
        onMessageRef(biomesManager->ListBiomes());
        return true;
    }

    if (type == "composableItems") {
        const ItemManager *itemManager = appContext_->GetItemManager();
        if (itemManager == nullptr) {
            return false;
        }
        onMessageRef(itemManager->ListComposableItems());
        return true;
    }

    if (type == "abilityItems") {
        const ItemManager *itemManager = appContext_->GetItemManager();
        if (itemManager == nullptr) {
            return false;
        }
        onMessageRef(itemManager->ListAbilityItems());
        return true;
    }

    if (type == "lootTables") {
        const LootTableManager *lootTableManager = appContext_->GetLootTableManager();
        if (lootTableManager == nullptr) {
            return false;
        }
        onMessageRef(lootTableManager->ListLootTables());
        return true;
    }

    if (type == "startinv") {
        const InitialInventoryManager *initialInventoryManager = appContext_->GetInitialInventoryManager();
        if (initialInventoryManager == nullptr) {
            return false;
        }
        onMessageRef(initialInventoryManager->ListInitialInventory());
        return true;
    }

    if (type == "structures") {
        const StructureManager *structureManager = appContext_->GetStructureManager();
        if (structureManager == nullptr) {
            return false;
        }
        onMessageRef(structureManager->ListStructures());
        return true;
    }

    if (type == "mobs") {
        const MobManager *mobManager = appContext_->GetMobManager();
        if (mobManager == nullptr) {
            return false;
        }
        onMessageRef(mobManager->ListMobs());
        return true;
    }

    if (type == "shapes") {
        const ShapeManager *shapeManager = appContext_->GetShapeManager();
        if (shapeManager == nullptr) {
            return false;
        }
        onMessageRef(shapeManager->ListShapes());
        return true;
    }
    if (type == "biomeDecors") {
        const BiomeDecoratorResourcesManager *decoratorResourcesManager = appContext_->
                GetBiomeDecoratorResourcesManager();
        if (decoratorResourcesManager == nullptr) {
            return false;
        }
        onMessageRef(decoratorResourcesManager->ListBiomeDecorators());
        return true;
    }
    if (type == "fixedColor") {
        const FixedColorManager *fixedColorManager = appContext_->GetFixedColorManager();
        if (fixedColorManager == nullptr) {
            return false;
        }
        onMessageRef(fixedColorManager->ListFixedColorResources());
        return true;
    }
    if (type == "lightMask") {
        const LightMaskManager *lightMaskManager = appContext_->GetLightMaskManager();
        if (lightMaskManager == nullptr) {
            return false;
        }
        onMessageRef(lightMaskManager->ListLightMaskResource());
        return true;
    }

    if (type == "lightSource") {
        const LightSourceManager *lightSourceManager = appContext_->GetLightSourceManager();
        if (lightSourceManager == nullptr) {
            return false;
        }
        onMessageRef(lightSourceManager->ListLightSourceResource());
        return true;
    }

    const LangsResources *langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr) {
        return false;
    }
    onMessageRef(langsResources->unknownAssetType);
    return false;
}
