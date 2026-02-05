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
#include "generator/Chunk.h"
#include "generator/ChunkPhysicsHelper.h"

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
            playerEntity = chunkLoader_->RecoveryEntity(playerMessage->entity());
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
        const auto height = chunkGenerator_->GetHeight(0);
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

void glimmer::WorldContext::LoadChunkAt(TileVector2D position) {
    if (chunks_.contains(position)) {
        return;
    }
    std::unique_ptr<Chunk> chunk = chunkLoader_->LoadChunkFromSaves(position);
    if (chunk == nullptr) {
        chunk = chunkGenerator_->GenerateChunkAt(position);
    }
    ChunkPhysicsHelper::AttachPhysicsBodyToChunk(worldId_, chunk.get());
    chunks_.insert({position, std::move(chunk)});
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
    RegisterSystem(std::make_unique<DroppedItemSystem>(this));
    RegisterSystem(std::make_unique<AutoPickSystem>(this));
    RegisterSystem(std::make_unique<DiggingSystem>(this));
    RegisterSystem(std::make_unique<PauseSystem>(this));
    RegisterSystem(std::make_unique<ItemEditorSystem>(this));
#ifdef __ANDROID__
    RegisterSystem(std::make_unique<AndroidControlSystem>(this));
#endif
#if  !defined(NDEBUG)
    RegisterSystem(std::make_unique<DebugDrawSystem>(this));
    RegisterSystem(std::make_unique<DebugDrawBox2dSystem>(this));
    RegisterSystem(std::make_unique<DebugPanelSystem>(this));
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
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2(0.0F, -10.0F);
    worldId_ = b2CreateWorld(&worldDef);
    InitSystem();
    appContext_ = appContext;
    for (const auto &command: appContext_->GetCommandManager()->GetCommands() | std::views::values) {
        if (command->RequiresWorldContext()) {
            command->BindWorldContext(this);
        }
    }
    appContext_->GetBiomeDecoratorManager()->SetSeed(seed);
    chunkLoader_ = std::make_unique<ChunkLoader>(this, saves, [this](std::unique_ptr<GameEntity> entity) {
        return this->RegisterEntity(std::move(entity));
    });
    chunkGenerator_ = std::make_unique<ChunkGenerator>(this, seed);
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
