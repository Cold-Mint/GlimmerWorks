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
#include "../ecs/system/AutoPickSystem.h"
#include "../ecs/system/DiggingSystem.h"
#include "../ecs/system/DroppedItemSystem.h"
#include "../ecs/system/HotBarSystem.h"
#include "../ecs/system/ItemSlotSystem.h"
#include "../ecs/system/MagnetSystem.h"
#include "../ecs/system/PauseSystem.h"
#include "../ecs/system/PhysicsSystem.h"
#include "../ecs/system/PlayerControlSystem.h"
#include "../ecs/system/TileLayerSystem.h"
#include "../log/LogCat.h"
#include "../mod/ResourceLocator.h"
#include "../saves/Saves.h"
#include "../utils/Box2DUtils.h"
#include "box2d/box2d.h"
#include "core/ecs/component/AutoPickComponent.h"
#include "core/ecs/component/DroppedItemComponent.h"
#include "core/ecs/component/GuiTransform2DComponent.h"
#include "core/ecs/component/RigidBody2DComponent.h"
#include "core/ecs/component/HotBarComonent.h"
#include "core/ecs/component/ItemEditorComponent.h"
#include "core/ecs/component/MagnetComponent.h"
#include "core/ecs/component/MagneticComponent.h"
#include "core/ecs/component/PlayerControlComponent.h"
#include "core/ecs/system/ItemEditorSystem.h"
#include "core/utils/TimeUtils.h"

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

glimmer::GameEntity::ID glimmer::WorldContext::RegisterEntity(std::unique_ptr<GameEntity> entity) {
    entities.push_back(std::move(entity));
    GameEntity *lastEntity = entities.back().get();
    entityMap[lastEntity->GetID()] = lastEntity;
    return lastEntity->GetID();
}


void glimmer::WorldContext::UnRegisterEntity(const GameEntity::ID id) {
    entityMap.erase(id);
}

glimmer::WorldContext::~WorldContext() {
    activeSystems.clear();
    inactiveSystems.clear();
    entityComponents.clear();
    b2DestroyWorld(worldId_);
    worldId_ = b2_nullWorldId;
    for (const auto &command: appContext_->GetCommandManager()->GetCommands() | std::views::values) {
        if (command->RequiresWorldContext()) {
            command->UnBindWorldContext();
        }
    }
}


glimmer::GameEntity::ID glimmer::WorldContext::GetEntityIdIndex() const {
    return entityId_;
}

bool glimmer::WorldContext::IsRuning() const {
    return running;
}

std::vector<glimmer::GameComponent *> glimmer::WorldContext::GetAllComponents(const GameEntity::ID id) {
    auto &components = entityComponents[id];
    std::vector<GameComponent *> result;
    for (auto &gameComponent: components) {
        result.push_back(gameComponent.get());
    }
    return result;
}

std::vector<glimmer::GameSystem *> glimmer::WorldContext::GetAllActiveSystem() const {
    std::vector<GameSystem *> result;
    for (auto &activeSystem: activeSystems) {
        result.push_back(activeSystem.get());
    }
    return result;
}

void glimmer::WorldContext::SetRuning(const bool run) {
    running = run;
}

glimmer::Saves *glimmer::WorldContext::GetSaves() const {
    return saves_;
}

bool glimmer::WorldContext::HasComponentType(const std::type_index &type) const {
    const auto it = componentCount.find(type);
    return it != componentCount.end() && it->second > 0;
}

void glimmer::WorldContext::InitPlayer() {
    if (!IsEmptyEntityId(player_)) {
        return;
    }
    GameEntity::ID playerEntity = 0;
    if (saves_->PlayerExists()) {
        auto playerMessage = saves_->ReadPlayer();
        if (playerMessage.has_value()) {
            playerEntity = RecoveryEntity(playerMessage->entity());
        }
    }
    if (IsEmptyEntityId(playerEntity)) {
        playerEntity = CreateEntity();
    }
    SetPersistable(playerEntity, true);
    if (!HasComponent<PlayerControlComponent>(playerEntity)) {
        AddComponent<PlayerControlComponent>(playerEntity);
    }
    if (!HasComponent<Transform2DComponent>(playerEntity)) {
        const auto transform2DComponentInPlayer = AddComponent<Transform2DComponent>(playerEntity);
        const auto height = GetHeight(0);
        transform2DComponentInPlayer->SetPosition(
            TileLayerComponent::TileToWorld(TileVector2D(0, height + 3)));
    }
    SetCameraPosition(GetComponent<Transform2DComponent>(playerEntity));
    if (!HasComponent<DebugDrawComponent>(playerEntity)) {
        const auto debugDrawComponent = AddComponent<DebugDrawComponent>(playerEntity);
        debugDrawComponent->SetSize(Vector2D(static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE)));
        debugDrawComponent->SetColor(SDL_Color{255, 0, 0, 255});
    }
    if (!HasComponent<CameraComponent>(playerEntity)) {
        AddComponent<CameraComponent>(playerEntity);
    }
    SetCameraComponent(GetComponent<CameraComponent>(playerEntity));
    if (!HasComponent<DiggingComponent>(playerEntity)) {
        AddComponent<DiggingComponent>(playerEntity);
    }
    SetDiggingComponent(GetComponent<DiggingComponent>(playerEntity));
    if (!HasComponent<RigidBody2DComponent>(playerEntity)) {
        const auto rigidBody2DComponent = AddComponent<RigidBody2DComponent>(playerEntity);
        rigidBody2DComponent->SetBodyType(b2_dynamicBody);
        rigidBody2DComponent->SetEnableSleep(false);
        rigidBody2DComponent->SetFixedRotation(true);
        rigidBody2DComponent->SetWidth(1.25F * TILE_SIZE);
        rigidBody2DComponent->SetHeight(2.6F * TILE_SIZE);
        rigidBody2DComponent->SetCategoryBits(BOX2D_CATEGORY_PLAYER);
        rigidBody2DComponent->SetMaskBits(BOX2D_CATEGORY_TILE);
        rigidBody2DComponent->CreateBody(GetWorldId(),
                                         Box2DUtils::ToMeters(
                                             GetComponent<Transform2DComponent>(playerEntity)->GetPosition()));
    }
    if (!HasComponent<MagnetComponent>(playerEntity)) {
        auto *magnetComponent = AddComponent<MagnetComponent>(playerEntity);
        magnetComponent->SetType(MAGNETIC_TYPE_ITEM);
    }

    if (!HasComponent<ItemContainerComponent>(playerEntity)) {
        AddComponent<ItemContainerComponent>(playerEntity, HOT_BAR_SIZE);
    }
    if (!HasComponent<AutoPickComponent>(playerEntity)) {
        AddComponent<AutoPickComponent>(playerEntity);
    }
    player_ = playerEntity;
}

void glimmer::WorldContext::InitHotbar(ItemContainer *itemContainer) {
    auto hotBar = CreateEntity();
    auto *hotBarComponent = AddComponent<HotBarComponent>(hotBar, HOT_BAR_SIZE);
    hotBarEntity = hotBar;
    auto uiScale = appContext_->GetConfig()->window.uiScale;
    constexpr float slotStep = ITEM_SLOT_SIZE + ITEM_SLOT_PADDING;
    for (int i = 0; i < HOT_BAR_SIZE; ++i) {
        const auto slotEntity = CreateEntity();
        AddComponent<ItemSlotComponent>(slotEntity, itemContainer, i);
        auto *guiTransform2DComponent = AddComponent<GuiTransform2DComponent>(slotEntity);
        guiTransform2DComponent->SetSize(CameraVector2D(ITEM_SLOT_SIZE * uiScale, ITEM_SLOT_SIZE * uiScale));
        guiTransform2DComponent->SetPosition(CameraVector2D(
            (ITEM_SLOT_PADDING + slotStep * static_cast<float>(i)) * uiScale,
            ITEM_SLOT_PADDING * uiScale
        ));

        hotBarComponent->AddSlotEntity(slotEntity);
    }
}

void glimmer::WorldContext::InitItemEditor() {
    const auto itemEditorEntity = CreateEntity();
    itemEditorComponent_ = AddComponent<ItemEditorComponent>(itemEditorEntity);
}

glimmer::ItemEditorComponent *glimmer::WorldContext::GetItemEditorComponent() const {
    return itemEditorComponent_;
}

glimmer::GameEntity::ID glimmer::WorldContext::GetPlayerEntity() const {
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
    float landmassNoise = mountainNoise * MOUNTAIN_WEIGHT + hillsNoise * HILLS_WEIGHT;

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
    const float altitudePenalty = std::pow(1.0F - elevation, 1.5F);
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

void glimmer::WorldContext::LoadChunkAt(TileVector2D position) {
    TileVector2D relativeCoordinates = Chunk::TileCoordinatesToChunkRelativeCoordinates(position);
    if (relativeCoordinates.x != 0 || relativeCoordinates.y != 0) {
        LogCat::e("The loaded coordinates are not vertex coordinates.");
        assert(false);
    }
    if (chunks_.contains(position)) {
        return;
    }
    if (saves_->EntityExists(position)) {
        //Read and load the entity file.
        //读取并加载实体文件。
        auto chunkEntityMessageOptional = saves_->ReadChunkEntity(position);
        if (chunkEntityMessageOptional.has_value()) {
            ChunkEntityMessage &chunkEntityMessage = chunkEntityMessageOptional.value();
            int entitySize = chunkEntityMessage.entitys_size();
            for (int i = 0; i < entitySize; i++) {
                RecoveryEntity(chunkEntityMessage.entitys(i));
            }
        }
    }

    auto chunkExists = saves_->ChunkExists(position);
    bool loadFromFile = false;
    auto chunk = std::make_unique<Chunk>(position);
    if (chunkExists) {
        //Read the chunk file.
        //读取区块文件。
        auto chunkMessage = saves_->ReadChunk(position);
        if (chunkMessage.has_value()) {
            chunk.get()->FromMessage(appContext_, chunkMessage.value());
            loadFromFile = true;
        }
    }
    if (!loadFromFile) {
        //Failed to load the file. Creating the block instead.
        //从文件加载失败，创建区块。
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
        ResourceRef bedrockTileRef;
        bedrockTileRef.SetResourceType(RESOURCE_TYPE_TILE);
        bedrockTileRef.SetPackageId(RESOURCE_REF_CORE);
        bedrockTileRef.SetSelfPackageId(RESOURCE_REF_CORE);
        bedrockTileRef.SetResourceKey(TILE_ID_BEDROCK);
        std::map<std::string, std::vector<TileVector2D> > biomeMap = {};
        std::map<std::string, BiomeResource *> biomeResourceMap = {};
        //Does the current block contain the sky? (Air block)
        //当前区块内是否包含天空。（空气方块）
        bool includeSky = false;
        for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
            const int height = GetHeight(position.x + localX);
            for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
                TileVector2D localTile(localX, localY);
                TileVector2D worldTilePos = position + localTile;
                if (worldTilePos.y == WORLD_MIN_Y) {
                    tilesRef[localX][localY] = bedrockTileRef;
                    continue;
                }
                if (worldTilePos.y > height) {
                    if (worldTilePos.y < SEA_LEVEL_HEIGHT) {
                        //water
                        //水
                        tilesRef[localX][localY] = waterTileRef;
                    } else {
                        //sky
                        //天空
                        tilesRef[localX][localY] = airTileRef;
                        includeSky = true;
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
                tilesRef[localX][localY] = airTileRef;
                biomeMap[biomeResource->key].push_back(localTile);
                if (!biomeResourceMap.contains(biomeResource->key)) {
                    biomeResourceMap[biomeResource->key] = biomeResource;
                }
            }
        }
        for (auto &[biomeKey, tilePositions]: biomeMap) {
            BiomeResource *biomeResource = biomeResourceMap[biomeKey];
            for (auto &tilePlacerRef: biomeResource->tilePlacerRefs) {
                LogCat::d(biomeKey, " tilePlacerRefs size=", biomeResource->tilePlacerRefs.size());
                std::string id = tilePlacerRef.id;
                TilePlacer *tilePlacer = appContext_->GetTilePlacerManager()->GetTilePlacer(id);
                if (tilePlacer == nullptr) {
                    LogCat::w("Tile placer ", id, " does not exist.");
                    continue;
                }
                //As long as this part is called, it indicates that the current block is not completely empty.
                //只要调用了这里就表示当前区块，并非全部为空气。
                LogCat::d("Placement ", id, " execute.");
                if (!tilePlacer->PlaceTileId(appContext_, tilesRef, tilePlacerRef.tiles, tilePositions,
                                             includeSky,
                                             tilePlacerRef.config)) {
                    LogCat::w("Placement ", id, " failed to execute. Block coordinates: x:", position.x, ",y:",
                              position.y,
                              ".");
                }
            }
        }
        for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
            for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
                TileVector2D localTile(localX, localY);
                ResourceRef resourceRef = tilesRef[localX][localY];
                const std::optional<TileResource *> tileResource = appContext_->GetResourceLocator()->FindTile(
                    resourceRef);
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
    }
    ChunkPhysicsHelper::AttachPhysicsBodyToChunk(worldId_, chunk.get());
    chunks_.insert({position, std::move(chunk)});
    chunksVersion_++;
}


void glimmer::WorldContext::UnloadChunkAt(TileVector2D position) {
    auto it = chunks_.find(position);
    if (it == chunks_.end()) {
        return;
    }
    if (SaveChunk(position)) {
        ChunkPhysicsHelper::DetachPhysicsBodyToChunk(it->second.get());
        chunks_.erase(it);
        chunksVersion_++;
    } else {
        LogCat::e("Saving the chunk failed, x=", position.x, ",y=", position.y);
    }
}

bool glimmer::WorldContext::SaveChunk(TileVector2D position) {
    const auto it = chunks_.find(position);
    if (it == chunks_.end()) {
        return false;
    }
    ChunkMessage chunkMessage;
    Chunk *chunk = it->second.get();
    chunk->ToMessage(chunkMessage);
    (void) saves_->WriteChunk(position, chunkMessage);
    const WorldVector2D startWorldVector2d = chunk->GetStartWorldPosition();
    const WorldVector2D endWorldVector2d = chunk->GetEndWorldPosition();
    const float minX = std::min(startWorldVector2d.x, endWorldVector2d.x);
    const float maxX = std::max(startWorldVector2d.x, endWorldVector2d.x);
    const float minY = std::min(startWorldVector2d.y, endWorldVector2d.y);
    const float maxY = std::max(startWorldVector2d.y, endWorldVector2d.y);
    auto transform2DEntityList = GetEntityIDWithComponents<Transform2DComponent>();
    ChunkEntityMessage chunkEntityMessage;
    std::vector<GameEntity::ID> entitiesToRemove;
    for (auto &transform2dEntity: transform2DEntityList) {
        if (transform2dEntity == player_) {
            continue;
        }
        auto *transform2dComponent = GetComponent<Transform2DComponent>(transform2dEntity);
        if (transform2dComponent == nullptr) {
            continue;
        }
        WorldVector2D pos = transform2dComponent->GetPosition();
        if (pos.x < minX || pos.x >= maxX ||
            pos.y < minY || pos.y >= maxY) {
            continue;
        }
        (void) SaveEntity(chunkEntityMessage.add_entitys(), transform2dEntity);
        //Whether this entity is successfully saved or not, it will disappear due to the block unloading.
        //无论这个实体是否成功保存，它都会因为区块卸载而消失。
        entitiesToRemove.push_back(transform2dEntity);
    }
    if (chunkEntityMessage.entitys_size() > 0) {
        //Create a file and save it
        //创建文件并保存
        (void) saves_->WriteChunkEntity(position, chunkEntityMessage);
    } else {
        (void) saves_->DeleteChunkEntity(position);
    }
    for (auto id: entitiesToRemove) {
        RemoveEntity(id);
    }
    return true;
}

bool glimmer::WorldContext::SaveEntity(EntityItemMessage *entityItemMessage, const GameEntity::ID entityId) {
    if (!IsPersistable(entityId)) {
        return false;
    }
    entityItemMessage->mutable_gameentity()->set_id(entityId);
    auto &components = entityComponents[entityId];
    auto mutableComponents = entityItemMessage->mutable_components();
    for (auto &componentItem: components) {
        if (!componentItem->IsSerializable()) {
            continue;
        }
        ComponentMessage *componentMessage = mutableComponents->Add();
        componentMessage->set_id(componentItem->GetId());
        componentMessage->set_data(componentItem->Serialize());
    }
    return true;
}

bool glimmer::WorldContext::HasChunk(const TileVector2D position) const {
    return chunks_.contains(position);
}

bool glimmer::WorldContext::ChunkIsOutOfBounds(const TileVector2D position) {
    return position.y >= WORLD_MAX_Y || position.y < WORLD_MIN_Y;
}


bool glimmer::WorldContext::HandleEvent(const SDL_Event &event) const {
    bool handled = false;
    for (auto &system: activeSystems) {
        if (system == nullptr) {
            continue;
        }
        if (!running && !system->CanRunWhilePaused()) {
            continue;
        }
        if (system->HandleEvent(event)) {
            handled = true;
        }
    }
    return handled;
}

void glimmer::WorldContext::Update(const float delta) const {
    for (auto &system: activeSystems) {
        if (system == nullptr) {
            continue;
        }
        if (!running && !system->CanRunWhilePaused()) {
            continue;
        }
        system->Update(delta);
    }
}

bool glimmer::WorldContext::OnBackPressed() const {
    bool handled = false;
    for (auto &system: activeSystems) {
        if (system == nullptr) {
            continue;
        }
        if (!running && !system->CanRunWhilePaused()) {
            continue;
        }
        if (system->OnBackPressed()) {
            handled = true;
        }
    }
    return handled;
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
#if  defined(NDEBUG)
        system->Render(renderer);
#else
        SDL_Color oldColor;
        SDL_GetRenderDrawColor(renderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);
        system->Render(renderer);
        SDL_Color newColor;
        SDL_GetRenderDrawColor(renderer, &newColor.r, &newColor.g, &newColor.b, &newColor.a);
        if (oldColor.a != newColor.a || oldColor.r != newColor.r || oldColor.g != newColor.g || oldColor.b != newColor.
            b) {
            LogCat::e("The color of the renderer has been changed by the game system.", system->GetName());
            assert(false);
        }
#endif
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
    RegisterSystem(std::make_unique<ItemSlotSystem>(appContext, this));
    RegisterSystem(std::make_unique<MagnetSystem>(appContext, this));
    RegisterSystem(std::make_unique<DroppedItemSystem>(appContext, this));
    RegisterSystem(std::make_unique<AutoPickSystem>(appContext, this));
    RegisterSystem(std::make_unique<DiggingSystem>(appContext, this));
    RegisterSystem(std::make_unique<PauseSystem>(appContext, this));
    RegisterSystem(std::make_unique<ItemEditorSystem>(appContext, this));
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

void glimmer::WorldContext::SetDiggingComponent(DiggingComponent *diggingComponent) {
    diggingComponent_ = diggingComponent;
}

void glimmer::WorldContext::SetCameraComponent(CameraComponent *cameraComponent) {
    cameraComponent_ = cameraComponent;
}

glimmer::DiggingComponent *glimmer::WorldContext::GetDiggingComponent() const {
    return diggingComponent_;
}

glimmer::CameraComponent *glimmer::WorldContext::GetCameraComponent() const {
    return cameraComponent_;
}

glimmer::Transform2DComponent *glimmer::WorldContext::GetCameraTransform2D() const {
    return cameraTransform2D_;
}

glimmer::GameEntity::ID glimmer::WorldContext::GetHotBarEntity() const {
    return hotBarEntity;
}


void glimmer::WorldContext::RegisterSystem(std::unique_ptr<GameSystem> system) {
    if (allowRegisterSystem) {
        LogCat::i("Registered system: ", system->GetName());
        inactiveSystems.push_back(std::move(system));
    } else {
        LogCat::e("WorldContext is not allowed to register system");
    }
}


glimmer::GameEntity::ID glimmer::WorldContext::CreateEntity() {
    return RegisterEntity(std::make_unique<GameEntity>(++entityId_));
}

glimmer::GameEntity::ID glimmer::WorldContext::RecoveryEntity(const EntityItemMessage &entityItemMessage) {
    const auto id = entityItemMessage.gameentity().id();
    auto entity = std::make_unique<GameEntity>(id);
    const int componentSize = entityItemMessage.components_size();
    RigidBody2DComponent *rigidBody2dComponent = nullptr;
    Transform2DComponent *transform2dComponent = nullptr;
    for (int j = 0; j < componentSize; j++) {
        GameComponent *component = RecoveryComponent(id, entityItemMessage.components(j));
        if (component->GetId() == COMPONENT_ID_RIGID_BODY_2D) {
            rigidBody2dComponent = dynamic_cast<RigidBody2DComponent *>(component);
        }
        if (component->GetId() == COMPONENT_ID_TRANSFORM_2D) {
            transform2dComponent = dynamic_cast<Transform2DComponent *>(component);
        }
    }
    if (rigidBody2dComponent != nullptr && transform2dComponent != nullptr) {
        rigidBody2dComponent->CreateBody(worldId_,
                                         Box2DUtils::ToMeters(
                                             transform2dComponent->GetPosition()));
    }
    return RegisterEntity(std::move(entity));
}

void glimmer::WorldContext::ShowItemEditorPanel(const ComposableItem *composableItem) {
    if (!IsEmptyEntityId(itemEditorPanel_)) {
        LogCat::e("Please close the previous panel first.");
        assert(false);
    }
    const auto entityId = CreateEntity();
    auto *itemEditorComponent = AddComponent<ItemEditorComponent>(entityId);
    ItemContainer *itemContainer = composableItem->GetItemContainer();
    const size_t capacity = itemContainer->GetCapacity();
    itemEditorComponent->Reserve(capacity);
    auto uiScale = appContext_->GetConfig()->window.uiScale;
    constexpr float slotStep = ITEM_SLOT_SIZE + ITEM_SLOT_PADDING;
    for (int i = 0; i < capacity; i++) {
        const auto slotEntity = CreateEntity();
        AddComponent<ItemSlotComponent>(slotEntity, itemContainer, i);
        auto *guiTransform2DComponent = AddComponent<GuiTransform2DComponent>(slotEntity);
        guiTransform2DComponent->SetSize(CameraVector2D(ITEM_SLOT_SIZE * uiScale, ITEM_SLOT_SIZE * uiScale));
        guiTransform2DComponent->SetPosition(CameraVector2D(
            (ITEM_SLOT_PADDING + slotStep * static_cast<float>(i)) * uiScale,
            (ITEM_SLOT_PADDING + slotStep) * uiScale
        ));
        itemEditorComponent->AddSlotEntity(slotEntity);
    }
    itemEditorPanel_ = entityId;
}

bool glimmer::WorldContext::IsItemEditorPanelVisible() const {
    return !IsEmptyEntityId(itemEditorPanel_);
}

void glimmer::WorldContext::HideItemEditorPanel() {
    if (!IsEmptyEntityId(itemEditorPanel_)) {
        if (auto *itemEditorComponent = GetComponent<ItemEditorComponent>(itemEditorPanel_);
            itemEditorComponent != nullptr) {
            for (const GameEntity::ID &slotEntity: itemEditorComponent->GetSlotEntities()) {
                RemoveEntity(slotEntity);
            }
        }
        RemoveEntity(itemEditorPanel_);
        itemEditorPanel_ = 0;
    }
}

glimmer::GameComponent *glimmer::WorldContext::RecoveryComponent(const GameEntity::ID id,
                                                                 const ComponentMessage &componentMessage) {
    uint32_t componentId = componentMessage.id();
    GameComponent *gameComponent = nullptr;
    switch (componentId) {
        case COMPONENT_ID_AUTO_PICK:
            gameComponent = AddComponent<AutoPickComponent>(id);
            break;
        case COMPONENT_ID_CAMERA:
            gameComponent = AddComponent<CameraComponent>(id);
            break;
        case COMPONENT_ID_DEBUG_DRAW:
            gameComponent = AddComponent<DebugDrawComponent>(id);
            break;
        case COMPONENT_ID_DIGGING:
            gameComponent = AddComponent<DiggingComponent>(id);
            break;
        case COMPONENT_ID_DROPPED_ITEM:
            gameComponent = AddComponent<DroppedItemComponent>(id);
            break;
        case COMPONENT_ID_ITEM_CONTAINER:
            gameComponent = AddComponent<ItemContainerComponent>(id);
            break;
        case COMPONENT_ID_MAGNET:
            gameComponent = AddComponent<MagnetComponent>(id);
            break;
        case COMPONENT_ID_MAGNETIC:
            gameComponent = AddComponent<MagneticComponent>(id);
            break;
        case COMPONENT_ID_RIGID_BODY_2D:
            gameComponent = AddComponent<RigidBody2DComponent>(id);
            break;
        case COMPONENT_ID_TRANSFORM_2D:
            gameComponent = AddComponent<Transform2DComponent>(id);
            break;
        default:
            LogCat::w("The game components do not support serialization.");
            return nullptr;
    }
    //Non-const lvalue reference to type std::string cannot bind to lvalue of type const std::string
    gameComponent->Deserialize(appContext_, this, componentMessage.data());
    return gameComponent;
}


glimmer::GameEntity::ID
glimmer::WorldContext::CreateDroppedItemEntity(std::unique_ptr<Item> item, const WorldVector2D position,
                                               const float pickupCooldown) {
    const GameEntity::ID droppedEntity = CreateEntity();
    SetPersistable(droppedEntity, true);
    auto *transform2dComponent = AddComponent<
        Transform2DComponent>(droppedEntity);
    transform2dComponent->SetPosition(position);
    auto *droppedItemComponent = AddComponent<DroppedItemComponent>(
        droppedEntity
    );
    droppedItemComponent->SetItem(std::move(item));
    droppedItemComponent->SetPickupCooldown(pickupCooldown);
    const auto rigidBody2DComponent = AddComponent<RigidBody2DComponent>(
        droppedEntity);
    rigidBody2DComponent->SetCategoryBits(BOX2D_CATEGORY_ITEM);
    rigidBody2DComponent->SetMaskBits(BOX2D_CATEGORY_TILE);
    rigidBody2DComponent->SetBodyType(b2_dynamicBody);
    rigidBody2DComponent->SetWidth(DROPPED_ITEM_SIZE);
    rigidBody2DComponent->SetHeight(DROPPED_ITEM_SIZE);
    rigidBody2DComponent->CreateBody(GetWorldId(),
                                     Box2DUtils::ToMeters(
                                         transform2dComponent->GetPosition()));
    auto *magnetic = AddComponent<MagneticComponent>(droppedEntity);
    magnetic->SetType(MAGNETIC_TYPE_ITEM);
    return droppedEntity;
}

bool glimmer::WorldContext::SetPersistable(const GameEntity::ID id, const bool persistable) {
    const auto it = entityMap.find(id);
    if (it == entityMap.end()) {
        return false;
    }
    it->second->SetPersistable(persistable);
    return true;
}

bool glimmer::WorldContext::IsPersistable(const GameEntity::ID id) {
    const auto it = entityMap.find(id);
    if (it == entityMap.end()) {
        return false;
    }
    return it->second->IsPersistable();
}

std::vector<glimmer::GameEntity::ID> glimmer::WorldContext::GetAllGameEntityId() const {
    std::vector<GameEntity::ID> result;
    for (auto &entity: entities) {
        result.push_back(entity->GetID());
    }
    return result;
}


std::vector<glimmer::GameEntity::ID> glimmer::WorldContext::GetAllPersistableEntityId() const {
    std::vector<GameEntity::ID> result;
    // Traverse all entities
    // 遍历所有实体
    for (auto &entity: entities) {
        const GameEntity *entityPtr = entity.get();
        if (entityPtr == nullptr || !entityPtr->IsPersistable()) {
            continue;
        }
        result.push_back(entityPtr->GetID());
    }
    return result;
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

        // Make a copy of raw pointers to avoid iterator invalidation during removal
        // 为了避免在删除过程中迭代器失效
        std::vector<GameComponent *> componentsToRemove;
        componentsToRemove.reserve(components.size());
        for (const auto &comp: components) {
            componentsToRemove.push_back(comp.get());
        }

        for (auto *comp: componentsToRemove) {
            LogCat::d("Perform component removal =", id, " components = ", comp);
            RemoveComponentInternal(id, comp);
            LogCat::d("Perform component removal =", id, " components = ", comp, " success");
        }

        entityComponents.erase(compIt);
        LogCat::d("All components of Entity ID = ", id, " have been removed.");
    }

    // Remove the entity record
    // 移除实体记录
    UnRegisterEntity(id);
    auto it = std::ranges::find_if(entities, [id](auto &ent) {
        return ent && ent->GetID() == id;
    });
    if (it != entities.end()) {
        entities.erase(it);
    }

    LogCat::i("Entity ID ", id, " successfully removed. Remaining entities = ", entities.size());
}

bool glimmer::WorldContext::IsEmptyEntityId(const GameEntity::ID id) {
    return id <= 0;
}

int glimmer::WorldContext::GetSeed() const {
    return seed;
}

glimmer::WorldContext::WorldContext(AppContext *appContext, const int seed, Saves *saves,
                                    const GameEntity::ID entityId) : seed(seed),
                                                                     entityId_(entityId), saves_(saves) {
    // 1. 大型陆地板块/大陆噪声 (极低频) - 控制大岛屿和大陆的生成
    continentHeightMapNoise = std::make_unique<FastNoiseLite>();
    continentHeightMapNoise->SetSeed(seed);
    continentHeightMapNoise->SetFrequency(0.005F); // 极低频，用于大型板块
    continentHeightMapNoise->SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    // 2. 高原/山脉噪声 (低频) - 控制地形的宏观起伏
    mountainHeightMapNoise = std::make_unique<FastNoiseLite>();
    mountainHeightMapNoise->SetSeed(seed + 1); // 不同的种子
    mountainHeightMapNoise->SetFrequency(0.01F); // 低频，用于主要地形
    mountainHeightMapNoise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    // 3. 丘陵/细节噪声 (中频) - 控制平原和丘陵的细节
    hillsNoiseHeightMapNoise = std::make_unique<FastNoiseLite>();
    hillsNoiseHeightMapNoise->SetSeed(seed + 2); // 不同的种子
    hillsNoiseHeightMapNoise->SetFrequency(0.02F); // 中频，用于细节
    hillsNoiseHeightMapNoise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    humidityMapNoise = std::make_unique<FastNoiseLite>();
    humidityMapNoise->SetSeed(seed + 100);
    humidityMapNoise->SetFrequency(0.005F);
    humidityMapNoise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    temperatureMapNoise = std::make_unique<FastNoiseLite>();
    temperatureMapNoise->SetSeed(seed + 200);
    temperatureMapNoise->SetFrequency(0.01F);
    temperatureMapNoise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    weirdnessMapNoise = std::make_unique<FastNoiseLite>();
    weirdnessMapNoise->SetSeed(seed + 300);
    weirdnessMapNoise->SetFrequency(0.02F);
    weirdnessMapNoise->SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    erosionMapNoise = std::make_unique<FastNoiseLite>();
    erosionMapNoise->SetSeed(seed + 400);
    erosionMapNoise->SetFrequency(0.003F);
    erosionMapNoise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2(0.0F, -10.0F);
    worldId_ = b2CreateWorld(&worldDef);
    InitSystem(appContext);
    appContext_ = appContext;
    for (const auto &command: appContext_->GetCommandManager()->GetCommands() | std::views::values) {
        if (command->RequiresWorldContext()) {
            command->BindWorldContext(this);
        }
    }
    appContext_->GetTilePlacerManager()->SetSeed(seed);
    startTime_ = TimeUtils::GetCurrentTimeMs();
}

long glimmer::WorldContext::GetStartTime() const {
    return startTime_;
}

b2WorldId glimmer::WorldContext::GetWorldId() const {
    return worldId_;
}
