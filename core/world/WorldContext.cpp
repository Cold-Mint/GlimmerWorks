//
// Created by Cold-Mint on 2025/10/24.
//

#include "WorldContext.h"

#include <utility>

#include "ChunkPhysicsHelper.h"
#include "TilePlacer.h"
#include "../Constants.h"
#include "../ecs/component/DebugDrawComponent.h"
#include "../ecs/system/GameStartSystem.h"
#include "../ecs/system/Transform2DSystem.h"
#include "../ecs/system/CameraSystem.h"
#include "../ecs/system/ChunkSystem.h"
#include "../ecs/system/DebugDrawBox2dSystem.h"
#include "../ecs/system/DebugDrawSystem.h"
#include "../ecs/system/DebugPanelSystem.h"
#include "../ecs/system/AndroidControlSystem.h"
#include "../ecs/system/DroppedItemSystem.h"
#include "../ecs/system/HotBarSystem.h"
#include "../ecs/system/PhysicsSystem.h"
#include "../ecs/system/PlayerControlSystem.h"
#include "../ecs/system/TileLayerSystem.h"
#include "../log/LogCat.h"
#include "../mod/ResourceLocator.h"

void glimmer::WorldContext::RemoveComponentInternal(GameEntity::ID id, GameComponent *comp) {
    const auto type = std::type_index(typeid(*comp));
    //Reduce componentCount
    // 减少 componentCount
    if (componentCount[type] > 0) --componentCount[type];

    //Check if the system is disabled
    // 检查系统是否停用
    if (componentCount[type] == 0) {
        for (auto &sys: activeSystems) {
            if (sys && sys->SupportsComponentType(type)) {
                sys->CheckActivation();
            }
        }
    }

    // Remove from entityComponents
    // 从 entityComponents 删除
    auto &components = entityComponents[id];
    std::erase_if(components,
                  [comp](const std::unique_ptr<GameComponent> &c) {
                      return c.get() == comp;
                  });
}


glimmer::Saves *glimmer::WorldContext::GetSaves() const {
    return saves;
}

bool glimmer::WorldContext::HasComponentType(const std::type_index &type) const {
    const auto it = componentCount.find(type);
    return it != componentCount.end() && it->second > 0;
}

void glimmer::WorldContext::SetPlayerEntity(GameEntity *player) {
    player_ = player;
}

glimmer::GameEntity *glimmer::WorldContext::GetPlayerEntity() const {
    return player_;
}

int glimmer::WorldContext::GetHeight(int x) {
    const auto it = heightMap_.find(x);
    if (it != heightMap_.end()) {
        LogCat::d("HeightMap cache hit for chunkX=", x);
        return it->second;
    }

    LogCat::d("HeightMap cache miss, generating new chunk at chunkX=", x);
    const float sampleX = static_cast<float>(x);

    // 1. 获取三层归一化噪声 (0.0 到 1.0)

    // a) 极低频: 大陆/大型岛屿掩码。
    const float continentNoise = (continentHeightMapNoise->GetNoise(sampleX, 0.0F) + 1.0F) * 0.5F;

    // b) 低频: 宏观地形 (山脉/高原)。
    const float mountainNoise = (mountainHeightMapNoise->GetNoise(sampleX, 0.0F) + 1.0F) * 0.5F;

    // c) 中频: 细节地形 (丘陵/平原起伏)。
    const float hillsNoise = (hillsNoiseHeightMapNoise->GetNoise(sampleX, 0.0F) + 1.0F) * 0.5F;


    // 2. 核心：创建大陆掩码和陆地起伏

    // A) 大陆掩码 (控制海洋和大陆板块)：使用 3 次方夸大低值，创建清晰的海洋边界。
    // 这层噪声确保了大陆/岛屿（大型陆地板块）的孤立性。
    const float continentMask = std::pow(continentNoise, 3.0F);

    // B) 组合陆地起伏噪声 (主要由山脉和丘陵构成)
    // landmassNoise 范围 [0, 1]
    float landmassNoise = (mountainNoise * MOUNTAIN_WEIGHT) + (hillsNoise * HILLS_WEIGHT);

    // C) 平滑山峰抬升 (生成高原和尖锐的山脉)
    float peakLift = 0.0F;
    if (landmassNoise > PEAK_LIFT_THRESHOLD) {
        // 只有在陆地噪声较高时，才计算额外的线性抬升，避免山峰顶部平坦。
        // liftFactor 范围 [0, 1]
        float liftFactor = (landmassNoise - PEAK_LIFT_THRESHOLD) / (1.0F - PEAK_LIFT_THRESHOLD);
        peakLift = liftFactor * MAX_PEAK_LIFT;
    }

    // D) 最终陆地噪声：基础起伏 + 额外山峰抬升
    float totalLandNoise = landmassNoise + peakLift;
    // 确保总噪声不会超出我们预设的最大抬升比例
    totalLandNoise = std::min(totalLandNoise, 1.0F + MAX_PEAK_LIFT);

    // E) 应用大陆掩码：只有大陆区域，陆地噪声才能抬升地形
    float combinedNoise = continentMask * totalLandNoise;


    // 3. 将总噪声映射到最终的世界高度
    // combinedNoise 的范围现在约为 [0, 1.3]，我们使用 TERRAIN_HEIGHT_RANGE 进行映射
    int height = WORLD_MIN_Y + BASE_HEIGHT_OFFSET +
                 static_cast<int>(combinedNoise * TERRAIN_HEIGHT_RANGE / (1.0F + MAX_PEAK_LIFT));


    // 5. 确保不超过世界最大高度
    height = std::min(height, MAX_LAND_HEIGHT);

    // 缓存并返回高度
    heightMap_[x] = height;
    LogCat::d("Generated and cached heights for chunkX=", x);
    return height;
}

std::unordered_map<TileVector2D, glimmer::Chunk *, glimmer::Vector2DIHash> *glimmer::WorldContext::GetAllChunks() {
    if (lastChunksVersion_ != chunksVersion_) {
        chunksCache_.clear();
        chunksCache_.reserve(chunks_.size());

        for (auto &[pos, chunkPtr]: chunks_) {
            chunksCache_[pos] = chunkPtr.get();
        }
        lastChunksVersion_ = chunksVersion_;
    }
    return &chunksCache_;
}


float glimmer::WorldContext::GetHumidity(const TileVector2D tileVector2d) {
    const auto it = humidityMap.find(tileVector2d);
    if (it != humidityMap.end()) {
        return it->second;
    }
    humidityMap[tileVector2d] = (humidityMapNoise->GetNoise(static_cast<float>(tileVector2d.x),
                                                            static_cast<float>(tileVector2d.y)) + 1) * 0.5F;
    return humidityMap[tileVector2d];
}

float glimmer::WorldContext::GetTemperature(TileVector2D tileVector2d, float elevation) {
    const auto it = temperatureMap.find(tileVector2d);
    if (it != temperatureMap.end()) {
        return it->second;
    }
    const float noiseTemp = (temperatureMapNoise->GetNoise(
                                 static_cast<float>(tileVector2d.x),
                                 static_cast<float>(tileVector2d.y)
                             ) + 1.0F) * 0.5F;
    const float altitudePenalty = std::pow(1.0f - elevation, 1.5f);
    const float temperature = noiseTemp * altitudePenalty;

    temperatureMap[tileVector2d] = temperature;
    return temperatureMap[tileVector2d];
}

float glimmer::WorldContext::GetWeirdness(const TileVector2D tileVector2d) {
    const auto it = weirdnessMap.find(tileVector2d);
    if (it != weirdnessMap.end()) {
        return it->second;
    }
    weirdnessMap[tileVector2d] = (weirdnessMapNoise->GetNoise(static_cast<float>(tileVector2d.x),
                                                              static_cast<float>(tileVector2d.y)) + 1) * 0.5F;
    return weirdnessMap[tileVector2d];
}

float glimmer::WorldContext::GetErosion(const TileVector2D tileVector2d) {
    const auto it = erosionMap.find(tileVector2d);
    if (it != erosionMap.end()) {
        return it->second;
    }
    erosionMap[tileVector2d] = (erosionMapNoise->GetNoise(static_cast<float>(tileVector2d.x),
                                                          static_cast<float>(tileVector2d.y)) + 1) * 0.5F;
    return erosionMap[tileVector2d];
}

float glimmer::WorldContext::GetElevation(const int height) {
    return static_cast<float>(height) / (WORLD_MAX_Y - WORLD_MIN_Y + WORLD_MIN_Y);
}

void glimmer::WorldContext::LoadChunkAt(const WorldVector2D &tileLayerPos, TileVector2D position) {
    TileVector2D relativeCoordinates = Chunk::TileCoordinatesToChunkRelativeCoordinates(position);
    if (relativeCoordinates.x != 0 || relativeCoordinates.y != 0) {
        LogCat::e("The loaded coordinates are not vertex coordinates.");
        assert(false);
    }
    if (chunks_.contains(position)) {
        return;
    }
    auto chunk = std::make_unique<Chunk>(position);
    std::array<std::array<ResourceRef, CHUNK_SIZE>, CHUNK_SIZE> tilesRef;
    ResourceRef airTileRef;
    airTileRef.SetResourceType(RESOURCE_TYPE_TILE);
    airTileRef.SetPackageId(RESOURCE_REF_CORE);
    airTileRef.SetSelfPackageId(RESOURCE_REF_CORE);
    airTileRef.SetResourceKey(TILE_ID_AIR);
    ResourceRef waterTileRef;
    waterTileRef.SetResourceType(RESOURCE_TYPE_TILE);
    waterTileRef.SetPackageId(RESOURCE_REF_CORE);
    waterTileRef.SetSelfPackageId(RESOURCE_REF_CORE);
    waterTileRef.SetResourceKey(TILE_ID_WATER);
    std::map<std::string, std::vector<TileVector2D> > biomeMap = {};
    std::map<std::string, BiomeResource *> biomeResourceMap = {};
    for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
        const int height = GetHeight(position.x + localX);
        for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
            TileVector2D localTile(localX, localY);
            TileVector2D worldTilePos = position + localTile;
            if (worldTilePos.y > height) {
                if (worldTilePos.y < SEA_LEVEL_HEIGHT) {
                    //water
                    //水
                    tilesRef[localX][localY] = waterTileRef;
                } else {
                    //sky
                    //天空
                    tilesRef[localX][localY] = airTileRef;
                }
                continue;
            }
            const float elevation = GetElevation(worldTilePos.y);
            const auto humidity = GetHumidity(worldTilePos);
            const auto temperature = GetTemperature(worldTilePos, elevation);
            const auto weirdness = GetWeirdness(worldTilePos);
            const auto erosion = GetErosion(worldTilePos);
            BiomeResource *biomeResource = appContext_->GetBiomesManager()->FindBestBiome(
                humidity, temperature, weirdness, erosion,
                elevation);
            if (biomeResource == nullptr) {
                LogCat::e("Failed to search for the biome.");
                return;
            }
            biomeMap[biomeResource->key].push_back(localTile);
            if (!biomeResourceMap.contains(biomeResource->key)) {
                biomeResourceMap[biomeResource->key] = biomeResource;
            }
        }
    }
    for (auto &[biomeKey, tilePositions]: biomeMap) {
        BiomeResource *biomeResource = biomeResourceMap[biomeKey];
        for (auto &tilePlacerRef: biomeResource->tilePlacerRefs) {
            std::string id = tilePlacerRef.id;
            TilePlacer *tilePlacer = appContext_->GetTilePlacerManager()->GetTilePlacer(id);
            if (tilePlacer == nullptr) {
                LogCat::w("Tile placer ", id, " does not exist.");
                continue;
            }
            if (!tilePlacer->PlaceTileId(appContext_, tilesRef, tilePlacerRef.tiles, tilePositions,
                                         JsonUtils::LoadJsonFromString(tilePlacerRef.config))) {
                LogCat::e("Placement ", id, " failed to execute. Block coordinates: x:", position.x, ",y:", position.y,
                          ".");
            }
        }
    }
    for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
        for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
            TileVector2D localTile(localX, localY);
            ResourceRef resourceRef = tilesRef[localX][localY];
            const std::optional<TileResource *> tileResource = appContext_->GetResourceLocator()->FindTile(resourceRef);
            TileResource *tileResourceValue = nullptr;
            if (tileResource.has_value()) {
                tileResourceValue = tileResource.value();
            } else {
                LogCat::w("Tile packageId=", resourceRef.GetPackageId(), ", key=", resourceRef.GetResourceKey(),
                          " does not exist.");
                tileResourceValue = appContext_->GetTileManager()->GetAir();
            }
            chunk->SetTile(localTile, Tile::FromResourceRef(appContext_, tileResourceValue));
        }
    }
    ChunkPhysicsHelper::AttachPhysicsBodyToChunk(worldId_, tileLayerPos, chunk.get());
    chunks_.insert({position, std::move(chunk)});
    chunksVersion_++;
}


void glimmer::WorldContext::UnloadChunkAt(TileVector2D position) {
    auto it = chunks_.find(position);
    if (it == chunks_.end()) {
        return;
    }
    ChunkPhysicsHelper::DetachPhysicsBodyToChunk(it->second.get());
    chunks_.erase(it);
    chunksVersion_++;
    LogCat::d("Unloaded chunk at position=", position.x, "y =", position.y);
}

bool glimmer::WorldContext::HasChunk(const TileVector2D position) const {
    return chunks_.contains(position);
}

bool glimmer::WorldContext::ChunkIsOutOfBounds(TileVector2D position) {
    if (position.y >= WORLD_MAX_Y ||
        position.y < WORLD_MIN_Y) {
        LogCat::w("Chunk position out of world bounds: x=", position.x, "y =", position.y);
        return true;
    }
    return false;
}


bool glimmer::WorldContext::HandleEvent(const SDL_Event &event) const {
    bool handled = false;
    for (auto &system: activeSystems) {
        if (system && system->HandleEvent(event)) {
            handled = true;
        }
    }
    return handled;
}

void glimmer::WorldContext::Update(const float delta) const {
    for (auto &system: activeSystems) {
        if (system) {
            system->Update(delta);
        }
    }
}

void glimmer::WorldContext::Render(SDL_Renderer *renderer) const {
    std::vector<GameSystem *> systemsToRender;
    systemsToRender.reserve(activeSystems.size());

    for (const auto &system: activeSystems) {
        if (system)
            systemsToRender.push_back(system.get());
    }

    //Sort by rendering order (lower layers at the bottom, upper layers at the top)
    //按渲染顺序排序（低层在底，高层在上）
    std::ranges::sort(systemsToRender,
                      [](GameSystem *a, GameSystem *b) {
                          return a->GetRenderOrder() < b->GetRenderOrder();
                      });

    for (GameSystem *system: systemsToRender) {
        system->Render(renderer);
    }
}


void glimmer::WorldContext::OnFrameStart() {
    std::vector<GameSystem *> toActivate;
    std::vector<GameSystem *> toDeactivate;

    // Traverse inactiveSystems to check if activation is needed
    // 遍历 inactiveSystems 检查是否需要激活
    for (auto &system: inactiveSystems) {
        if (system && system->CheckActivation()) {
            // Returning true indicates that the system is currently active
            // 返回 true 表示系统现在是激活的
            toActivate.push_back(system.get());
        }
    }

    // Traverse activeSystems to check if it needs to be disabled
    // 遍历 activeSystems 检查是否需要停用
    for (auto &system: activeSystems) {
        if (system && !system->CheckActivation()) {
            // Returning false indicates that the system is not activated at present
            // 返回 false 表示系统现在未激活
            toDeactivate.push_back(system.get());
        }
    }

    // Batch mobile activation system
    // 批量移动激活系统
    for (auto *sys: toActivate) {
        auto it = std::ranges::find_if(inactiveSystems,
                                       [sys](auto &s) { return s.get() == sys; });
        if (it != inactiveSystems.end()) {
            activeSystems.push_back(std::move(*it));
            inactiveSystems.erase(it);
        }
    }

    // Batch mobile deactivation of the system
    // 批量移动停用系统
    for (auto *sys: toDeactivate) {
        auto it = std::ranges::find_if(activeSystems,
                                       [sys](auto &s) { return s.get() == sys; });
        if (it != activeSystems.end()) {
            inactiveSystems.push_back(std::move(*it));
            activeSystems.erase(it);
        }
    }
}

void glimmer::WorldContext::InitSystem(AppContext *appContext) {
    allowRegisterSystem = true;
    RegisterSystem(std::make_unique<GameStartSystem>(appContext, this));
    RegisterSystem(std::make_unique<Transform2DSystem>(appContext, this));
    RegisterSystem(std::make_unique<CameraSystem>(appContext, this));
    RegisterSystem(std::make_unique<PlayerControlSystem>(appContext, this));
    RegisterSystem(std::make_unique<TileLayerSystem>(appContext, this));
    RegisterSystem(std::make_unique<ChunkSystem>(appContext, this));
    RegisterSystem(std::make_unique<PhysicsSystem>(appContext, this));
    RegisterSystem(std::make_unique<HotBarSystem>(appContext, this));
    RegisterSystem(std::make_unique<DroppedItemSystem>(appContext, this));
#ifdef __ANDROID__
    RegisterSystem(std::make_unique<AndroidControlSystem>(appContext, this));
#endif
#if  !defined(NDEBUG)
    RegisterSystem(std::make_unique<DebugDrawSystem>(appContext, this));
    RegisterSystem(std::make_unique<DebugDrawBox2dSystem>(appContext, this));
    RegisterSystem(std::make_unique<DebugPanelSystem>(appContext, this));
#endif
    allowRegisterSystem = false;
}

void glimmer::WorldContext::SetCameraPosition(Transform2DComponent *worldPositionComponent) {
    cameraTransform2D_ = worldPositionComponent;
}

void glimmer::WorldContext::SetHotBarComponent(HotBarComponent *hotbarComponent) {
    hotBarComponent_ = hotbarComponent;
}

void glimmer::WorldContext::SetCameraComponent(CameraComponent *cameraComponent) {
    cameraComponent_ = cameraComponent;
}

glimmer::CameraComponent *glimmer::WorldContext::GetCameraComponent() const {
    return cameraComponent_;
}

glimmer::Transform2DComponent *glimmer::WorldContext::GetCameraTransform2D() const {
    return cameraTransform2D_;
}

glimmer::HotBarComponent *glimmer::WorldContext::GetHotBarComponent() const {
    return hotBarComponent_;
}


void glimmer::WorldContext::RegisterSystem(std::unique_ptr<GameSystem> system) {
    if (allowRegisterSystem) {
        LogCat::i("Registered system: ", system->GetName());
        inactiveSystems.push_back(std::move(system));
    } else {
        LogCat::e("WorldContext is not allowed to register system");
    }
}


glimmer::GameEntity *glimmer::WorldContext::CreateEntity() {
    const auto newId = static_cast<GameEntity::ID>(entities.size());
    auto entity = std::make_unique<GameEntity>(newId);

    LogCat::d("Creating new entity, ID = ", newId);

    entities.push_back(std::move(entity));
    entityMap[newId] = entities.back().get();

    LogCat::i("Entity registered successfully, total entities = ", entities.size());

    return entities.back().get();
}


glimmer::GameEntity *glimmer::WorldContext::GetEntity(const GameEntity::ID id) {
    const auto it = entityMap.find(id);
    return it != entityMap.end() ? it->second : nullptr;
}

void glimmer::WorldContext::RemoveEntity(GameEntity::ID id) {
    LogCat::d("Attempting to remove entity ID = ", id);
    auto entityIt = entityMap.find(id);
    if (entityIt == entityMap.end()) {
        LogCat::w("Entity ID ", id, " not found, skipping removal.");
        return;
    }

    auto compIt = entityComponents.find(id);
    if (compIt != entityComponents.end()) {
        auto &components = compIt->second;

        for (auto &comp: components) {
            RemoveComponentInternal(id, comp.get());
        }

        entityComponents.erase(compIt);
        LogCat::d("All components of Entity ID = ", id, " have been removed.");
    }

    // Remove the entity record
    // 移除实体记录
    entityMap.erase(id);

    auto it = std::ranges::find_if(entities, [id](auto &ent) {
        return ent && ent->GetID() == id;
    });
    if (it != entities.end()) {
        entities.erase(it);
    }

    LogCat::i("Entity ID ", id, " successfully removed. Remaining entities = ", entities.size());
}

int glimmer::WorldContext::GetSeed() const {
    return seed;
}

b2WorldId glimmer::WorldContext::GetWorldId() const {
    return worldId_;
}
