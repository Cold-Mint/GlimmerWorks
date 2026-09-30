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
#include "core/world/Dimension.h"
#include "core/world/WorldContext.h"
#include "core/world/generator/Chunk.h"
#include "core/world/generator/TerrainMath.h"
#include "fmt/xchar.h"

glimmer::LocateCommand::LocateCommand(AppContext *appContext) : Command(appContext) {
}

void glimmer::LocateCommand::InitSuggestions(NodeTree<std::string> *suggestionsTree) {
    if (suggestionsTree == nullptr) {
        return;
    }
    suggestionsTree->AddChild("biome")->AddChild(BIOME_DYNAMIC_SUGGESTIONS_NAME);
}

std::optional<glimmer::TileVector2D> glimmer::LocateCommand::SearchBiomes(int tileX, const ResourceRef &dimension,
                                                                          const BiomeRegistry *biomeRegistry,
                                                                          ClimateSampler *climateSampler,
                                                                          const std::string &targetBiomeId) {
    if (climateSampler == nullptr || biomeRegistry == nullptr) {
        return std::nullopt;
    }
    TileVector2D chunkCenter = Chunk::TileCoordinatesToChunkVertexCoordinates({tileX, 0}) + TileVector2D{
                                   HALF_CHUNK_SIZE, HALF_CHUNK_SIZE
                               };
    const int firstTileTerrainY = climateSampler->GetFirstTileTerrainY(tileX);
    for (int y = WORLD_MAX_Y - HALF_CHUNK_SIZE; y > WORLD_MIN_Y; y -= CHUNK_SIZE) {
        if (y > firstTileTerrainY) {
            continue;
        }
        chunkCenter.y = y;
        float elevation = TerrainMath::GetElevation(y);
        const BiomeResource *nowBiomeResource = biomeRegistry->FindBestBiome(
            dimension, climateSampler->GetHumidity(chunkCenter),
            climateSampler->GetTemperature(chunkCenter, elevation),
            climateSampler->GetWeirdness(chunkCenter),
            climateSampler->GetErosion(chunkCenter),
            elevation,
            TerrainMath::GetSurfaceProximity(firstTileTerrainY, y));
        if (nowBiomeResource == nullptr) {
            continue;
        }
        if (Resource::GenerateId(*nowBiomeResource) == targetBiomeId) {
            return chunkCenter;
        }
    }
    return std::nullopt;
}


bool glimmer::LocateCommand::ExecuteBiome(const CommandArgs *commandArgs,
                                          const std::function<void(const std::string &text)> &onMessageRef,
                                          const AppContext *appContext,
                                          const WorldContext *worldContext) {
    const ModContext *modContext = appContext->GetModContext();
    if (modContext == nullptr) {
        return false;
    }
    BiomeRegistry *biomeRegistry = modContext->GetBiomeRegistry();
    if (biomeRegistry == nullptr) {
        return false;
    }
    const auto biomeResourceRefOptional = commandArgs->AsResourceRef(2, RESOURCE_BIOME);
    if (!biomeResourceRefOptional.has_value()) {
        return false;
    }
    const ResourceRef &biomeResourceRef = biomeResourceRefOptional.value();
    const BiomeResource *targetBiomeResource = biomeRegistry->Find(biomeResourceRef.GetPackageId(),
                                                                   biomeResourceRef.GetResourceKey());
    if (targetBiomeResource == nullptr) {
        return false;
    }
    TerrainGenerator *terrainGenerator = worldContext->GetTerrainGenerator();
    if (terrainGenerator == nullptr) {
        return false;
    }
    ClimateSampler *climateSampler = terrainGenerator->GetMutableClimateSampler();
    if (climateSampler == nullptr) {
        return false;
    }
    std::string targetBiomeID = Resource::GenerateId(*targetBiomeResource);
    EntityShortCut *entityShortCut = worldContext->GetEntityShortCut();
    if (entityShortCut == nullptr) {
        return false;
    }
    EntityManager *entityManager = worldContext->GetEntityManager();
    if (entityManager == nullptr) {
        return false;
    }
    Dimension *dimension = worldContext->GetDimension();
    if (dimension == nullptr) {
        return false;
    }
    const DimensionResource *dimensionResource =
            dimension->GetDimensionResource();
    if (dimensionResource == nullptr) {
        return false;
    }
    ResourceRef dimensionResourceRef = ResourceRef();
    dimensionResourceRef.SetSelfPackageId(dimensionResource->packId);
    dimensionResourceRef.SetResourceKey(dimensionResource->resourceId);
    dimensionResourceRef.SetResourceType(RESOURCE_DIMENSION);
    auto playerId = entityShortCut->GetPlayer();
    if (WorldContext::IsEmptyEntityId(playerId)) {
        return false;
    }
    const Transform2DComponent *transform2dComponent = entityManager->GetComponent<
        Transform2DComponent>(playerId);
    if (transform2dComponent == nullptr) {
        return false;
    }
    TileVector2D position = CoordinateTransformer::WorldToTile(transform2dComponent->GetPosition());
    uint16_t locateMaxRadiusSearchChunks = appContext->GetConfig()->command.locateMaxRadiusSearchChunks;
    std::optional<TileVector2D> target = SearchBiomeInRadius(
        position, dimensionResourceRef, biomeRegistry, climateSampler, targetBiomeID, locateMaxRadiusSearchChunks);
    if (target.has_value()) {
        onMessageRef(fmt::format(
            fmt::runtime(appContext->GetLangsResources()->biomeHasFound), targetBiomeID, target.value().x,
            target.value().y
        ));
        return true;
    }
    onMessageRef(fmt::format(
        fmt::runtime(appContext->GetLangsResources()->noBiomeWasFound), targetBiomeID
    ));
    return false;
}

std::optional<glimmer::TileVector2D> glimmer::LocateCommand::SearchBiomeInRadius(const TileVector2D &position,
    const ResourceRef &dimension, const BiomeRegistry *biomeRegistry, ClimateSampler *climateSampler,
    const std::string &targetBiomeId, uint16_t maxRadiusChunks) {
    auto target = SearchBiomes(position.x, dimension, biomeRegistry, climateSampler, targetBiomeId);
    if (target.has_value()) {
        return target;
    }
    for (int searchRadius = 1; searchRadius < maxRadiusChunks; searchRadius++) {
        const int distance = searchRadius * CHUNK_SIZE;
        target = SearchBiomes(position.x + distance, dimension, biomeRegistry, climateSampler, targetBiomeId);
        if (target.has_value()) {
            return target;
        }
        target = SearchBiomes(position.x - distance, dimension, biomeRegistry, climateSampler, targetBiomeId);
        if (target.has_value()) {
            return target;
        }
    }
    return std::nullopt;
}

const std::string &glimmer::LocateCommand::GetName() const {
    return LOCATE_COMMAND_NAME;
}

bool glimmer::LocateCommand::RequiresWorldContext() const {
    return true;
}

bool glimmer::LocateCommand::RequiresCheatEnabled() const {
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
    AppContext *appContext = GetAppContext();
    WorldContext *worldContext = GetWorldContext();
    if (appContext == nullptr || commandArgs == nullptr || onMessage == nullptr) {
        return false;
    }
    const std::function<void(const std::string &text)> &onMessageRef = *onMessage;
    if (worldContext == nullptr) {
        onMessageRef(appContext->GetLangsResources()->worldContextIsNull);
        return false;
    }
    const size_t size = commandArgs->GetSize();
    if (size < 3) {
        onMessageRef(fmt::format(
            fmt::runtime(appContext->GetLangsResources()->insufficientParameterLength),
            3, size));
        return false;
    }
    std::string type = commandArgs->AsString(1);
    if (type == "biome") {
        return ExecuteBiome(commandArgs, onMessageRef, appContext, worldContext);
    }
    return false;
}
