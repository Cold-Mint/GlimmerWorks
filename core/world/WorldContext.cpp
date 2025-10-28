//
// Created by Cold-Mint on 2025/10/24.
//

#include "WorldContext.h"

#include "../Constants.h"
#include "../log/LogCat.h"
#include "../saves/Saves.h"

const std::vector<std::shared_ptr<glimmer::Chunk> > &glimmer::WorldContext::GetChunks() const {
    return chunks;
}

void glimmer::WorldContext::RegisterChunk(const std::shared_ptr<Chunk> &chunk) {
    chunks.push_back(chunk);
}

glimmer::Saves *glimmer::WorldContext::GetSaves() const {
    return saves;
}

bool glimmer::WorldContext::HasComponentType(const std::type_index &type) const {
    const auto it = componentCount.find(type);
    return it != componentCount.end() && it->second > 0;
}

std::vector<int> glimmer::WorldContext::GetHeightMap(int x) {
    const int chunkX = (x / CHUNK_SIZE) * CHUNK_SIZE;
    LogCat::d("getHeightMap called for x=", x, " aligned to chunkX=", chunkX);

    const auto it = heightMap.find(chunkX);
    if (it != heightMap.end()) {
        LogCat::d("HeightMap cache hit for chunkX=", chunkX);
        return it->second;
    }

    LogCat::d("HeightMap cache miss, generating new chunk at chunkX=", chunkX);
    std::vector<int> heights(CHUNK_SIZE);
    for (int i = 0; i < CHUNK_SIZE; ++i) {
        const auto worldX = static_cast<float>(chunkX + i);
        const float noiseValue = heightMapNoise->GetNoise(worldX, 0.0f);
        const int height = static_cast<int>((noiseValue + 1.0f) * 0.5f * (WORLD_HEIGHT - 1));
        heights[i] = height;
    }

    heightMap[chunkX] = heights;
    LogCat::d("Generated and cached heights for chunkX=", chunkX);
    return heights;
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
    for (auto &system: activeSystems) {
        if (system) {
            system->Render(renderer);
        }
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
    allowRegisterSystem = false;
}


void glimmer::WorldContext::RegisterSystem(std::unique_ptr<GameSystem> system) {
    if (allowRegisterSystem) {
        inactiveSystems.push_back(std::move(system));
    } else {
        LogCat::e("WorldContext is not allowed to register system");
    }
}

void glimmer::WorldContext::OnSystemActivationChanged(GameSystem *sys, const bool active) {
    if (active) {
        const auto it = std::ranges::find_if(inactiveSystems,
                                             [sys](const auto &s) { return s.get() == sys; });
        if (it != inactiveSystems.end()) {
            activeSystems.push_back(std::move(*it));
            inactiveSystems.erase(it);
            LogCat::i("Activated system: ", typeid(sys).name());
        }
    } else {
        const auto it = std::ranges::find_if(activeSystems,
                                             [sys](const auto &s) { return s.get() == sys; });
        if (it != activeSystems.end()) {
            inactiveSystems.push_back(std::move(*it));
            activeSystems.erase(it);
            LogCat::i("Deactivated system: ", typeid(sys).name());
        }
    }
}


glimmer::GameEntity *glimmer::WorldContext::CreateEntity() {
    auto entity = std::make_unique<GameEntity>(entities.size());
    entities.push_back(std::move(entity));
    entityMap[entity->GetID()] = entities.back().get();
    return GetEntity(entities.back()->GetID());
}


glimmer::GameEntity *glimmer::WorldContext::GetEntity(const GameEntity::ID id) {
    const auto it = entityMap.find(id);
    return it != entityMap.end() ? it->second : nullptr;
}

void glimmer::WorldContext::RemoveEntity(GameEntity::ID id) {
    if (const auto it = entityMap.find(id); it != entityMap.end()) {
        std::erase_if(entities,
                      [id](const std::unique_ptr<GameEntity> &e) { return e->GetID() == id; });
        entityMap.erase(it);
    }
}
