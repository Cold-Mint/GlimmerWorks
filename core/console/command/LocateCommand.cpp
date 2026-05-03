//
// Created by coldmint on 2026/4/6.
//

#include "LocateCommand.h"

#include "core/world/WorldContext.h"
#include "fmt/xchar.h"

glimmer::LocateCommand::LocateCommand(AppContext *appContext) : Command(appContext) {
}

std::optional<TileVector2D> glimmer::LocateCommand::SearchBiomes(int tileX, const BiomesManager *biomesManager,
                                                                 ChunkGenerator *chunkGenerator,
                                                                 const std::string &targetBiomeId) {
    TileVector2D chunkCenter = Chunk::TileCoordinatesToChunkVertexCoordinates({tileX, 0}) + TileVector2D{
                                   HALF_CHUNK_SIZE, HALF_CHUNK_SIZE
                               };
    const int firstTileTerrainY = chunkGenerator->GetFirstTileTerrainY(tileX);
    for (int y = WORLD_MAX_Y - HALF_CHUNK_SIZE; y > WORLD_MIN_Y; y -= CHUNK_SIZE) {
        if (y > firstTileTerrainY) {
            //It is meaningless to judge the tile-based biological community of the sky.
            //判断天空的瓦片生物群系是无意义的。
            continue;
        }
        chunkCenter.y = y;
        float elevation = ChunkGenerator::GetElevation(y);
        BiomeResource *nowBiomeResource = biomesManager->FindBestBiome(chunkGenerator->GetHumidity(chunkCenter),
                                                                       chunkGenerator->GetTemperature(
                                                                           chunkCenter, elevation),
                                                                       chunkGenerator->GetWeirdness(chunkCenter),
                                                                       chunkGenerator->GetErosion(chunkCenter),
                                                                       elevation);
        if (nowBiomeResource == nullptr) {
            continue;
        }
        if (Resource::GenerateId(*nowBiomeResource) == targetBiomeId) {
            return chunkCenter;
        }
    }
    return std::nullopt;
}


void glimmer::LocateCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    suggestionsTree.AddChild("biome")->AddChild(BIOME_DYNAMIC_SUGGESTIONS_NAME);
}

std::string glimmer::LocateCommand::GetName() const {
    return LOCATE_COMMAND_NAME;
}

bool glimmer::LocateCommand::RequiresWorldContext() const {
    return true;
}

void glimmer::LocateCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
    strings.emplace_back("[biome:string]");
    strings.emplace_back("[biomeId:string]");
}

bool glimmer::LocateCommand::Execute(const CommandSender *commandSender, const CommandArgs commandArgs,
    const std::function<void(const std::string &text)> onMessage) {
  if (appContext_ == nullptr) {
        return false;
    }
    if (worldContext_ == nullptr) {
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
    if (type == "biome") {
        BiomesManager *biomesManager = appContext_->GetBiomesManager();
        if (biomesManager == nullptr) {
            return false;
        }
        auto resourceRefOptional = commandArgs.AsResourceRef(2, RESOURCE_TYPE_BIOME);
        if (!resourceRefOptional.has_value()) {
            return false;
        }
        ResourceRef &resourceRef = resourceRefOptional.value();
        BiomeResource *targetBiomeResource = biomesManager->Find(resourceRef.GetPackageId(),
                                                                 resourceRef.GetResourceKey());
        if (targetBiomeResource == nullptr) {
            return false;
        }
        ChunkGenerator *chunkGenerator = worldContext_->GetChunkGenerator();
        if (chunkGenerator == nullptr) {
            return false;
        }
        std::string targetBiomeID = Resource::GenerateId(*targetBiomeResource);
        const GameEntity::ID playerId = worldContext_->GetPlayerEntity();
        if (WorldContext::IsEmptyEntityId(playerId)) {
            return false;
        }
        const Transform2DComponent *transform2dComponent = worldContext_->GetComponent<Transform2DComponent>(playerId);
        if (transform2dComponent == nullptr) {
            return false;
        }
        TileVector2D position = TileLayerComponent::WorldToTile(transform2dComponent->GetPosition());
        int locateMaxRadiusSearchChunks = appContext_->GetConfig()->command.locateMaxRadiusSearchChunks;
        std::optional<TileVector2D> target = std::nullopt;
        for (int searchRadius = 0; searchRadius < locateMaxRadiusSearchChunks; searchRadius++) {
            if (searchRadius == 0) {
                target = SearchBiomes(position.x, biomesManager, chunkGenerator, targetBiomeID);
                if (target.has_value()) {
                    break;
                }
                continue;
            }
            const int distance = searchRadius * CHUNK_SIZE;
            target = SearchBiomes(position.x + distance, biomesManager, chunkGenerator, targetBiomeID);
            if (target.has_value()) {
                break;
            }
            target = SearchBiomes(position.x - distance, biomesManager, chunkGenerator, targetBiomeID);
            if (target.has_value()) {
                break;
            }
        }
        if (target.has_value()) {
            onMessage(fmt::format(
                fmt::runtime(appContext_->GetLangsResources()->biomeHasFound), targetBiomeID, target.value().x,
                target.value().y
            ));
            return true;
        }
        onMessage(fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->noBiomeWasFound), targetBiomeID
        ));
        return false;
    }
    return false;
}
