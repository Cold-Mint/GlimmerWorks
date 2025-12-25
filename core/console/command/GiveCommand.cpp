//
// Created by coldmint on 2025/12/23.
//

#include "GiveCommand.h"

#include "../../Constants.h"
#include "../../ecs/component/ItemContainerComonent.h"
#include "../../inventory/ComposableItem.h"
#include "../../inventory/ItemContainer.h"
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
    if (commandArgs.GetSize() >= 3) {
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
            auto playerId = worldContext_->GetPlayerEntity()->GetID();
            auto *item_container = worldContext_->GetComponent<ItemContainerComponent>(playerId);
            if (item_container == nullptr) {
                return false;
            }
            auto tileItem = std::make_unique<TileItem>(Tile::FromResourceRef(appContext_, tileId));
            if (commandArgs.GetSize() >= 4) {
                const int number = commandArgs.AsInt(3);
                if (number > 1) {
                    (void) tileItem->AddAmount(number - 1);
                }
            }
            std::unique_ptr<Item> item = item_container->GetItemContainer()->AddItem(
                std::move(tileItem));
            return item == nullptr;
        }
        if (itemType == "composableItem") {
            auto itemId = commandArgs.AsResourceRef(2, RESOURCE_TYPE_ITEM);
            if (!itemId.has_value()) {
                return false;
            }
            ResourceRef &resourceRef = itemId.value();
            auto itemResourceOptional = appContext_->GetResourceLocator()->FindItem(resourceRef);
            if (!itemResourceOptional.has_value()) {
                return false;
            }
            auto itemResource = itemResourceOptional.value();
            if (itemResource == nullptr) {
                return false;
            }
            auto playerId = worldContext_->GetPlayerEntity()->GetID();
            auto *item_container = worldContext_->GetComponent<ItemContainerComponent>(playerId);
            if (item_container == nullptr) {
                return false;
            }

            auto composableItem = ComposableItem::FromItemResource(appContext_, itemResource);
            if (composableItem == nullptr) {
                return false;
            }
            if (commandArgs.GetSize() >= 4) {
                const int number = commandArgs.AsInt(3);
                if (number > 1) {
                    (void) composableItem->AddAmount(number - 1);
                }
            }
            std::unique_ptr<Item> item = item_container->GetItemContainer()->AddItem(
                std::move(composableItem));
            return item == nullptr;
        }
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
