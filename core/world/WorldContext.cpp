//
// Created by Cold-Mint on 2025/10/24.
//

#include "WorldContext.h"

#include <utility>


#include "../Constants.h"
#include "../ecs/component/DebugDrawComponent.h"
#include "../ecs/system/GameStartSystem.h"
#include "../ecs/system/Transform2DSystem.h"
#include "../ecs/system/CameraSystem.h"
#include "../ecs/system/ChunkSystem.h"
#include "../ecs/system/DebugDrawBox2dSystem.h"
#include "../ecs/system/DebugDrawSystem.h"
#include "../ecs/system/DebugPanelSystem.h"
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
#include "../saves/Saves.h"
#include "../utils/Box2DUtils.h"
#include "box2d/box2d.h"
#include "core/ecs/DroppedItemCreator.h"
#include "core/ecs/MobEntityCreator.h"
#include "core/ecs/component/AutoPickComponent.h"
#include "core/ecs/component/DraggableComponent.h"
#include "core/ecs/component/DroppedItemComponent.h"
#include "core/ecs/component/GuiTransform2DComponent.h"
#include "core/ecs/component/RigidBody2DComponent.h"
#include "core/ecs/component/HotBarComonent.h"
#include "core/ecs/component/ItemEditorComponent.h"
#include "core/ecs/component/MagnetComponent.h"
#include "core/ecs/component/MagneticComponent.h"
#include "core/ecs/component/SpiritRendererComponent.h"
#include "core/ecs/system/AreaMarkerSystem.h"
#include "core/ecs/system/BiomeBGMSystem.h"
#include "core/ecs/system/DebugMultiMapSystem.h"
#include "core/ecs/system/DraggableSystem.h"
#include "core/ecs/system/FloatingTextSystem.h"
#include "core/ecs/system/ItemEditorSystem.h"
#include "core/ecs/system/Light2DSystem.h"
#include "core/ecs/system/RayCast2DSystem.h"
#include "core/ecs/system/SpiritRendererSystem.h"
#include "core/utils/TimeUtils.h"
#include "generator/Chunk.h"
#include "generator/ChunkPhysicsHelper.h"
#ifdef __ANDROID__
#include "core/ecs/system/AndroidControlSystem.h"
#endif

bool glimmer::WorldContext::IsDragMode() const {
    return dragMode_;
}

const glimmer::Color *glimmer::WorldContext::GetFinalLightColor(const TileVector2D position) const {
    if (lightingBuffer_ == nullptr) {
        return nullptr;
    }
    return lightingBuffer_->GetFinalLightColor(position);
}

void glimmer::WorldContext::SetDragMode(const bool dragMode) {
    dragMode_ = dragMode;
}

void glimmer::WorldContext::RemoveComponentInternal(GameEntity::ID id, GameComponent *comp) {
    const auto type = std::type_index(typeid(*comp));
    //Reduce componentCount
    // 减少 componentCount
    if (componentCount_[type] > 0) --componentCount_[type];

    //Check if the system is disabled
    // 检查系统是否停用
    if (componentCount_[type] == 0) {
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
    GameEntity *raw = entity.get();
    const auto id = raw->GetID();
    entities_.push_back(std::move(entity));
    entityMap_.emplace(id, raw);
    return id;
}


void glimmer::WorldContext::UnRegisterEntity(const GameEntity::ID id) {
    entityMap_.erase(id);
}

void glimmer::WorldContext::OnChunkTileChange(Chunk *chunk, Tile *tile, const TileLayerType layerType,
                                              const int index) const {
    if (layerType == Ground) {
        ChunkPhysicsHelper::UpdatePhysicsBodyToChunk(this, chunk);
    }
    UpdateTileLight(chunk, layerType, index);
}

void glimmer::WorldContext::UpdateTileLight(const Chunk *chunk, const TileLayerType layerType, const int index) const {
    if (chunk == nullptr) {
        return;
    }
    Tile *tile = chunk->GetTile(layerType, index);
    if (tile == nullptr) {
        return;
    }
    const ResourceLocator *resourceLocator = appContext_->GetResourceLocator();
    if (resourceLocator == nullptr) {
        return;
    }
    const TileVector2D chunkPosition = chunk->GetPosition();
    const int localX = index & CHUNK_MASK;
    const int localY = index >> CHUNK_SHIFT;
    auto lightSourcePosition = TileVector2D(chunkPosition.x + localX, chunkPosition.y + localY);
    const LightMaskResource *lightMaskResource = resourceLocator->FindLightMask(tile->GetLightMaskResource());
    if (lightMaskResource != nullptr) {
        const std::unique_ptr<Color> lightMaskColorPtr = resourceLocator->FindColor(
            lightMaskResource->lightMaskColor);
        if (lightMaskColorPtr == nullptr) {
            return;
        }
        if (lightMaskColorPtr->a == 0) {
            lightingBuffer_->ClearLightMask(lightSourcePosition, tile->GetLayerType());
        } else {
            lightingBuffer_->SetLightMask(lightSourcePosition, layerType,
                                          std::make_unique<LightMask>(lightMaskColorPtr.get()));
        }
    }
    const LightSourceResource *lightSourceResource = resourceLocator->FindLightSource(
        tile->GetLightSourceResource());
    if (lightSourceResource != nullptr) {
        const std::unique_ptr<Color> lightColorPtr = resourceLocator->
                FindColor(lightSourceResource->lightColor);
        if (lightColorPtr == nullptr) {
            return;
        }
        if (lightColorPtr->a == 0) {
            lightingBuffer_->ClearLightSource(lightSourcePosition, layerType);
        } else {
            lightingBuffer_->SetLightSource(lightSourcePosition, layerType,
                                            std::make_unique<LightSource>(
                                                lightSourcePosition, lightSourceResource->lightRadius,
                                                *lightColorPtr));
        }
    }
}


void glimmer::WorldContext::UpdateChunkLight(const Chunk *chunk) const {
    if (chunk == nullptr) {
        return;
    }
    if (appContext_ == nullptr) {
        return;
    }

    for (int index = 0; index < CHUNK_AREA; ++index) {
        for (int i = 0; i < TILE_LAYER_TYPE_COUNT; ++i) {
            UpdateTileLight(chunk, static_cast<TileLayerType>(1 << i), index);
        }
    }
}

glimmer::WorldContext::~WorldContext() {
    activeSystems.clear();
    inactiveSystems.clear();
    entityComponents.clear();
    entities_.clear();
    entityMap_.clear();
    chunks_.clear();
    terrainTileData_.clear();
    processedTerrainTiles_.clear();
    chunksCache_.clear();
    terrainTileDataCache_.clear();
    componentCount_.clear();
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

glimmer::TerrainResult *glimmer::WorldContext::GetTerrainData(TileVector2D position) {
    if (auto it = terrainTileData_.find(position); it != terrainTileData_.end()) {
        return it->second.get();
    }
    return nullptr;
}

glimmer::TerrainResult *glimmer::WorldContext::GetOrCreateTerrainData(TileVector2D position) {
    if (auto it = terrainTileData_.find(position); it != terrainTileData_.end()) {
        return it->second.get();
    }

    auto terrainResult = chunkGenerator_->GenerateTerrain(position);
    if (terrainResult == nullptr) {
        return nullptr;
    }
    auto *terrainPtr = terrainResult.get();
    terrainTileData_.emplace(position, std::move(terrainResult));
    terrainTileDataCache_.emplace(position, terrainPtr);
    return terrainPtr;
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

glimmer::MapManifest *glimmer::WorldContext::GetMapManifest() const {
    return mapManifest_;
}

bool glimmer::WorldContext::HasComponentType(const std::type_index &type) const {
    const auto it = componentCount_.find(type);
    return it != componentCount_.end() && it->second > 0;
}

void glimmer::WorldContext::InitPlayer(const ResourceRef &resourceRef) {
    if (!IsEmptyEntityId(player_)) {
        return;
    }
    GameEntity::ID playerEntity = 0;
    if (saves_->PlayerExists()) {
        auto playerMessage = saves_->ReadPlayer();
        if (playerMessage.has_value()) {
            playerEntity = chunkLoader_->RecoveryEntity(playerMessage->entity());
        }
    }
    if (IsEmptyEntityId(playerEntity)) {
        const auto firstTileTerrainY = chunkGenerator_->GetFirstTileTerrainY(0);
        playerEntity = CreateEntity();
        MobEntityCreator mobEntityCreator{this};
        mobEntityCreator.LoadTemplateComponents(playerEntity, resourceRef);
        mobEntityCreator.MergeEntityItemMessage(playerEntity,
                                                MobEntityCreator::GetEntityItemMessage(
                                                    TileLayerComponent::TileToWorld(
                                                        TileVector2D(0, firstTileTerrainY + 3))));
    }
    if (!HasComponent<ItemContainerComponent>(playerEntity)) {
        const auto *itemContainerComponent = AddComponent<ItemContainerComponent>(
            playerEntity, HOT_BAR_SIZE);
        if (itemContainerComponent != nullptr) {
            auto &allInitialInventory = appContext_->GetInitialInventoryManager()->GetAllInitialInventory();
            for (auto &initialInventory: allInitialInventory) {
                for (auto &addItem: initialInventory->addItems) {
                    auto item = appContext_->GetResourceLocator()->FindItem(addItem);
                    if (item == nullptr) {
                        continue;
                    }
                    std::unique_ptr<Item> returnItem = itemContainerComponent->GetItemContainer()->AddItem(
                        std::move(item));
                    if (returnItem != nullptr) {
                        const GameEntity::ID droppedEntity = CreateEntity();
                        DroppedItemCreator droppedItemCreator{this};
                        droppedItemCreator.LoadTemplateComponents(droppedEntity,
                                                                  DroppedItemCreator::GetResourceRef());
                        droppedItemCreator.MergeEntityItemMessage(droppedEntity,
                                                                  DroppedItemCreator::GetEntityItemMessage(
                                                                      GetComponent<Transform2DComponent>(
                                                                          playerEntity)->
                                                                      GetPosition(), std::move(returnItem), 2));
                    }
                }
            }
        }
    }
    player_ = playerEntity;
}

void glimmer::WorldContext::InitHotbar(ItemContainer *itemContainer) {
    auto hotBar = CreateEntity();
    auto *hotBarComponent = AddComponent<HotBarComponent>(hotBar, HOT_BAR_SIZE);
    if (hotBarComponent == nullptr) {
        LogCat::e("initHotbar Error.");
        return;
    }
    hotBarEntity = hotBar;
    auto uiScale = appContext_->GetConfig()->window.uiScale;
    constexpr float slotStep = ITEM_SLOT_SIZE + ITEM_SLOT_PADDING;
    for (int i = 0; i < HOT_BAR_SIZE; ++i) {
        const auto slotEntity = CreateEntity();
        auto *itemSlotComponent = AddComponent<ItemSlotComponent>(slotEntity, itemContainer, i, true);
        if (itemSlotComponent == nullptr) {
            continue;
        }
        itemSlotComponent->SetSelected(i == 0);
        auto *guiTransform2DComponent = AddComponent<GuiTransform2DComponent>(slotEntity);
        if (guiTransform2DComponent == nullptr) {
            continue;
        }
        guiTransform2DComponent->SetSize(CameraVector2D(ITEM_SLOT_SIZE * uiScale, ITEM_SLOT_SIZE * uiScale));
        guiTransform2DComponent->SetPosition(CameraVector2D(
            (ITEM_SLOT_PADDING + slotStep * static_cast<float>(i)) * uiScale,
            ITEM_SLOT_PADDING * uiScale
        ));
        auto draggableComponent = AddComponent<DraggableComponent>(slotEntity);
        if (draggableComponent == nullptr) {
            continue;
        }
        draggableComponent->SetSize(guiTransform2DComponent->GetSize());
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

std::unordered_map<TileVector2D, glimmer::TerrainResult *, glimmer::Vector2DIHash> *glimmer::WorldContext::
GetTerrainResults() {
    return &terrainTileDataCache_;
}

void glimmer::WorldContext::LoadTerrainAt(TileVector2D position) {
    if (processedTerrainTiles_.contains(position)) {
        return;
    }
    chunkGenerator_->GenerateStructure(position);
    processedTerrainTiles_.emplace(position);
}

void glimmer::WorldContext::UnloadTerrainAt(TileVector2D position) {
    if (!processedTerrainTiles_.contains(position)) {
        return;
    }
    processedTerrainTiles_.erase(position);
    terrainTileDataCache_.erase(position);
    terrainTileData_.erase(position);
}

void glimmer::WorldContext::LoadChunkAt(TileVector2D position) {
    if (chunks_.contains(position)) {
        return;
    }
    std::unique_ptr<Chunk> newlyCreatedChunk = chunkLoader_->LoadChunkFromSaves(position);
    if (newlyCreatedChunk == nullptr) {
        newlyCreatedChunk = chunkGenerator_->GenerateChunkAt(position);
    }
    if (newlyCreatedChunk == nullptr) {
        return;
    }
    ChunkPhysicsHelper::AttachPhysicsBodyToChunk(appContext_, worldId_, newlyCreatedChunk.get());
    newlyCreatedChunk->AddReplaceTileCallback([this](Chunk *chunk, TileLayerType layerType,
                                                     int index,
                                                     Tile *, Tile *newTile) {
        OnChunkTileChange(chunk, newTile, layerType, index);
    });
    newlyCreatedChunk->AddSetTileCallback([this](Chunk *chunk, int index, Tile *tile, TileLayerType layerType) {
        OnChunkTileChange(chunk, tile, layerType, index);
    });
    chunks_.insert({position, std::move(newlyCreatedChunk)});
    chunksVersion_++;
}

glimmer::ChunkGenerator *glimmer::WorldContext::GetChunkGenerator() const {
    return chunkGenerator_.get();
}


void glimmer::WorldContext::UnloadChunkAt(TileVector2D position) {
    auto it = chunks_.find(position);
    if (it == chunks_.end()) {
        return;
    }
    if (SaveChunk(position)) {
        //Uninstall the light sources and shadows of the current chunk.
        //卸载当前区块的光源和遮挡。
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                auto lightPosition = TileVector2D(position.x + x, position.y + y);
                lightingBuffer_->ClearTileLightData(lightPosition);
            }
        }
        ChunkPhysicsHelper::DetachPhysicsBodyToChunk(appContext_, it->second.get());
        chunks_.erase(it);
        chunksVersion_++;
    } else {
        LogCat::e("Saving the chunk failed, x=", position.x, ",y=", position.y);
    }
}

glimmer::Chunk *glimmer::WorldContext::GetChunk(const TileVector2D position) {
#if  !defined(NDEBUG)
    const TileVector2D relativeVector = Chunk::TileCoordinatesToChunkRelativeCoordinates(position);
    if (relativeVector.x != 0 || relativeVector.y != 0) {
        LogCat::e("The coordinates are not the vertices of the chunk.");
        assert(false);
    }
#endif

    const auto it = chunks_.find(position);
    if (it == chunks_.end()) {
        return nullptr;
    }
    return it->second.get();
}

bool glimmer::WorldContext::SaveChunk(TileVector2D position) {
    const auto it = chunks_.find(position);
    if (it == chunks_.end()) {
        return false;
    }
    ChunkMessage chunkMessage;
    Chunk *chunk = it->second.get();
    chunk->WriteChunkMessage(chunkMessage);
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
        if (IsPersistable(transform2dEntity)) {
            SaveEntity(chunkEntityMessage.add_entitys(), transform2dEntity);
        }
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

void glimmer::WorldContext::SaveEntity(EntityItemMessage *entityItemMessage, const GameEntity::ID entityId) {
    entityItemMessage->mutable_gameentity()->set_id(entityId);
    const ResourceRef *resourceRef = GetResourceRef(entityId);
    if (resourceRef != nullptr) {
        resourceRef->WriteResourceRefMessage(*entityItemMessage->mutable_resourceref());
    }
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
}

bool glimmer::WorldContext::HasChunk(const TileVector2D position) const {
    return chunks_.contains(position);
}

bool glimmer::WorldContext::ChunkIsOutOfBounds(const TileVector2D position) {
    return position.y >= WORLD_MAX_Y || position.y < WORLD_MIN_Y || position.x >= WORLD_MAX_X || position.x <
           WORLD_MIN_X;
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
    std::ranges::stable_sort(systemsToRender,
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
            LogCat::e("The color of the renderer has been changed by the game system.", system->GetName(),
                      " invoke AppContext::RestoreColorRenderer(renderer);");
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

void glimmer::WorldContext::InitSystem() {
    allowRegisterSystem = true;
    RegisterSystem(std::make_unique<GameStartSystem>(this));
    RegisterSystem(std::make_unique<Transform2DSystem>(this));
    RegisterSystem(std::make_unique<CameraSystem>(this));
    RegisterSystem(std::make_unique<PlayerControlSystem>(this));
    RegisterSystem(std::make_unique<TileLayerSystem>(this));
    RegisterSystem(std::make_unique<ChunkSystem>(this));
    RegisterSystem(std::make_unique<PhysicsSystem>(this));
    RegisterSystem(std::make_unique<HotBarSystem>(this));
    RegisterSystem(std::make_unique<ItemSlotSystem>(this));
    RegisterSystem(std::make_unique<MagnetSystem>(this));
    RegisterSystem(std::make_unique<FloatingTextSystem>(this));
    RegisterSystem(std::make_unique<DroppedItemSystem>(this));
    RegisterSystem(std::make_unique<AutoPickSystem>(this));
    RegisterSystem(std::make_unique<AreaMarkerSystem>(this));
    RegisterSystem(std::make_unique<DiggingSystem>(this));
    RegisterSystem(std::make_unique<DraggableSystem>(this));
    RegisterSystem(std::make_unique<SpiritRendererSystem>(this));
    RegisterSystem(std::make_unique<PauseSystem>(this));
    RegisterSystem(std::make_unique<ItemEditorSystem>(this));
    RegisterSystem(std::make_unique<DebugDrawSystem>(this));
    RegisterSystem(std::make_unique<DebugDrawBox2dSystem>(this));
    RegisterSystem(std::make_unique<DebugPanelSystem>(this));
    RegisterSystem(std::make_unique<DebugMultiMapSystem>(this));
    RegisterSystem(std::make_unique<RayCast2DSystem>(this));
    RegisterSystem(std::make_unique<BiomeBGMSystem>(this));
    RegisterSystem(std::make_unique<Light2DSystem>(this));
#ifdef __ANDROID__
    RegisterSystem(std::make_unique<AndroidControlSystem>(this));
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

glimmer::GameComponent *glimmer::WorldContext::RecoveryComponent(GameEntity::ID id,
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
        case COMPONENT_ID_TRANSFORM_2D:
            gameComponent = AddComponent<Transform2DComponent>(id);
            break;
        default:
            LogCat::w("The game components do not support serialization.");
            return nullptr;
    }
    if (gameComponent != nullptr) {
        gameComponent->Deserialize(this, componentMessage.data());
    }
    return gameComponent;
}


void glimmer::WorldContext::ShowItemEditorPanel(const ComposableItem *composableItem) {
    if (!IsEmptyEntityId(itemEditorPanel_)) {
        LogCat::e("Please close the previous panel first.");
#if  !defined(NDEBUG)
        assert(false);
#endif
    }
    const auto entityId = CreateEntity();
    auto *itemEditorComponent = AddComponent<ItemEditorComponent>(entityId);
    if (itemEditorComponent == nullptr) {
        LogCat::e("itemEditorComponent is nullptr.");
        return;
    }
    ItemContainer *itemContainer = composableItem->GetItemContainer();
    const size_t capacity = itemContainer->GetCapacity();
    itemEditorComponent->Reserve(capacity);
    auto uiScale = appContext_->GetConfig()->window.uiScale;
    constexpr float slotStep = ITEM_SLOT_SIZE + ITEM_SLOT_PADDING;
    for (int i = 0; i < capacity; i++) {
        const auto slotEntity = CreateEntity();
        AddComponent<ItemSlotComponent>(slotEntity, itemContainer, i, false);
        auto *guiTransform2DComponent = AddComponent<GuiTransform2DComponent>(slotEntity);
        if (guiTransform2DComponent == nullptr) {
            continue;
        }
        guiTransform2DComponent->SetSize(CameraVector2D(ITEM_SLOT_SIZE * uiScale, ITEM_SLOT_SIZE * uiScale));
        guiTransform2DComponent->SetPosition(CameraVector2D(
            (ITEM_SLOT_PADDING + slotStep * static_cast<float>(i)) * uiScale,
            (ITEM_SLOT_PADDING + slotStep) * uiScale
        ));
        auto draggableComponent = AddComponent<DraggableComponent>(slotEntity);
        if (draggableComponent == nullptr) {
            continue;
        }
        draggableComponent->SetSize(guiTransform2DComponent->GetSize());
        itemEditorComponent->AddSlotEntity(slotEntity);
    }
    itemEditorPanel_ = entityId;
    dragMode_ = true;
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
    dragMode_ = false;
}


bool glimmer::WorldContext::SetPersistable(const GameEntity::ID id, const bool persistable) {
    const auto it = entityMap_.find(id);
    if (it == entityMap_.end()) {
        return false;
    }
    it->second->SetPersistable(persistable);
    return true;
}

bool glimmer::WorldContext::SetResourceRef(const GameEntity::ID id, const ResourceRef &resourceRef) {
    const auto it = entityMap_.find(id);
    if (it == entityMap_.end()) {
        return false;
    }
    it->second->SetResourceRef(resourceRef);
    return true;
}

const glimmer::ResourceRef *glimmer::WorldContext::GetResourceRef(const GameEntity::ID id) {
    const auto it = entityMap_.find(id);
    if (it == entityMap_.end()) {
        return nullptr;
    }
    return &it->second->GetResourceRef();
}


bool glimmer::WorldContext::IsPersistable(const GameEntity::ID id) {
    const auto it = entityMap_.find(id);
    if (it == entityMap_.end()) {
        return false;
    }
    return it->second->IsPersistable();
}

std::vector<glimmer::GameEntity::ID> glimmer::WorldContext::GetAllGameEntityId() const {
    std::vector<GameEntity::ID> result;
    for (auto &entity: entities_) {
        result.push_back(entity->GetID());
    }
    return result;
}


void glimmer::WorldContext::RemoveEntity(GameEntity::ID id) {
    LogCat::d("Attempting to remove entity ID = ", id);
    auto entityIt = entityMap_.find(id);
    if (entityIt == entityMap_.end()) {
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
    auto it = std::ranges::find_if(entities_, [id](auto &ent) {
        return ent && ent->GetID() == id;
    });
    if (it != entities_.end()) {
        entities_.erase(it);
    }

    LogCat::i("Entity ID ", id, " successfully removed. Remaining entities = ", entities_.size());
}

bool glimmer::WorldContext::IsEmptyEntityId(const GameEntity::ID id) {
    return id <= GAME_ENTITY_ID_INVALID;
}

int glimmer::WorldContext::GetWorldSeed() const {
    return worldSeed_;
}

glimmer::WorldContext::WorldContext(AppContext *appContext, MapManifest *mapManifest, Saves *saves) : saves_(
    saves) {
    worldSeed_ = mapManifest->seed;
    entityId_ = mapManifest->entityIDIndex;
    mapManifest_ = mapManifest;
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2(0.0F, -10.0F);
    worldId_ = b2CreateWorld(&worldDef);
    appContext_ = appContext;
    InitSystem();
    for (const auto &command: appContext_->GetCommandManager()->GetCommands() | std::views::values) {
        if (command->RequiresWorldContext()) {
            command->BindWorldContext(this);
        }
    }
    appContext_->GetBiomeDecoratorManager()->SetWorldSeed(worldSeed_);
    appContext->GetStructureGeneratorManager()->SetWorldSeed(worldSeed_);
    chunkLoader_ = std::make_unique<ChunkLoader>(this, saves, [this](std::unique_ptr<GameEntity> entity) {
        return this->RegisterEntity(std::move(entity));
    });
    lightingBuffer_ = std::make_unique<LightingBuffer>();
    chunkGenerator_ = std::make_unique<ChunkGenerator>(this, worldSeed_);
    startTime_ = TimeUtils::GetCurrentTimeMs();
}

long glimmer::WorldContext::GetStartTime() const {
    return startTime_;
}

glimmer::AppContext *glimmer::WorldContext::GetAppContext() const {
    return appContext_;
}

b2WorldId glimmer::WorldContext::GetWorldId() const {
    return worldId_;
}
