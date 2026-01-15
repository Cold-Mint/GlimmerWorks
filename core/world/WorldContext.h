//
// Created by Cold-Mint on 2025/10/24.
//

#ifndef GLIMMERWORKS_WORLDCONTEXT_H
#define GLIMMERWORKS_WORLDCONTEXT_H

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "FastNoiseLite.h"
#include "../ecs/GameEntity.h"
#include "../ecs/GameSystem.h"
#include "../log/LogCat.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>

#include "Chunk.h"
#include "../ecs/GameComponent.h"
#include "../ecs/component/DiggingComponent.h"
#include "../ecs/component/HotBarComonent.h"
#include "../ecs/component/PauseComponent.h"
#include "../inventory/Item.h"
#include "../math/Vector2DI.h"
#include "../saves/Saves.h"
#include "../scene/AppContext.h"
#include "../utils/JsonUtils.h"
#include "../utils/TimeUtils.h"
#include "box2d/box2d.h"
#include "box2d/id.h"

namespace glimmer {
    class TileLayerComponent;
    class Transform2DComponent;
    class CameraComponent;
    class GameEntity;
    class Item;

    class WorldContext {
        void RegisterSystem(std::unique_ptr<GameSystem> system);

        uint32_t chunksVersion_ = 0;
        uint32_t lastChunksVersion_ = UINT32_MAX;

        /**
         * World Seed
         * 世界种子
         */
        int seed;

        /**
         * Whether the registration system is allowed
         * 是否允许注册系统
         */
        bool allowRegisterSystem = false;

        std::unordered_map<TileVector2D, std::unique_ptr<Chunk>, Vector2DIHash> chunks_;
        std::unordered_map<TileVector2D, Chunk *, Vector2DIHash> chunksCache_;
        GameEntity::ID entityId_ = 0;

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
         * camera Component
         * 相机组件
         */
        CameraComponent *cameraComponent_{};

        /**
         * Camera coordinate component
         * 相机坐标组件
         */
        Transform2DComponent *cameraTransform2D_{};

        /**
         * Pause component
         * 暂停组件
         */
        PauseComponent *pauseComponent_{};

        /**
         * HotBar Component
         * 快捷栏组件
         */
        HotBarComponent *hotBarComponent_{};

        /**
         * digging Component
         * 正在挖掘组件
         */
        DiggingComponent *diggingComponent_{};

        /**
         * Height map
         * 高度图
         */
        std::unordered_map<int, int> heightMap_;


        /**
         * Humidity chart
         * 湿度图
         * The starting x-coordinate of the block and the starting y-coordinate of the block
         * 区块的起点X坐标和区块的起点Y坐标
         */
        std::unordered_map<TileVector2D, float, Vector2DIHash> humidityMap;
        /**
         * temperature chart
         * 温度图
         */
        std::unordered_map<TileVector2D, float, Vector2DIHash> temperatureMap;
        std::unordered_map<TileVector2D, float, Vector2DIHash> weirdnessMap;
        std::unordered_map<TileVector2D, float, Vector2DIHash> erosionMap;
        /**
         * 用于生成大陆的噪声生成器
         */
        std::unique_ptr<FastNoiseLite> continentHeightMapNoise;
        /**
         * 用于生成山脉的噪声生成器
         */
        std::unique_ptr<FastNoiseLite> mountainHeightMapNoise;
        /**
         * 用于丘陵的噪声生成器
         */
        std::unique_ptr<FastNoiseLite> hillsNoiseHeightMapNoise;

        /**
         * 湿度噪声生成器
         */
        std::unique_ptr<FastNoiseLite> humidityMapNoise;

        /**
         * Odd value noise generator
         * 怪异值噪声生成器
         */
        std::unique_ptr<FastNoiseLite> weirdnessMapNoise;

        /**
         * Erosion noise generator
         * 侵蚀噪声生成器
         */
        std::unique_ptr<FastNoiseLite> erosionMapNoise;

        /**
         * 温度噪声生成器
         */
        std::unique_ptr<FastNoiseLite> temperatureMapNoise;

        std::vector<std::unique_ptr<GameSystem> > activeSystems;
        std::vector<std::unique_ptr<GameSystem> > inactiveSystems;
        /**
        * Game saves
        * 游戏存档
        */
        Saves *saves;

        b2WorldId worldId_ = b2_nullWorldId;
        std::vector<std::unique_ptr<GameEntity> > entities;
        std::unordered_map<GameEntity::ID, GameEntity *> entityMap;
        AppContext *appContext_;

        GameEntity *player_ = nullptr;

        /**
         * Whether it is running or not, if false, it indicates that the game has been paused.
         * 是否正在运行中，为false则表示游戏已被暂停。
         */
        bool running = true;

        void RemoveComponentInternal(GameEntity::ID id, GameComponent *comp);

        /**
         * RegisterEntity
         * 注册实体
         * @param entity
         */
        GameEntity *RegisterEntity(std::unique_ptr<GameEntity> entity);

        /**
         * UnRegisterEntity
         * 注销实体
         * @param id
         */
        void UnRegisterEntity(GameEntity::ID id);

    public:
        ~WorldContext() {
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


        [[nodiscard]] GameEntity::ID GetEntityIdIndex() const;

        /**
         * Is the game running
         * 游戏是否正在运行中
         * @return
         */
        [[nodiscard]] bool IsRuning() const;


        /**
         * Retrieve all the components that are mounted on a certain entity.
         * 获取某个实体上挂载的全部组件。
         * @param id id 实体id
         * @return
         */
        [[nodiscard]] std::vector<GameComponent *> GetAllComponents(GameEntity::ID id);

        [[nodiscard]] std::vector<GameSystem *> GetAllActiveSystem() const;

        /**
         *Set the running
         * 设置运行状态
         * @param run
         */
        void SetRuning(bool run);

        Saves *GetSaves() const;

        template<typename TComponent, typename... Args>
        TComponent *AddComponent(GameEntity::ID id, Args &&... args);


        template<typename TComponent>
        void RemoveComponent(GameEntity::ID id);


        template<typename TComponent>
        bool HasComponent(GameEntity::ID id);

        /**
         * 获取实体的指定类型组件
         * @tparam TComponent 组件类型
         * @param id 实体ID
         * @return 组件指针，如果不存在则返回nullptr
         */
        template<typename TComponent>
        TComponent *GetComponent(GameEntity::ID id);

        /**
         * Is there a component of the specified type
         * 是否有指定类型的组件
         * @param type 类型
         * @return
         */
        bool HasComponentType(const std::type_index &type) const;


        /**
         * Set player entity
         * 设置玩家实体
         * @param player
         */
        void SetPlayerEntity(GameEntity *player);

        /**
         * GetPlayerEntity
         * 获取玩家实体
         * @return
         */
        [[nodiscard]] GameEntity *GetPlayerEntity() const;

        /**
         * get Height
         * 获取高度
         * @param x Starting point x coordinate 起点x坐标
         * @return The height array of this block (length = CHUNK_SIZE)
         */
        int GetHeight(int x);

        /**
         * GetChunks
         * 获取区块
         * @return
         */
        std::unordered_map<TileVector2D, Chunk *, Vector2DIHash> *GetAllChunks();

        /**
         * Obtain the humidity value of a certain coordinate
         * 获取某个坐标的湿度值
         * @param tileVector2d tileVector2d 瓦片坐标
         * @return 湿度0-1
         */
        float GetHumidity(TileVector2D tileVector2d);

        /**
         * Obtain the temperature value of a certain coordinate
         * 获取某个坐标的温度值
         * @param tileVector2d tileVector2d 瓦片坐标
         * @param elevation elevation 海拔
         * @return 温度0-1
         */
        float GetTemperature(TileVector2D tileVector2d, float elevation);

        /**
         * Obtain the strange value of a certain coordinate
         * 获取某个坐标的怪异值
         * @param tileVector2d tileVector2d 瓦片坐标
         * @return 怪异0-1
         */
        float GetWeirdness(TileVector2D tileVector2d);


        /**
         *Obtain the degree of erosion at a certain coordinate
         * 获取某个坐标的侵蚀度
         * @param tileVector2d tileVector2d 瓦片坐标
         * @return 侵蚀0-1
         */
        float GetErosion(TileVector2D tileVector2d);

        /**
         * GetElevation
         * 获取海拔
         * @param height 高度
         * @return
         */
        [[nodiscard]] static float GetElevation(int height);

        /**
        * Load Chunk
        * 加载区块
        * @param position position 位置
        */
        void LoadChunkAt(TileVector2D position);

        /**
         * Unload Chunk
         * 卸载区块
         * @param position position 位置
         */
        void UnloadChunkAt(TileVector2D position);

        /**
         * SaveChunk
         * 保存某个区块
         * @param position
         */
        [[nodiscard]] bool SaveChunk(TileVector2D position);

        /**
         * Determine whether a block at a certain position has been loaded
         * 判断某个位置的区块是否被加载
         * @param position
         * @return
         */
        bool HasChunk(TileVector2D position) const;


        /**
         * Check whether the block exceeds the boundary
         * 检查区块是否超出边界
         * @param position 区块位置 position
         * @return Whether it exceeds the boundary 是否超出边界
         */
        static bool ChunkIsOutOfBounds(TileVector2D position);

        bool HandleEvent(const SDL_Event &event) const;

        void Update(float delta) const;

        bool OnBackPressed() const;

        void Render(SDL_Renderer *renderer) const;

        void OnFrameStart();

        void InitSystem(AppContext *appContext);

        /**
         * Set Camera Position
         * 设置相机位置组件
         * @param worldPositionComponent
         */
        void SetCameraPosition(Transform2DComponent *worldPositionComponent);


        /**
         * Set HotBar Component
         * 设置相机组件
         * @param hotbarComponent
         */
        void SetHotBarComponent(HotBarComponent *hotbarComponent);

        /**
         * SetDiggingComponent
         * 设置正在挖掘组件
         * @param diggingComponent
         */
        void SetDiggingComponent(DiggingComponent *diggingComponent);


        /**
         * Set Camera Component
         * 设置相机的组件
         * @param cameraComponent
         */
        void SetCameraComponent(CameraComponent *cameraComponent);


        DiggingComponent *GetDiggingComponent() const;

        CameraComponent *GetCameraComponent() const;


        Transform2DComponent *GetCameraTransform2D() const;


        HotBarComponent *GetHotBarComponent() const;

        /**
         * Create an entity。
         * 创建一个实体
         * @return
         */
        GameEntity *CreateEntity();

        /**
         * Recovery Entity
         * 恢复实体
         * @param entityItemMessage
         * @return
         */
        GameEntity *RecoveryEntity(const EntityItemMessage &entityItemMessage);

        /**
         * Recovery Component
         * 恢复组件
         * @param id id id
         * @param componentMessage componentMessage 组件消息
         * @return
         */
        GameComponent *RecoveryComponent(GameEntity::ID id, const ComponentMessage &componentMessage);

        /**
         * Create a dropping object entity
         * 创建一个掉落物实体
         * @param item
         * @param position
         * @param pickupCooldown
         * @return
         */
        GameEntity *CreateDroppedItemEntity(std::unique_ptr<Item> item, WorldVector2D position,
                                            float pickupCooldown = 0.0F);

        /**
         * Search for entities based on their ids
         * 根据 ID 查找实体
         * @param id
         * @return
         */
        GameEntity *GetEntity(GameEntity::ID id);


        /**
         * GetAllGameEntities
         * 获取所有游戏实体
         * @return
         */
        [[nodiscard]] std::vector<GameEntity *> GetAllGameEntities() const;


        /**
         * Obtain all game objects that need to be persisted
         * 获取所有需要持久化的游戏对象
         * @return
         */
        [[nodiscard]] std::vector<GameEntity *> GetAllPersistableEntities() const;

        /**
         * Remove Entity
         * 移除实体
         * @param id
         */
        void RemoveEntity(GameEntity::ID id);

        /**
         * 获取具有指定组件类型的所有实体
         * @tparam T 组件类型
         * @tparam Ts 其他组件类型
         * @return 具有所有指定组件类型的实体列表
         */
        template<typename T, typename... Ts>
        std::vector<GameEntity *> GetEntitiesWithComponents();

        /**
         * GetSeed
         * 获取种子号
         * @return int 种子号
         */
        [[nodiscard]] int GetSeed() const;

        long startTime_ = 0;

        explicit WorldContext(AppContext *appContext, const int seed, Saves *saves,
                              const GameEntity::ID entityId = 0) : seed(seed),
                                                                   entityId_(entityId), saves(saves) {
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
            startTime_ = TimeUtils::GetCurrentTimeMs();
        }

        /**
         * The time when the world environment is constructed and completed.
         * 世界环境构建完成的时间。
         * @return
         */
        [[nodiscard]] long GetStartTime() const;

        [[nodiscard]] b2WorldId GetWorldId() const;
    };


    namespace detail {
        // 基本情况：只有一个组件类型
        template<typename T>
        bool HasAllComponents(WorldContext *world, GameEntity::ID id) {
            return world->HasComponent<T>(id);
        }

        // 递归情况：多个组件类型
        template<typename T, typename U, typename... Ts>
        bool HasAllComponents(WorldContext *world, GameEntity::ID id) {
            return world->HasComponent<T>(id) && HasAllComponents<U, Ts...>(world, id);
        }
    }

    template<typename T, typename... Ts>
    std::vector<GameEntity *> WorldContext::GetEntitiesWithComponents() {
        std::vector<GameEntity *> result;

        // Traverse all entities
        // 遍历所有实体
        for (auto &entity: entities) {
            // Check if the entity has all specified component types
            // 检查实体是否有所有指定类型的组件
            if (detail::HasAllComponents<T, Ts...>(this, entity->GetID())) {
                result.push_back(entity.get());
            }
        }

        return result;
    }

    template<typename TComponent, typename... Args>
    TComponent *WorldContext::AddComponent(GameEntity::ID id, Args &&... args) {
        const auto type = std::type_index(typeid(TComponent));

        LogCat::d("Adding component ", type.name(), " to Entity ID = ", id);

        // 创建组件实例
        auto comp = std::make_unique<TComponent>(std::forward<Args>(args)...);
        TComponent *ptr = comp.get();
        entityComponents[id].push_back(std::move(comp));

        // 记录组件类型数量
        ++componentCount[type];
        LogCat::i("Component ", type.name(), " count = ", componentCount[type]);

        // 通知可能依赖该组件的系统
        if (componentCount[type] == 1) {
            LogCat::d("First instance of component ", type.name(),
                      " detected, checking inactive systems for activation...");

            for (auto &sys: inactiveSystems) {
                if (sys && sys->SupportsComponentType(type)) {
                    LogCat::d("System ", sys->GetName(),
                              " supports ", type.name(), ", checking activation...");
                    sys->CheckActivation();
                }
            }
        }

        LogCat::d("Component ", type.name(), " ptr=", ptr, " successfully added to Entity ID = ", id);
        return ptr;
    }

    template<typename TComponent>
    void WorldContext::RemoveComponent(GameEntity::ID id) {
        auto it = entityComponents.find(id);
        if (it == entityComponents.end()) return;

        auto &components = it->second;
        for (auto &c: components) {
            if (auto ptr = dynamic_cast<TComponent *>(c.get())) {
                RemoveComponentInternal(id, ptr);
                break;
            }
        }
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

    template<typename TComponent>
    TComponent *WorldContext::GetComponent(GameEntity::ID id) {
        const auto it = entityComponents.find(id);
        if (it == entityComponents.end()) return nullptr;

        for (auto &c: it->second) {
            if (auto *component = dynamic_cast<TComponent *>(c.get())) {
                return component;
            }
        }

        return nullptr;
    }
}


#endif //GLIMMERWORKS_WORLDCONTEXT_H
