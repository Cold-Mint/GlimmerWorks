//
// Created by Cold-Mint on 2026/2/13.
//

#include "PlaceCommand.h"
#include "core/scene/AppContext.h"
#include "core/world/WorldContext.h"
#include "core/world/generator/ChunkPhysicsHelper.h"
#include "fmt/format.h"


void glimmer::PlaceCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
    if (suggestionsTree == nullptr)
    {
        return;
    }
    suggestionsTree->AddChild("structure")->AddChild(STRUCTURE_DYNAMIC_SUGGESTIONS_NAME)->
                     AddChild(X_DYNAMIC_SUGGESTIONS_NAME)->AddChild(Y_DYNAMIC_SUGGESTIONS_NAME);
}

glimmer::PlaceCommand::PlaceCommand(AppContext* appContext) : Command(appContext)
{
}

std::string glimmer::PlaceCommand::GetName() const
{
    return PLACE_COMMAND_NAME;
}

void glimmer::PlaceCommand::PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
{
    if (strings == nullptr)
    {
        return;
    }
    strings->emplace_back("[structure:string]");
    strings->emplace_back("[structureId:string]");
    strings->emplace_back("[x:int]");
    strings->emplace_back("[y:int]");
}

bool glimmer::PlaceCommand::RequiresWorldContext() const
{
    return true;
}

bool glimmer::PlaceCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                    const std::function<void(const std::string& text)>* onMessage)
{
    if (appContext_ == nullptr || commandArgs == nullptr || onMessage == nullptr)
    {
        return false;
    }
    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    if (worldContext_ == nullptr)
    {
        onMessageRef(appContext_->GetLangsResources()->worldContextIsNull);
        return false;
    }
    const size_t size = commandArgs->GetSize();
    if (size < 5)
    {
        onMessageRef(fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->insufficientParameterLength),
            5, size));
        return false;
    }
    std::string type = commandArgs->AsString(1);
    if (type == "structure")
    {
        auto structureId = commandArgs->AsResourceRef(2, RESOURCE_STRUCTURE);
        IStructureResource* structureResource = appContext_->GetStructureManager()->Find(
            structureId->GetPackageId(), structureId->GetResourceKey());
        if (structureResource == nullptr)
        {
            return false;
        }
        const WorldVector2D commandSenderPosition = commandSender->GetPosition();
        auto tilePosition = TileLayerComponent::WorldToTile(
            {
                commandArgs->AsCoordinate(3, commandSenderPosition.x),
                commandArgs->AsCoordinate(4, commandSenderPosition.y)
            });
        std::optional<StructureInfo> structureInfoOptional = appContext_->GetStructureGeneratorManager()->Generate(
            tilePosition, structureResource);
        if (!structureInfoOptional.has_value())
        {
            return false;
        }
        TileInstancePool* tileInstancePool = worldContext_->GetTileInstancePool();
        if (tileInstancePool == nullptr)
        {
            return false;
        }
        StructureInfo& structureInfo = structureInfoOptional.value();
        const int baseX = tilePosition.x;
        const int baseY = tilePosition.y;

        Chunk* currentChunk = nullptr;
        Vector2DI currentChunkCoord = {INT_MIN, INT_MIN};
        for (auto& [tileLayerType, tileMap] : structureInfo.GetStructureMap())
        {
            for (auto& [coord, resourceRef] : tileMap)
            {
                const int worldX = baseX + coord.x;
                const int worldY = baseY + coord.y;
                const int chunkX = worldX & ~CHUNK_MASK;
                const int chunkY = worldY & ~CHUNK_MASK;
                const int relativeX = worldX & CHUNK_MASK;
                const int relativeY = worldY & CHUNK_MASK;
                Vector2DI chunkCoord{chunkX, chunkY};
                if (chunkCoord != currentChunkCoord)
                {
                    currentChunkCoord = chunkCoord;
                    currentChunk = worldContext_->GetChunk(
                        Chunk::TileCoordinatesToChunkVertexCoordinates(chunkCoord)
                    );
                }
                if (currentChunk == nullptr)
                {
                    continue;
                }
                auto* tileResource = appContext_->GetResourceLocator()->FindTileRaw(&resourceRef);
                if (tileResource == nullptr)
                {
                    continue;
                }
                for (int x = 0; x < tileResource->tileWidth; x++)
                {
                    for (int y = 0; y < tileResource->tileHeight; y++)
                    {
                        const int index = relativeY << CHUNK_SHIFT | relativeX;
                        TileStateMessage* tileStateMessage = currentChunk->GetTileState(tileLayerType, index);
                        tileStateMessage->set_width(tileResource->tileWidth);
                        tileStateMessage->set_height(tileResource->tileHeight);
                        tileStateMessage->mutable_offset()->set_x(x);
                        tileStateMessage->mutable_offset()->set_y(y);
                        resourceRef.WriteResourceRefMessage(*tileStateMessage->mutable_resourceref());
                        currentChunk->CommitTileState(tileLayerType, index, false);
                    }
                }
            }
        }

        return true;
    }
    return false;
}
