//
// Created by Cold-Mint on 2025/10/24.
//

#ifndef GLIMMERWORKS_WORLDCONTEXT_H
#define GLIMMERWORKS_WORLDCONTEXT_H

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "../ecs/GameEntity.h"
#include "../log/LogCat.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>

#include "../ecs/GameComponent.h"
#include "../ecs/component/DiggingComponent.h"
#include "../ecs/component/PauseComponent.h"
#include "../inventory/Item.h"
#include "../math/Vector2DI.h"
#include "../saves/Saves.h"
#include "../scene/AppContext.h"
#include "box2d/id.h"
#include "core/ecs/component/ItemEditorComponent.h"
#include "../ecs/GameSystem.h"
#include "core/ecs/component/CameraComponent.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/inventory/ComposableItem.h"
#include "generator/ChunkGenerator.h"
#include "generator/ChunkLoader.h"

namespace glimmer {
    /**
     * GameEntity has been restricted to be accessed directly only within the WorldContext. GameEntity::ID is provided externally.
     * GameEntity 已被限制为仅在WorldContext内部直接访问。对外提供GameEntity::ID。
     */
    class WorldContext {
        void RegisterSystem(std::unique_ptr<GameSystem> system);

        uint32_t chunksVersion_ = 0;
        uint32_t lastChunksVersion_ = UINT32_MAX;

        /**
         * World Seed
         * 世界种子
         */
        int worldSeed_;

        /**
         * Whether the registration system is allowed
         * 是否允许注册系统
         */
        bool allowRegisterSystem = false;

        std::unordered_map<TileVector2D, std::unique_ptr<Chunk>, Vector2DIHash> chunks_;
        //Save all the terrain result data.
        //保存所有的地形结果数据。
        std::unordered_map<TileVector2D, std::unique_ptr<TerrainResult>, Vector2DIHash> terrainResults_;
        //Mark the terrain coordinates that have been scanned and structured.(The coordinates are the vertices of the block.)When loading block A, block A can request and create the terrain data for block B, so as to write the structure information. Therefore, using this variable to determine whether the terrain of the block has been manually generated is completely correct.
        //标记已扫描过结构的地形坐标。（坐标为区块顶点。）当加载区块A时，区块A可以申请，创建区块B的地形数据，以便写入结构信息。所以使用这个变量判断区块的地形是否手动生成过是完全正确的。
        std::unordered_set<TileVector2D, Vector2DIHash> terrain_;
        std::unordered_map<TileVector2D, Chunk *, Vector2DIHash> chunksCache_;
        std::unordered_map<TileVector2D, TerrainResult *, Vector2DIHash> terrainResultsCache_;
        GameEntity::ID entityId_ = GAME_ENTITY_ID_INVALID;

        /**
         * Entity to component list
         * 实体到组件列表
         */
        std::unordered_map<GameEntity::ID, std::vector<std::unique_ptr<GameComponent> > > entityComponents;

        /**
         * Components to quantity
         * 组件到数量
         */
        std::unordered_map<std::type_index, size_t> componentCount_;


        /**
         * camera Component
         * 相机组件
         */
        CameraComponent *cameraComponent_{};

        ItemEditorComponent *itemEditorComponent_{};

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
        GameEntity::ID hotBarEntity = GAME_ENTITY_ID_INVALID;

        /**
         * digging Component
         * 正在挖掘组件
         */
        DiggingComponent *diggingComponent_{};

        std::vector<std::unique_ptr<GameSystem> > activeSystems;
        std::vector<std::unique_ptr<GameSystem> > inactiveSystems;
        /**
        * Game saves
        * 游戏存档
        */
        Saves *saves_;

        b2WorldId worldId_ = b2_nullWorldId;
        std::vector<std::unique_ptr<GameEntity> > entities_;
        std::unordered_map<GameEntity::ID, GameEntity *> entityMap_;
        AppContext *appContext_;

        GameEntity::ID player_ = GAME_ENTITY_ID_INVALID;
        GameEntity::ID itemEditorPanel_ = GAME_ENTITY_ID_INVALID;
        std::unique_ptr<ChunkLoader> chunkLoader_;
        std::unique_ptr<ChunkGenerator> chunkGenerator_;

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
        GameEntity::ID RegisterEntity(std::unique_ptr<GameEntity> entity);

        /**
         * UnRegisterEntity
         * 注销实体
         * @param id
         */
        void UnRegisterEntity(GameEntity::ID id);

    public:
        ~WorldContext();


        [[nodiscard]] GameEntity::ID GetEntityIdIndex() const;

        [[nodiscard]] TerrainResult *GetTerrainData(TileVector2D position);

        /**
* GetOrCreateTerrainData
* 创建或查找地形数据。
* If the terrain already exists, then return the terrain data; if the terrain does not exist, then create a blank set of data.
* 如果地形已存在，那么返回地形数据，如果地形不存在，那么创建一份空白数据。
* @return
*/
        [[nodiscard]] TerrainResult *GetOrCreateTerrainData(TileVector2D position);


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
         * Initialize the player
         * 初始化玩家
         * This method will load the player data from the disk and then supplement the player's components after the loading process.
         * 这个方法将从磁盘加载玩家数据，并在加载后补充玩家的组件。
         */
        void InitPlayer(const ResourceRef &resourceRef);

        /**
         * Initialize the hotbar
         * 初始化快捷栏
         */
        void InitHotbar(ItemContainer *itemContainer);


        void InitItemEditor();

        [[nodiscard]] ItemEditorComponent *GetItemEditorComponent() const;


        /**
         * GetPlayerEntity
         * 获取玩家实体
         * @return
         */
        [[nodiscard]] GameEntity::ID GetPlayerEntity() const;


        /**
         * GetChunks
         * 获取区块
         * @return
         */
        std::unordered_map<TileVector2D, Chunk *, Vector2DIHash> *GetAllChunks();


        std::unordered_map<TileVector2D, TerrainResult *, Vector2DIHash> *GetTerrainResults();

        /**
         * LoadTerrainAt
         * 加载地形
         * @param position position 位置
         */
        void LoadTerrainAt(TileVector2D position);

        void UnloadTerrainAt(TileVector2D position);

        /**
        * Load Chunk
        * 加载区块
        * @param position position 位置
        */
        void LoadChunkAt(TileVector2D position);

        ChunkGenerator *GetChunkGenerator() const;


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
         * SaveEntity
         * 保存实体
         * @param entityItemMessage
         * @param entityId
         * @return
         */
        void SaveEntity(EntityItemMessage *entityItemMessage, GameEntity::ID entityId);

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

        void InitSystem();

        /**
         * Set Camera Position
         * 设置相机位置组件
         * @param worldPositionComponent
         */
        void SetCameraPosition(Transform2DComponent *worldPositionComponent);


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


        [[nodiscard]] GameEntity::ID GetHotBarEntity() const;

        /**
         * Create an entity。
         * 创建一个实体
         * @return
         */
        GameEntity::ID CreateEntity();

        /**
      * Recovery Component
      * 恢复组件
      * @param id id id
      * @param componentMessage componentMessage 组件消息
      * @return
      */
        [[nodiscard]] GameComponent *RecoveryComponent(GameEntity::ID id,
                                                       const ComponentMessage &componentMessage);

        /**
         * Display the item editing panel
         * 显示物品编辑面板
         * @param composableItem
         */
        void ShowItemEditorPanel(const ComposableItem *composableItem);

        /**
         * Is the item editing panel currently displayed?
         * 物品编辑面板是否在显示中。
         * @return
         */
        bool IsItemEditorPanelVisible() const;

        /**
         * Hidden Items Editing Panel
         * 隐藏物品编辑面板
         */
        void HideItemEditorPanel();


        /**
         * Set up the persistence of game entities
         * 设置游戏实体持久化
         * @param id
         * @param persistable
         * @return
         */
        bool SetPersistable(GameEntity::ID id, bool persistable);

        bool SetResourceRef(GameEntity::ID id, const ResourceRef &resourceRef);

        const ResourceRef *GetResourceRef(GameEntity::ID id);

        /**
         * Is the game entity persistent?
         * 游戏实体是否为持久化的
         * @param id
         * @return
         */
        bool IsPersistable(GameEntity::ID id);


        /**
         * GetAllGameEntitiesId
         * 获取所有游戏实体Id
         * @return
         */
        [[nodiscard]] std::vector<GameEntity::ID> GetAllGameEntityId() const;

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
        std::vector<GameEntity::ID> GetEntityIDWithComponents();

        /**
         * Does the entity ID point to a blank entity?
         * 实体id是否指向空白实体
         * @param id
         * @return
         */
        [[nodiscard]] static bool IsEmptyEntityId(GameEntity::ID id);

        /**
         * Get World Seed
         * 获取世界种子号
         * @return int 种子号
         */
        [[nodiscard]] int GetWorldSeed() const;

        long startTime_ = 0;

        explicit WorldContext(AppContext *appContext, int worldSeed, Saves *saves, GameEntity::ID entityId = 0);

        /**
         * The time when the world environment is constructed and completed.
         * 世界环境构建完成的时间。
         * @return
         */
        [[nodiscard]] long GetStartTime() const;

        [[nodiscard]] AppContext *GetAppContext() const;

        [[nodiscard]] b2WorldId GetWorldId() const;
    };


    namespace detail {
        // Basic situation: There is only one component type.
        // 基本情况：只有一个组件类型
        template<typename T>
        bool HasAllComponents(WorldContext *world, const GameEntity::ID id) {
            return world->HasComponent<T>(id);
        }

        // Recursive situation: Multiple component types
        // 递归情况：多个组件类型
        template<typename T, typename U, typename... Ts>
        bool HasAllComponents(WorldContext *world, const GameEntity::ID id) {
            return world->HasComponent<T>(id) && HasAllComponents<U, Ts...>(world, id);
        }
    }

    template<typename T, typename... Ts>
    std::vector<GameEntity::ID> WorldContext::GetEntityIDWithComponents() {
        std::vector<GameEntity::ID> result;

        // Traverse all entities
        // 遍历所有实体
        for (auto &entity: entities_) {
            // Check if the entity has all specified component types
            // 检查实体是否有所有指定类型的组件
            if (detail::HasAllComponents<T, Ts...>(this, entity->GetID())) {
                result.push_back(entity->GetID());
            }
        }

        return result;
    }

    template<typename TComponent>
    TComponent *WorldContext::GetComponent(const GameEntity::ID id) {
        const auto it = entityComponents.find(id);
        if (it == entityComponents.end()) {
            return nullptr;
        }
        for (auto &component: it->second) {
            if (component == nullptr) {
                continue;
            }
            GameComponent *gameComponent = component.get();
            auto *target = dynamic_cast<TComponent *>(gameComponent);
            if (target == nullptr) {
                continue;
            }
            return target;
        }

        return nullptr;
    }

    /**
     * Add a component to the entity. If a component of the same type already exists on the entity, then return the existing pointer.
     * 添加组件到实体，如果同类型组件已存在于实体上，那么返回已存在的指针。
     * @tparam TComponent
     * @tparam Args
     * @param id
     * @param args
     * @return
     */
    template<typename TComponent, typename... Args>
    TComponent *WorldContext::AddComponent(GameEntity::ID id, Args &&... args) {
        const auto it = entityMap_.find(id);
        if (it == entityMap_.end()) {
            LogCat::e("Entity ", id, " does not exist.");
#if  !defined(NDEBUG)
            assert(false);
#endif
            return nullptr;
        }
        const auto components = entityComponents.find(id);
        if (components != entityComponents.end()) {
            for (auto &component: components->second) {
                if (component == nullptr) {
                    continue;
                }
                GameComponent *gameComponent = component.get();
                auto *target = dynamic_cast<TComponent *>(gameComponent);
                if (target == nullptr) {
                    continue;
                }
                return target;
            }
        }

        const auto type = std::type_index(typeid(TComponent));
        auto comp = std::make_unique<TComponent>(std::forward<Args>(args)...);
        TComponent *ptr = comp.get();
        entityComponents[id].push_back(std::move(comp));
        ++componentCount_[type];
        if (componentCount_[type] == 1) {
            for (const auto &sys: inactiveSystems) {
                if (sys && sys->SupportsComponentType(type)) {
                    sys->CheckActivation();
                }
            }
        }
        return ptr;
    }

    template<typename TComponent>
    void WorldContext::RemoveComponent(GameEntity::ID id) {
        const auto it = entityComponents.find(id);
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
        if (it == entityComponents.end()) {
            return false;
        }

        for (auto &component: it->second) {
            if (component == nullptr) {
                continue;
            }
            GameComponent *gameComponent = component.get();
            if (dynamic_cast<TComponent *>(gameComponent) == nullptr) {
                continue;
            }
            return true;
        }

        return false;
    }
}


#endif //GLIMMERWORKS_WORLDCONTEXT_H
