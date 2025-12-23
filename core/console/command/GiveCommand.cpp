//
// Created by coldmint on 2025/12/23.
//

#include "GiveCommand.h"

#include "../../Constants.h"
#include "../../inventory/TileItem.h"
#include "../../mod/Resource.h"
#include "../../mod/ResourceLocator.h"
#include "../../scene/AppContext.h"
#include "../../world/WorldContext.h"

void glimmer::GiveCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    suggestionsTree.AddChild("tileItem")->AddChild(TILE_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree.AddChild("composableItem")->AddChild(COMPOSABLE_ITEM_DYNAMIC_SUGGESTIONS_NAME);
}

bool glimmer::GiveCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onOutput) {
    if (worldContext_ == nullptr) {
        onOutput("WorldContext is nullptr");
        return false;
    }
    if (commandArgs.GetSize() > 2) {
        const std::string itemType = commandArgs.AsString(1);
        if (itemType == "tileItem") {
            auto itemId = commandArgs.AsResourceRef(2, RESOURCE_TYPE_TILE);
            if (!itemId.has_value()) {
                return false;
            }
            ResourceRef &resourceRef = itemId.value();
            auto tileResourceOptional = appContext_->GetResourceLocator()->FindTile(resourceRef);
            if (!tileResourceOptional.has_value()) {
                return false;
            }
            auto tileId = tileResourceOptional.value();
            if (tileId == nullptr) {
                return false;
            }
            TileItem tileItem = TileItem(Tile::FromResourceRef(appContext_,tileId));
           // worldContext_->GetPlayerEntity()->GetID()
        }
        // if (itemType == "composableItem") {

        // }
    }
    return false;
}

void glimmer::GiveCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
    strings.emplace_back("[item_type:string]");
    strings.emplace_back("[item_id:string]");
    strings.emplace_back("[number:int]");
}

std::string glimmer::GiveCommand::GetName() const {
    return GIVE_COMMAND_NAME;
}

bool glimmer::GiveCommand::RequiresWorldContext() const {
    return true;
}
