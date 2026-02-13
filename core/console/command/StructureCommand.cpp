//
// Created by coldmint on 2026/2/13.
//

#include "StructureCommand.h"
#include "../../scene/AppContext.h"
#include "../../world/WorldContext.h"
#include "fmt/format.h"


void glimmer::StructureCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    suggestionsTree.AddChild("create")->AddChild(STRUCTURE_DYNAMIC_SUGGESTIONS_NAME);
}

glimmer::StructureCommand::StructureCommand(AppContext *ctx) : Command(ctx) {
}

std::string glimmer::StructureCommand::GetName() const {
    return STRUCTURE_COMMAND_NAME;
}

void glimmer::StructureCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
    strings.emplace_back("[create:string]");
    strings.emplace_back("[structureId:string]");
}

bool glimmer::StructureCommand::RequiresWorldContext() const {
    return true;
}

bool glimmer::StructureCommand::Execute(CommandArgs commandArgs,
                                        std::function<void(const std::string &text)> onMessage) {
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
    std::string type = commandArgs.AsString(1);
    if (type == "create") {
        auto structureId = commandArgs.AsResourceRef(2, RESOURCE_TYPE_STRUCTURE);
        StructureResource *structureResource = appContext_->GetStructureManager()->Find(
            structureId->GetPackageId(), structureId->GetResourceKey());
        if (structureResource == nullptr) {
            return false;
        }

        Transform2DComponent *transform2dComponent = worldContext_->GetComponent<Transform2DComponent>(
            worldContext_->GetPlayerEntity());
        if (transform2dComponent == nullptr) {
            return false;
        }
        auto tilePosition = TileLayerComponent::WorldToTile(transform2dComponent->GetPosition());
    }
    return false;
}
