//
// Created by Cold-Mint on 2025/11/27.
//

#include "AssetViewerCommand.h"

#include "../../Constants.h"
#include "../../mod/dataPack/StringManager.h"
#include "../../scene/AppContext.h"

void glimmer::AssetViewerCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    suggestionsTree.AddChild("string");
    suggestionsTree.AddChild("texture")->AddChild(BOOL_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree.AddChild("tile");
    suggestionsTree.AddChild("biomes");
    suggestionsTree.AddChild("items");
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
    } else if (type == "items") {
        onMessage(appContext_->GetItemManager()->ListItems());
        result = true;
    } else {
        onMessage(appContext_->GetLangsResources()->unknownAssetType);
        result = false;
    }
    return result;
}
