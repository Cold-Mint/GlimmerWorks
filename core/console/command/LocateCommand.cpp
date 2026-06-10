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
#include "LocateCommand.h"

#include "core/math/CoordinateTransformer.h"
#include "core/world/WorldContext.h"
#include "fmt/xchar.h"

glimmer::LocateCommand::LocateCommand(AppContext *appContext) : Command(appContext) {
}

std::optional<glimmer::TileVector2D> glimmer::LocateCommand::SearchBiomes(int tileX, const BiomesManager *biomesManager,
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
                                                                       elevation, ChunkGenerator::GetSurfaceProximity(
                                                                           firstTileTerrainY, y));
        if (nowBiomeResource == nullptr) {
            continue;
        }
        if (Resource::GenerateId(*nowBiomeResource) == targetBiomeId) {
            return chunkCenter;
        }
    }
    return std::nullopt;
}

void glimmer::LocateCommand::InitSuggestions(NodeTree<std::string> *suggestionsTree) {
    if (suggestionsTree == nullptr) {
        return;
    }
    suggestionsTree->AddChild("biome")->AddChild(BIOME_DYNAMIC_SUGGESTIONS_NAME);
}

std::string glimmer::LocateCommand::GetName() const {
    return LOCATE_COMMAND_NAME;
}

bool glimmer::LocateCommand::RequiresWorldContext() const {
    return true;
}

void glimmer::LocateCommand::PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) {
    if (strings == nullptr) {
        return;
    }
    strings->emplace_back("[biome:string]");
    strings->emplace_back("[biomeId:string]");
}


bool glimmer::LocateCommand::Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
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
    std::string type = commandArgs->AsString(1);
    if (type == "biome") {
        BiomesManager *biomesManager = appContext_->GetBiomesManager();
        if (biomesManager == nullptr) {
            return false;
        }
        auto resourceRefOptional = commandArgs->AsResourceRef(2, RESOURCE_BIOME);
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
        EntityShortCut *entityShortCut = worldContext_->GetEntityShortCut();
        if (entityShortCut == nullptr)
        {
            return false;
        }
        EntityManager *entityManager = worldContext_->GetEntityManager();
        if (entityManager == nullptr)
        {
            return false;
        }
        auto playerId = entityShortCut->GetPlayer();
        if (WorldContext::IsEmptyEntityId(playerId))
        {
            return false;
        }
        const Transform2DComponent *transform2dComponent = entityManager->GetComponent<Transform2DComponent>(playerId);
        if (transform2dComponent == nullptr) {
            return false;
        }
        TileVector2D position = CoordinateTransformer::WorldToTile(transform2dComponent->GetPosition());
        uint16_t locateMaxRadiusSearchChunks = appContext_->GetConfig()->command.locateMaxRadiusSearchChunks;
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
            onMessageRef(fmt::format(
                fmt::runtime(appContext_->GetLangsResources()->biomeHasFound), targetBiomeID, target.value().x,
                target.value().y
            ));
            return true;
        }
        onMessageRef(fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->noBiomeWasFound), targetBiomeID
        ));
        return false;
    }
    return false;
}
