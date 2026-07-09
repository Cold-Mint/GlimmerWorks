/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "PlaceCommand.h"

#include "core/math/CoordinateTransformer.h"
#include "core/scene/AppContext.h"
#include "core/world/WorldContext.h"
#include "core/world/ChunkManager.h"
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

const std::string& glimmer::PlaceCommand::GetName() const
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

void glimmer::PlaceCommand::PlaceTileAt(Chunk* chunk, TileLayerType tileLayerType, int index,
                                        const ResourceRef& resourceRef, const TileResource* tileResource, int x, int y)
{
    TileStateMessage* tileStateMessage = chunk->GetTileState(tileLayerType, index);
    if (tileStateMessage == nullptr)
    {
        return;
    }
    tileStateMessage->set_width(tileResource->tileWidth);
    tileStateMessage->set_height(tileResource->tileHeight);
    tileStateMessage->set_placesource(PLACE_SOURCE_CONSOLE);
    tileStateMessage->mutable_offset()->set_x(x);
    tileStateMessage->mutable_offset()->set_y(y);
    resourceRef.WriteResourceRefMessage(*tileStateMessage->mutable_resourceref());
    chunk->CommitTileState(BreakSource::Console, tileLayerType, index, false);
}

void glimmer::PlaceCommand::PlaceTileAtWithSize(Chunk* chunk, TileLayerType tileLayerType, int index,
                                                const ResourceRef& resourceRef, const TileResource* tileResource)
{
    for (int x = 0; x < tileResource->tileWidth; x++)
    {
        for (int y = 0; y < tileResource->tileHeight; y++)
        {
            PlaceTileAt(chunk, tileLayerType, index, resourceRef, tileResource, x, y);
        }
    }
}

bool glimmer::PlaceCommand::ExecuteStructure(const CommandArgs* commandArgs, const CommandSender* commandSender,
                                             WorldContext* worldContext)
{
    if (worldContext == nullptr)
    {
        return false;
    }
    const AppContext* appContext = worldContext->GetAppContext();
    if (appContext == nullptr)
    {
        return false;
    }
    auto structureId = commandArgs->AsResourceRef(2, RESOURCE_STRUCTURE);
    IStructureResource* structureResource = appContext->GetModContext()->GetStructureManager()->Find(
        structureId->GetPackageId(), structureId->GetResourceKey());
    if (structureResource == nullptr)
    {
        return false;
    }
    const WorldVector2D commandSenderPosition = commandSender->GetPosition();
    auto tilePosition = CoordinateTransformer::WorldToTile(
        {
            commandArgs->AsCoordinate(3, commandSenderPosition.x),
            commandArgs->AsCoordinate(4, commandSenderPosition.y)
        });
    std::optional<StructureInfo> structureInfoOptional = appContext->GetModContext()->GetStructureGeneratorManager()->Generate(
        worldContext,
        tilePosition, structureResource);
    if (!structureInfoOptional.has_value())
    {
        return false;
    }
    TileInstancePool* tileInstancePool = worldContext->GetTileInstancePool();
    if (tileInstancePool == nullptr)
    {
        return false;
    }
    StructureInfo& structureInfo = structureInfoOptional.value();
    const int baseX = tilePosition.x;
    const int baseY = tilePosition.y;

    Chunk* currentChunk = nullptr;
    TileVector2D currentChunkCoord = {INT_MIN, INT_MIN};
    ChunkManager* chunkManager = worldContext->GetChunkManager();
    if (chunkManager == nullptr)
    {
        return false;
    }
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
            if (TileVector2D chunkCoord{chunkX, chunkY}; chunkCoord != currentChunkCoord)
            {
                currentChunkCoord = chunkCoord;
                currentChunk = chunkManager->GetChunk(
                    Chunk::TileCoordinatesToChunkVertexCoordinates(chunkCoord)
                );
            }
            if (currentChunk == nullptr)
            {
                continue;
            }
            auto tileResource = appContext->GetResourceLocator()->FindTileRaw(&resourceRef);
            if (tileResource == nullptr)
            {
                continue;
            }
            const int index = relativeY << CHUNK_SHIFT | relativeX;
            PlaceTileAtWithSize(currentChunk, tileLayerType, index, resourceRef, tileResource);
        }
    }

    return true;
}

bool glimmer::PlaceCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                    const std::function<void(const std::string& text)>* onMessage)
{
    WorldContext* worldContext = GetWorldContext();
    const AppContext* appContext = GetAppContext();
    if (appContext == nullptr || commandArgs == nullptr || onMessage == nullptr)
    {
        return false;
    }
    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    if (worldContext == nullptr)
    {
        onMessageRef(appContext->GetLangsResources()->worldContextIsNull);
        return false;
    }
    if (const size_t size = commandArgs->GetSize(); size < 5)
    {
        onMessageRef(fmt::format(
            fmt::runtime(appContext->GetLangsResources()->insufficientParameterLength),
            5, size));
        return false;
    }
    if (std::string type = commandArgs->AsString(1); type == "structure")
    {
        return ExecuteStructure(commandArgs, commandSender, worldContext);
    }
    return false;
}
