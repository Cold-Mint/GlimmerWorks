//
// Created by Cold-Mint on 2026/5/24.
//
#if  !defined(NDEBUG)
#include "TileSnapshotCommand.h"

#include "core/LangsResources.h"
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
        onMessageRef("come1");
        if (size < 4)
        {
            onMessageRef(fmt::format(
                fmt::runtime(langsResources->insufficientParameterLength),
                4, size));
            onMessageRef("come2");
            return false;
        }
        const WorldVector2D commandSenderPosition = commandSender->GetPosition();
        const TileVector2D tileVector2D = TileLayerComponent::WorldToTile(WorldVector2D(
            commandArgs->AsCoordinate(2, commandSenderPosition.x),
            commandArgs->AsCoordinate(
                3, commandSenderPosition.y)));
        const auto chunkVertex = Chunk::TileCoordinatesToChunkVertexCoordinates(tileVector2D);
        const Chunk* chunk = worldContext_->GetChunk(chunkVertex);
        if (chunk == nullptr)
        {
            onMessageRef(fmt::format(fmt::runtime(langsResources->chunkHasNotBeenLoadedYet), tileVector2D.x,
                                     tileVector2D.y));
            onMessageRef("come3");
            return false;
        }
        const auto chunkRelative = Chunk::TileCoordinatesToChunkRelativeCoordinates(tileVector2D);
        std::unique_ptr<std::vector<TileSnapshot>> tileSnapshotVectorPtr = chunk->GetTopVisibleTileSnapshots(
            Ground | BackGround, chunkRelative.y << CHUNK_SHIFT | chunkRelative.x);
        if (tileSnapshotVectorPtr == nullptr)
        {
            onMessageRef(fmt::format(fmt::runtime(langsResources->tileSnapshotsDoesNotExist), tileVector2D.x,
                                     tileVector2D.y));
            onMessageRef("come4");
            return false;
        }
        std::vector<TileSnapshot>* tileSnapshotVector = tileSnapshotVectorPtr.get();
        auto tileSnapshotSize = tileSnapshotVector->size();
        if (tileSnapshotSize == 0)
        {
            onMessageRef(fmt::format(fmt::runtime(langsResources->tileSnapshotsDoesNotExist), tileVector2D.x,
                                     tileVector2D.y));
            onMessageRef("come5");
            return false;
        }

        std::stringstream stringStream;
        for (int i = 0; i < tileSnapshotSize; i++)
        {
            TileSnapshot& tileSnapshot = tileSnapshotVector->at(i);
            const Tile* tile = tileSnapshot.GetTile();
            if (tile == nullptr)
            {
                continue;
            }
            const TileStateMessage* tileStateMessage = tileSnapshot.GetTileState();
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
        onMessageRef("come6");
        return true;
    }
    return false;
}
#endif
