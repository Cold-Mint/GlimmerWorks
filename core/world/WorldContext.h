//
// Created by Cold-Mint on 2025/10/24.
//

#ifndef GLIMMERWORKS_WORLDCONTEXT_H
#define GLIMMERWORKS_WORLDCONTEXT_H

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Chunk.h"
#include "FastNoiseLite.h"
#include "../math/Vector2D.h"
#include "../ecs/GameEntity.h"
#include "../ecs/GameSystem.h"
#include "../log/LogCat.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>

#include "../ecs/GameComponent.h"

namespace glimmer {
    class GameSystem;
    class GameEntity;
    class Saves;

    class WorldContext {
        void RegisterSystem(std::unique_ptr<GameSystem> system);

        void OnSystemActivationChanged(GameSystem *sys, bool active);

        /**
         * World Seed
         * 世界种子
         */
        const int seed;

        /**
         * Whether the registration system is allowed
         * 是否允许注册系统
         */
        bool allowRegisterSystem = false;

        /**
         * Entity to component list
         * 实体到组件列表
         */
        std::unordered_map<GameEntity::ID, std::vector<std::unique_ptr<GameComponent> > > entityComponents;

        /**
         * Components to quantity
         * 组件到数量
         */
        std::unordered_map<std::type_index, size_t> componentCount;


        /**
         * The player's position
         * 玩家的位置
         */
        Vector2D playerPosition;


        /**
         * Height map(Divided by blocks)
         * 高度图（按区块划分）
         * key：The x-coordinate of the block's starting point 区块起点X坐标
         * value：The height array of this block (length = CHUNK_SIZE)
         */
        std::unordered_map<int, std::vector<int> > heightMap;


        /**
         * 用于生成高度的噪声生成器
         */
        FastNoiseLite *heightMapNoise;

        std::vector<std::unique_ptr<GameSystem> > activeSystems;
        std::vector<std::unique_ptr<GameSystem> > inactiveSystems;
        /**
        * Game saves
        * 游戏存档
        */
        Saves *saves;

        /**
        * All loaded blocks
        * 所有已加载的区块
        */
        std::vector<std::shared_ptr<Chunk> > chunks;


        std::vector<std::unique_ptr<GameEntity> > entities;
        std::unordered_map<GameEntity::ID, GameEntity *> entityMap;

    public:
        const std::vector<std::shared_ptr<Chunk> > &GetChunks() const;

        void RegisterChunk(const std::shared_ptr<Chunk> &chunk);

        Saves *GetSaves() const;

        template<typename TComponent, typename... Args>
        TComponent *AddComponent(GameEntity *entity, Args &&... args);


        template<typename TComponent>
        bool HasComponent(GameEntity::ID id);

        /**
         * Is there a component of the specified type
         * 是否有指定类型的组件
         * @param type 类型
         * @return
         */
        bool HasComponentType(const std::type_index &type) const;


        /**
         * get Height Map
         * 获取高度图
         * @param x Starting point x coordinate 起点x坐标
         * @return The height array of this block (length = CHUNK_SIZE)
         */
        std::vector<int> GetHeightMap(int x);


        bool HandleEvent(const SDL_Event &event) const;

        void Update(float delta) const;

        void Render(SDL_Renderer *renderer) const;

        void OnFrameStart();

        void InitSystem();


        /**
         * Create an entity。
         * 创建一个实体
         * @return
         */
        GameEntity *CreateEntity();

        /**
         * Search for entities based on their ids
         * 根据 ID 查找实体
         * @param id
         * @return
         */
        GameEntity *GetEntity(GameEntity::ID id);

        /**
         * Remove Entity
         * 移除实体
         * @param id
         */
        void RemoveEntity(GameEntity::ID id);


        explicit WorldContext(const int seed, Vector2D playerPosition, Saves *saves) : seed(seed),
            playerPosition(playerPosition), saves(saves) {
            heightMapNoise = new FastNoiseLite();
            heightMapNoise->SetSeed(seed);
            heightMapNoise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
            heightMapNoise->SetFrequency(0.05f);
            InitSystem();
        }
    };

    template<typename TComponent, typename... Args>
    TComponent *WorldContext::AddComponent(GameEntity *entity, Args &&... args) {
        auto comp = std::make_unique<TComponent>(std::forward<Args>(args)...);
        TComponent *ptr = comp.get();
        entityComponents[entity->GetID()].push_back(std::move(comp));

        const auto type = std::type_index(typeid(TComponent));
        ++componentCount[type];

        //Notify all systems that may rely on this component
        // 通知所有可能依赖该组件的系统
        if (componentCount[type] == 1) {
            for (auto &sys: inactiveSystems) {
                if (sys && sys->SupportsComponentType(type)) {
                    sys->CheckActivation();
                }
            }
        }
        return ptr;
    }

    template<typename TComponent>
    bool WorldContext::HasComponent(const GameEntity::ID id) {
        const auto it = entityComponents.find(id);
        if (it == entityComponents.end()) return false;

        for (auto &c: it->second) {
            if (dynamic_cast<TComponent *>(c.get())) return true;
        }

        return false;
    }
}


#endif //GLIMMERWORKS_WORLDCONTEXT_H
