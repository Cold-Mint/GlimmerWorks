//
// Created by Cold-Mint on 2025/11/27.
//

#include "AssetViewerCommand.h"

#include "../../Constants.h"
#include "../../mod/dataPack/StringManager.h"
#include "../../scene/AppContext.h"

glimmer::AssetViewerCommand::AssetViewerCommand(AppContext *ctx)
    : Command(ctx) {
}

void glimmer::AssetViewerCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    suggestionsTree.AddChild("string");
    suggestionsTree.AddChild("texture")->AddChild(BOOL_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree.AddChild("tile");
    suggestionsTree.AddChild("biomes");
    suggestionsTree.AddChild("composableItems");
    suggestionsTree.AddChild("abilityItems");
    suggestionsTree.AddChild("lootTables");
    suggestionsTree.AddChild("structures");
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
    if (commandArgs.AsString(1) == "texture") {
        strings.emplace_back("[include expired:bool]");
    }
}

bool glimmer::AssetViewerCommand::Execute(const CommandArgs commandArgs,
                                          const std::function<void(const std::string &text)> onMessage) {
    const auto type = commandArgs.AsString(1);
    bool result = false;
    if (type == "string") {
        onMessage(appContext_->GetStringManager()->ListStrings());
        result = true;
    } else if (type == "texture") {
        onMessage(appContext_->GetResourcePackManager()->ListTextureCache(commandArgs.AsBool(2)));
        result = true;
    } else if (type == "tile") {
        onMessage(appContext_->GetTileManager()->ListTiles());
        result = true;
    } else if (type == "biomes") {
        onMessage(appContext_->GetBiomesManager()->ListBiomes());
        result = true;
    } else if (type == "composableItems") {
        onMessage(appContext_->GetItemManager()->ListComposableItems());
        result = true;
    } else if (type == "abilityItems") {
        onMessage(appContext_->GetItemManager()->ListAbilityItems());
        result = true;
    } else if (type == "lootTables") {
        onMessage(appContext_->GetLootTableManager()->ListLootTables());
        result = true;
    } else if (type == "structures") {
        onMessage(appContext_->GetStructureManager()->ListStructures());
        result = true;
    } else {
        onMessage(appContext_->GetLangsResources()->unknownAssetType);
        result = false;
    }
    return result;
}
