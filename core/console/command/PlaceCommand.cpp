//
// Created by Cold-Mint on 2026/2/13.
//

#include "PlaceCommand.h"
#include "../../scene/AppContext.h"
#include "../../world/WorldContext.h"
#include "core/world/generator/ChunkPhysicsHelper.h"
#include "fmt/format.h"


void glimmer::PlaceCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    suggestionsTree.AddChild("structure")->AddChild(STRUCTURE_DYNAMIC_SUGGESTIONS_NAME)->
            AddChild(X_DYNAMIC_SUGGESTIONS_NAME)->AddChild(Y_DYNAMIC_SUGGESTIONS_NAME);
}

glimmer::PlaceCommand::PlaceCommand(AppContext *appContext) : Command(appContext) {
}

std::string glimmer::PlaceCommand::GetName() const {
    return PLACE_COMMAND_NAME;
}

void glimmer::PlaceCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
    strings.emplace_back("[structure:string]");
    strings.emplace_back("[structureId:string]");
    strings.emplace_back("[x:int]");
    strings.emplace_back("[y:int]");
}

bool glimmer::PlaceCommand::RequiresWorldContext() const {
    return true;
}

bool glimmer::PlaceCommand::Execute(const CommandSender *commandSender, CommandArgs commandArgs,
                                    std::function<void(const std::string &text)> onMessage) {
    if (appContext_ == nullptr) {
        return false;
    }
    if (worldContext_ == nullptr) {
        onMessage(appContext_->GetLangsResources()->worldContextIsNull);
        return false;
    }
    const size_t size = commandArgs.GetSize();
    if (size < 5) {
        onMessage(fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->insufficientParameterLength),
            5, size));
        return false;
    }
    std::string type = commandArgs.AsString(1);
    if (type == "structure") {
        auto structureId = commandArgs.AsResourceRef(2, RESOURCE_TYPE_STRUCTURE);
        StructureResource *structureResource = appContext_->GetStructureManager()->Find(
            structureId->GetPackageId(), structureId->GetResourceKey());
        if (structureResource == nullptr) {
            return false;
        }
        const WorldVector2D commandSenderPosition = commandSender->GetPosition();
        auto tilePosition = TileLayerComponent::WorldToTile(
            {
                commandArgs.AsCoordinate(3, commandSenderPosition.x),
                commandArgs.AsCoordinate(4, commandSenderPosition.y)
            });
        std::optional<StructureInfo> structureInfoOptional = appContext_->GetStructureGeneratorManager()->Generate(
            tilePosition, structureResource);
        if (!structureInfoOptional.has_value()) {
            return false;
        }
        StructureInfo &structureInfo = structureInfoOptional.value();
        TileVector2D minPos = structureInfo.GetMinPosition();
        for (auto &[coord, resourceRef]: structureInfo.GetStructureMap()) {
            int localX = coord.x - minPos.x;
            int localY = coord.y - minPos.y;
            TileVector2D position =
                    tilePosition + TileVector2D(localX, localY);
            Chunk *chunk =
                    worldContext_->GetChunk(Chunk::TileCoordinatesToChunkVertexCoordinates(position));
            if (chunk == nullptr) {
                continue;
            }
            auto *tileResource =
                    appContext_->GetResourceLocator()->FindTile(resourceRef);
            if (tileResource == nullptr) {
                continue;
            }

            chunk->SetTile(
                Chunk::TileCoordinatesToChunkRelativeCoordinates(position),
                Tile::FromTileResource(appContext_, tileResource, resourceRef));
        }
        return true;
    }
    return false;
}
