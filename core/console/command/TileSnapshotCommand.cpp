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
#if  !defined(NDEBUG)
#include "TileSnapshotCommand.h"

#include "core/LangsResources.h"
#include "core/math/CoordinateTransformer.h"
#include "core/world/WorldContext.h"
#include "core/scene/AppContext.h"
#include "fmt/xchar.h"

void glimmer::TileSnapshotCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
    if (suggestionsTree == nullptr)
    {
        return;
    }
    suggestionsTree->AddChild("inspector");
    suggestionsTree->AddChild("info")->AddChild(X_DYNAMIC_SUGGESTIONS_NAME)->AddChild(Y_DYNAMIC_SUGGESTIONS_NAME);
}

glimmer::TileSnapshotCommand::TileSnapshotCommand(AppContext* appContext)
    : Command(appContext)
{
}

std::string glimmer::TileSnapshotCommand::GetName() const
{
    return TILE_SNAPSHOT_COMMAND_NAME;
}

bool glimmer::TileSnapshotCommand::RequiresWorldContext() const
{
    return true;
}

void glimmer::TileSnapshotCommand::PutCommandStructure(const CommandArgs* commandArgs,
                                                       std::vector<std::string>* strings)
{
    if (commandArgs == nullptr || strings == nullptr)
    {
        return;
    }
    strings->emplace_back("[operation:string]");
    if (commandArgs->GetSize() >= 2)
    {
        std::string operation = commandArgs->AsString(1);
        if (operation == "info")
        {
            strings->emplace_back("[x:int]");
            strings->emplace_back("[y:int]");
        }
    }
}

bool glimmer::TileSnapshotCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
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
    const LangsResources* langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr)
    {
        return false;
    }
    const int size = commandArgs->GetSize();
    if (size < 2)
    {
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }
    std::string operation = commandArgs->AsString(1);
    if (operation == "inspector")
    {
        CommandHookManager* commandHookManager = appContext_->GetCommandHookManager();
        if (commandHookManager == nullptr)
        {
            onMessageRef(langsResources->cmdHookManagerNotFound);
            return false;
        }
        if (commandHookManager->Contains(TILE_SNAPSHOT_INSPECTOR_ID))
        {
            if (commandHookManager->Unregister(TILE_SNAPSHOT_INSPECTOR_ID))
            {
                onMessageRef(langsResources->tileSnapshotInspectorDisable);
                return true;
            }
            onMessageRef(langsResources->tileSnapshotInspectorDisableFail);
        }
        else
        {
            auto commandHookEntry = std::make_unique<CommandHookEntry>();
            commandHookEntry->hookId = TILE_SNAPSHOT_INSPECTOR_ID;
            commandHookEntry->scope = CommandHookScope::SESSION;
            commandHookEntry->code = SDL_BUTTON_LEFT;
            commandHookEntry->command = TILE_SNAPSHOT_COMMAND_NAME + " info ~ ~";
            commandHookEntry->eventType = SDL_EVENT_MOUSE_BUTTON_DOWN;
            if (commandHookManager->Register(std::move(commandHookEntry)))
            {
                onMessageRef(langsResources->tileSnapshotInspectorEnable);
                return true;
            }
            onMessageRef(langsResources->tileSnapshotInspectorEnableFail);
        }
    }
    if (operation == "info")
    {
        if (size < 4)
        {
            onMessageRef(fmt::format(
                fmt::runtime(langsResources->insufficientParameterLength),
                4, size));
            return false;
        }
        const WorldVector2D commandSenderPosition = commandSender->GetPosition();
        const TileVector2D tileVector2D = CoordinateTransformer::WorldToTile(WorldVector2D(
            commandArgs->AsCoordinate(2, commandSenderPosition.x),
            commandArgs->AsCoordinate(
                3, commandSenderPosition.y)));
        const auto chunkVertex = Chunk::TileCoordinatesToChunkVertexCoordinates(tileVector2D);
         Chunk* chunk = worldContext_->GetChunk(chunkVertex);
        if (chunk == nullptr)
        {
            onMessageRef(fmt::format(fmt::runtime(langsResources->chunkHasNotBeenLoadedYet), tileVector2D.x,
                                     tileVector2D.y));
            return false;
        }
        const auto chunkRelative = Chunk::TileCoordinatesToChunkRelativeCoordinates(tileVector2D);
        std::vector<TileSnapshot*> tileSnapshotVectorPtr = chunk->GetTopVisibleTileSnapshots(
            Ground | BackGround, chunkRelative.y << CHUNK_SHIFT | chunkRelative.x);
        auto tileSnapshotSize = tileSnapshotVectorPtr.size();
        if (tileSnapshotSize == 0)
        {
            onMessageRef(fmt::format(fmt::runtime(langsResources->tileSnapshotsDoesNotExist), tileVector2D.x,
                                     tileVector2D.y));
            return false;
        }

        std::stringstream stringStream;
        for (int i = 0; i < tileSnapshotSize; i++)
        {
            TileSnapshot* tileSnapshot = tileSnapshotVectorPtr.at(i);
            if (tileSnapshot == nullptr)
            {
                continue;
            }
            const Tile* tile = tileSnapshot->GetTile();
            if (tile == nullptr)
            {
                continue;
            }
            const TileStateMessage* tileStateMessage = tileSnapshot->GetTileState();
            if (tileStateMessage == nullptr)
            {
                continue;
            }
            int32_t offsetX = tileStateMessage->offset().x();
            int32_t offsetY = tileStateMessage->offset().y();
            stringStream << fmt::format(fmt::runtime(langsResources->tileSnapshotInfo),
                                        static_cast<uint8_t>(tile->GetLayerType()),
                                        tileStateMessage->resourceref().packid() + ":" + tileStateMessage->resourceref()
                                        .resourcekey(),
                                        tileStateMessage->isplayerplaced(),
                                        offsetX,
                                        offsetY,
                                        tileStateMessage->width(),
                                        tileStateMessage->height());
            stringStream << '\n';
        }
        onMessageRef(stringStream.str());
        return true;
    }
    return false;
}
#endif
