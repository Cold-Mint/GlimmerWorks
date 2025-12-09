//
// Created by Cold-Mint on 2025/11/27.
//

#include "AssetViewerCommand.h"

#include "../../Constants.h"
#include "../../mod/dataPack/StringManager.h"
#include "../../scene/AppContext.h"

void glimmer::AssetViewerCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    suggestionsTree.AddChild("string");
    auto texture = suggestionsTree.AddChild("texture");
    texture->AddChild(BOOL_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree.AddChild("tile");
    suggestionsTree.AddChild("biomes");
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
    const auto type = commandArgs.AsString(1);
    if (type == "texture") {
        strings.emplace_back("[include expired:bool]");
    }
}

bool glimmer::AssetViewerCommand::Execute(CommandArgs commandArgs,
                                          std::function<void(const std::string &text)> onOutput) {
    const auto type = commandArgs.AsString(1);
    if (type == "string") {
        onOutput(appContext_->GetStringManager()->ListStrings());
    } else if (type == "texture") {
        onOutput(appContext_->GetResourcePackManager()->ListTextureCache(commandArgs.AsBool(2)));
    } else if (type == "tile") {
        onOutput(appContext_->GetTileManager()->ListTiles());
    } else if (type == "biomes") {
        onOutput(appContext_->GetBiomesManager()->ListTiles());
    }
    return true;
}
