//
// Created by Cold-Mint on 2025/12/23.
//

#include "GiveCommand.h"

#include "../../Constants.h"
#include "../../ecs/component/ItemContainerComonent.h"
#include "../../inventory/AbilityItem.h"
#include "../../inventory/ComposableItem.h"
#include "../../inventory/ItemContainer.h"
#include "../../inventory/TileItem.h"
#include "../../mod/Resource.h"
#include "../../mod/ResourceLocator.h"
#include "../../scene/AppContext.h"
#include "../../world/WorldContext.h"
#include "fmt/color.h"

void glimmer::GiveCommand::InitSuggestions(NodeTree<std::string> *suggestionsTree) {
    if (suggestionsTree == nullptr) {
        return;
    }
    suggestionsTree->AddChild("tileItem")->AddChild(TILE_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree->AddChild("composableItem")->AddChild(COMPOSABLE_ITEM_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree->AddChild("abilityItem")->AddChild(ABILITY_ITEM_DYNAMIC_SUGGESTIONS_NAME);
}

glimmer::GiveCommand::GiveCommand(AppContext *appContext)
    : Command(appContext) {
}

void glimmer::GiveCommand::PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) {
    if (strings == nullptr) {
        return;
    }
    strings->emplace_back("[item_type:string]");
    strings->emplace_back("[item_id:string]");
    strings->emplace_back("[number:int]");
}

bool glimmer::GiveCommand::Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                                   const std::function<void(const std::string &text)> *onMessage) {
    if (appContext_ == nullptr || commandArgs == nullptr || onMessage == nullptr) {
        return false;
    }
    const std::function<void(const std::string &text)> &onMessageRef = *onMessage;
    if (worldContext_ == nullptr) {
        onMessageRef(appContext_->GetLangsResources()->worldContextIsNull);
        return false;
    }
    const size_t size = commandArgs->GetSize();
    if (size < 3) {
        onMessageRef(fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->insufficientParameterLength),
            3, size));
        return false;
    }
    const std::string itemType = commandArgs->AsString(1);
    if (itemType == "tileItem") {
        auto itemId = commandArgs->AsResourceRef(2, RESOURCE_TYPE_TILE);
        if (!itemId.has_value()) {
            onMessageRef(appContext_->GetLangsResources()->itemIdNotFound);
            return false;
        }
        ResourceRef &resourceRef = itemId.value();
        auto tileResource = appContext_->GetResourceLocator()->FindTileRaw(resourceRef);
        if (tileResource == nullptr) {
            onMessageRef(appContext_->GetLangsResources()->tileResourceIsNull);
            return false;
        }
        auto playerId = worldContext_->GetPlayerEntity();
        auto *item_container = worldContext_->GetComponent<ItemContainerComponent>(playerId);
        if (item_container == nullptr) {
            onMessageRef(appContext_->GetLangsResources()->itemContainerIsNull);
            return false;
        }
        auto tileItem = std::make_unique<TileItem>(Tile::FromTileResource(appContext_, tileResource, resourceRef));
        if (size >= 4) {
            if (const int number = commandArgs->AsInt(3); number > 1) {
                tileItem->SetAmount(number);
            }
        }
        std::unique_ptr<Item> item = item_container->GetItemContainer()->AddItem(
            std::move(tileItem));
        return item == nullptr;
    }
    if (itemType == "composableItem") {
        auto itemId = commandArgs->AsResourceRef(2, RESOURCE_TYPE_COMPOSABLE_ITEM);
        if (!itemId.has_value()) {
            onMessageRef(appContext_->GetLangsResources()->itemIdNotFound);
            return false;
        }
        ResourceRef &resourceRef = itemId.value();
        auto itemResource = appContext_->GetResourceLocator()->FindComposableItem(resourceRef);
        if (itemResource == nullptr) {
            onMessageRef(appContext_->GetLangsResources()->itemResourceIsNull);
            return false;
        }
        auto playerId = worldContext_->GetPlayerEntity();
        auto *item_container = worldContext_->GetComponent<ItemContainerComponent>(playerId);
        if (item_container == nullptr) {
            onMessageRef(appContext_->GetLangsResources()->itemContainerIsNull);
            return false;
        }

        auto composableItem = ComposableItem::FromItemResource(appContext_, itemResource, resourceRef);
        if (composableItem == nullptr) {
            onMessageRef(appContext_->GetLangsResources()->composableItemIsNull);
            return false;
        }
        if (size >= 4) {
            if (const int number = commandArgs->AsInt(3); number > 1) {
                composableItem->SetAmount(number);
            }
        }
        std::unique_ptr<Item> item = item_container->GetItemContainer()->AddItem(
            std::move(composableItem));
        return item == nullptr;
    }
    if (itemType == "abilityItem") {
        auto itemId = commandArgs->AsResourceRef(2, RESOURCE_TYPE_ABILITY_ITEM);
        if (!itemId.has_value()) {
            onMessageRef(appContext_->GetLangsResources()->itemIdNotFound);
            return false;
        }
        ResourceRef &resourceRef = itemId.value();
        auto itemResource = appContext_->GetResourceLocator()->FindAbilityItem(resourceRef);
        if (itemResource == nullptr) {
            onMessageRef(appContext_->GetLangsResources()->itemResourceIsNull);
            return false;
        }
        auto playerId = worldContext_->GetPlayerEntity();
        auto *item_container = worldContext_->GetComponent<ItemContainerComponent>(playerId);
        if (item_container == nullptr) {
            onMessageRef(appContext_->GetLangsResources()->itemContainerIsNull);
            return false;
        }

        auto abilityItem = AbilityItem::FromItemResource(appContext_, itemResource, resourceRef);
        if (abilityItem == nullptr) {
            onMessageRef(appContext_->GetLangsResources()->composableItemIsNull);
            return false;
        }
        if (size >= 4) {
            if (const int number = commandArgs->AsInt(3); number > 1) {
                abilityItem->SetAmount(number);
            }
        }
        std::unique_ptr<Item> item = item_container->GetItemContainer()->AddItem(
            std::move(abilityItem));
        return item == nullptr;
    }
    onMessageRef(appContext_->GetLangsResources()->unknownCommandParameters);
    return false;
}

std::string glimmer::GiveCommand::GetName() const {
    return GIVE_COMMAND_NAME;
}

bool glimmer::GiveCommand::RequiresWorldContext() const {
    return true;
}
