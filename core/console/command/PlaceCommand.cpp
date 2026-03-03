//
// Created by coldmint on 2026/2/13.
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

glimmer::PlaceCommand::PlaceCommand(AppContext *ctx) : Command(ctx) {
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

bool glimmer::PlaceCommand::Execute(CommandArgs commandArgs,
                                    std::function<void(const std::string &text)> onMessage) {
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
        auto transform2dComponent = worldContext_->GetComponent<Transform2DComponent>(
            worldContext_->GetPlayerEntity());
        if (transform2dComponent == nullptr) {
            return false;
        }
        auto tilePosition = TileLayerComponent::WorldToTile(
            {
                commandArgs.AsCoordinate(3, transform2dComponent->GetPosition().x),
                commandArgs.AsCoordinate(4, transform2dComponent->GetPosition().y)
            });
        std::optional<StructureInfo> structureInfoOptional = appContext_->GetStructureGeneratorManager()->Generate(
            tilePosition, structureResource);
        if (!structureInfoOptional.has_value()) {
            return false;
        }
        StructureInfo &structureInfo = structureInfoOptional.value();
        std::unordered_set<Chunk *> dirtyChunks;
        TileVector2D minPos = structureInfo.GetMinPosition();
        for (auto &[coord, resourceRef]: structureInfo.GetStructureMap()) {
            int localX = coord.x - minPos.x;
            int localY = coord.y - minPos.y;
            TileVector2D position =
                    tilePosition + TileVector2D(localX, localY);
            Chunk *chunk =
                    Chunk::GetChunkByTileVector2D(worldContext_->GetAllChunks(), position);
            if (chunk == nullptr) {
                continue;
            }
            auto* tileResource =
                    appContext_->GetResourceLocator()->FindTile(resourceRef);
            if (tileResource == nullptr) {
                continue;
            }

            chunk->SetTile(
                Chunk::TileCoordinatesToChunkRelativeCoordinates(position),
                Tile::FromResourceRef(appContext_, tileResource, &resourceRef));

            dirtyChunks.insert(chunk);
        }
        for (Chunk *dirtyChunk: dirtyChunks) {
            ChunkPhysicsHelper::UpdatePhysicsBodyToChunk(worldContext_, dirtyChunk);
        }
        return true;
    }
    return false;
}
