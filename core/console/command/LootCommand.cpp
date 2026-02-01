//
// Created by coldmint on 2026/1/30.
//

#include "LootCommand.h"
#include "../../scene/AppContext.h"
#include "../../world/WorldContext.h"
#include "core/ecs/component/ItemContainerComonent.h"
#include "fmt/format.h"

void glimmer::LootCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    suggestionsTree.AddChild("get")->AddChild(LOOT_DYNAMIC_SUGGESTIONS_NAME);
}

std::string glimmer::LootCommand::GetName() const {
    return LOOT_COMMAND_NAME;
}

void glimmer::LootCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
    strings.emplace_back("[type:string]");
    strings.emplace_back("[lootTableId:string]");
}

bool glimmer::LootCommand::RequiresWorldContext() const {
    return true;
}

bool glimmer::LootCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) {
    if (worldContext_ == nullptr) {
        onMessage(appContext_->GetLangsResources()->worldContextIsNull);
        return false;
    }
    const size_t size = commandArgs.GetSize();
    if (size < 3) {
        onMessage(fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->insufficientParameterLength),
            3, size));
        return false;
    }
    const std::string type = commandArgs.AsString(1);
    if (type == "get") {
        auto playerId = worldContext_->GetPlayerEntity();
        auto *item_container = worldContext_->GetComponent<ItemContainerComponent>(playerId);
        if (item_container == nullptr) {
            onMessage(appContext_->GetLangsResources()->itemContainerIsNull);
            return false;
        }
        auto lootId = commandArgs.AsResourceRef(2, RESOURCE_TYPE_LOOT_TABLE);
        if (!lootId.has_value()) {
            onMessage(appContext_->GetLangsResources()->lootTableNotFound);
            return false;
        }
        ResourceRef &resourceRef = lootId.value();
        std::optional<LootResource *> lootResource = appContext_->GetResourceLocator()->FindLoot(resourceRef);
        if (!lootResource.has_value()) {
            return false;
        }
        auto resourceRefList = LootResource::GetLootItems(lootResource.value());
        if (resourceRefList.empty()) {
            return false;
        }
        for (auto &resourceRefItem: resourceRefList) {
            auto itemRes = appContext_->GetResourceLocator()->FindItem(appContext_, resourceRefItem);
            if (itemRes.has_value()) {
                std::unique_ptr<Item> item = item_container->GetItemContainer()->AddItem(
                    std::move(itemRes.value()));
            }
        }
        return true;
    }
    return false;
}
