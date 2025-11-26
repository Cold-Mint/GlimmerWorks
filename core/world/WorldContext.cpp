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
#include "../ecs/system/PhysicsSystem.h"
#include "../ecs/system/PlayerControlSystem.h"
#include "../ecs/system/TileLayerSystem.h"
#include "../log/LogCat.h"

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

std::vector<int> glimmer::WorldContext::GetHeightMap(int x) {
    auto tileToChunkStart = [](const int tileCoord) -> int {
        if (tileCoord >= 0)
            return (tileCoord / CHUNK_SIZE) * CHUNK_SIZE;
        // 对负数向下对齐，例如 CHUNK_SIZE=16:
        // tileCoord = -1 -> chunkStart = -16
        return (tileCoord - (CHUNK_SIZE - 1)) / CHUNK_SIZE * CHUNK_SIZE;
    };
    const int chunkX = tileToChunkStart(x);
    LogCat::d("getHeightMap called for x=", x, " aligned to chunkX=", chunkX);

    const auto it = heightMap.find(chunkX);
    if (it != heightMap.end()) {
        LogCat::d("HeightMap cache hit for chunkX=", chunkX);
        return it->second;
    }

    LogCat::d("HeightMap cache miss, generating new chunk at chunkX=", chunkX);
    std::vector<int> heights(CHUNK_SIZE);
    for (int i = 0; i < CHUNK_SIZE; ++i) {
        const auto sampleX = static_cast<float>(chunkX + i);
        const float noiseValue = heightMapNoise->GetNoise(sampleX, 0.0F);
        const int height = static_cast<int>((noiseValue + 1.0F) * 0.5F * (WORLD_MAX_Y - WORLD_MIN_Y)) + WORLD_MIN_Y;
        heights[i] = height;
    }

    heightMap[chunkX] = heights;
    LogCat::d("Generated and cached heights for chunkX=", chunkX);
    return heights;
}

void glimmer::WorldContext::LoadChunkAt(TileLayerComponent *tileLayerComponent, const WorldVector2D &tileLayerPos,
                                        const TileVector2D position) {
    if (chunks_.contains(position))
        return;

    Chunk newChunk(position);

    // 获取该区块（以 position.x 对齐）的高度数组（长度 CHUNK_SIZE）
    // position.x 是区块左上角的 world tile x（已对齐到 CHUNK_SIZE）
    const std::vector<int> heights = GetHeightMap(position.x);

    for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
        for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
            TileVector2D localTile(localX, localY);
            TileVector2D worldTilePos = position + localTile; // world 坐标

            Tile tile;

            // 从 heights 中获取该列（world X 对应的高度）
            // heights 下标 localX 对应 worldTilePos.x == position.x + localX
            const int height = heights[localX];

            int worldY = worldTilePos.y; // 世界 Y 坐标

            // 根据高度决定颜色（可以按需替换颜色值）
            if (worldY > height) {
                tile.physicsType = TilePhysicsType::None;
                // 在地面之下或水下（y 比地面大） -> 蓝色
                tile.color = {0, 128, 255, 255};
            } else {
                tile.physicsType = TilePhysicsType::Static;
                // 在或高于地面 -> 棕色
                tile.color = {139, 69, 19, 255};
            }

            newChunk.SetTile(localTile, tile);
            tileLayerComponent->SetTile(worldTilePos, tile);
        }
    }
    chunkPhysicsHelper_->AttachPhysicsBodyToChunk(worldId_, tileLayerPos,
                                                  &newChunk);
    chunks_.insert({position, newChunk});
}


void glimmer::WorldContext::UnloadChunkAt(TileLayerComponent *tileLayerComponent, TileVector2D position) {
    auto it = chunks_.find(position);
    if (it == chunks_.end()) {
        return;
    }
    ChunkPhysicsHelper::DetachPhysicsBodyToChunk(&it->second);
    if (tileLayerComponent != nullptr) {
        for (int y = 0; y < CHUNK_SIZE; ++y) {
            for (int x = 0; x < CHUNK_SIZE; ++x) {
                TileVector2D tilePos = position + TileVector2D(x, y);
                tileLayerComponent->ClearTile(tilePos);
            }
        }
    }
    chunks_.erase(it);
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

const std::unordered_map<TileVector2D, glimmer::Chunk, glimmer::Vector2DIHash> &glimmer::WorldContext::
GetAllChunks() {
    return chunks_;
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

void glimmer::WorldContext::SetCameraComponent(CameraComponent *cameraComponent) {
    cameraComponent_ = cameraComponent;
}

glimmer::CameraComponent *glimmer::WorldContext::GetCameraComponent() const {
    return cameraComponent_;
}

glimmer::Transform2DComponent *glimmer::WorldContext::GetCameraTransform2D() const {
    return cameraTransform2D_;
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

b2WorldId glimmer::WorldContext::GetWorldId() const {
    return worldId_;
}
