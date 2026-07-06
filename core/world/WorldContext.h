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
#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>

#include "LightBuffer.h"
#include "core/saves/Saves.h"
#include "box2d/id.h"
#include "core/ecs/EcsTypes.h"
#include "core/ecs/EntityManager.h"
#include "core/ecs/EntityShortCut.h"
#include "core/ecs/GameSystem.h"
#include "core/inventory/ComposableItem.h"
#include "generator/ChunkGenerator.h"
#include "generator/ChunkLoader.h"
#include "core/world/TileInstancePool.h"
#include "src/core/game_component_type.pb.h"
#include "src/saves/entity_item.pb.h"

namespace glimmer
{
    class ParallaxBackgroundComponent;
    class TileInstancePool;

    /**
     * GameEntity has been restricted to be accessed directly only within the WorldContext. uint32_t is provided externally.
     * GameEntity 已被限制为仅在WorldContext内部直接访问。对外提供uint32_t。
     */
    class WorldContext
    {
        void RegisterSystem(std::unique_ptr<GameSystem> system);

        uint32_t chunkSnapshot_ = 0;
        uint32_t lastChunkSnapshot_ = UINT32_MAX;

        uint32_t onComponentCountChangedId_ = 0;
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
        std::unordered_map<TileVector2D, std::unique_ptr<TerrainResult>, Vector2DIHash> terrainTileData_;
        //Mark the terrain coordinates that have been scanned and structured.(The coordinates are the vertices of the block.)When loading block A, block A can request and create the terrain data for block B, so as to write the structure information. Therefore, using this variable to determine whether the terrain of the block has been manually generated is completely correct.
        //标记已扫描过结构的地形坐标。（坐标为区块顶点。）当加载区块A时，区块A可以申请，创建区块B的地形数据，以便写入结构信息。所以使用这个变量判断区块的地形是否手动生成过是完全正确的。
        std::unordered_set<TileVector2D, Vector2DIHash> processedTerrainTiles_;
        std::unordered_map<TileVector2D, Chunk*, Vector2DIHash> chunksCache_;
        std::unordered_map<TileVector2D, TerrainResult*, Vector2DIHash> terrainTileDataCache_;

        std::vector<std::unique_ptr<GameSystem>> activeSystems_;
        std::vector<std::unique_ptr<GameSystem>> inactiveSystems_;
        std::unique_ptr<LightBuffer> lightBuffer_ = nullptr;
        std::stack<GameSystemType> activeSystemStack_;
        uint64_t persistentGuiSystemCount_ = 0;
        /**
        * Game saves
        * 游戏存档
        */
        Saves* saves_;

        b2WorldId worldId_ = b2_nullWorldId;
        MapManifest* mapManifest_ = nullptr;
        AppContext* appContext_;
        std::unique_ptr<EntityManager> entityManager_;
        std::unique_ptr<EntityShortCut> entityShortCut_;
        GameEntityID itemEditorPanel_ = GAME_ENTITY_ID_INVALID;
        std::unique_ptr<ChunkLoader> chunkLoader_ = nullptr;
        std::unique_ptr<ChunkGenerator> chunkGenerator_ = nullptr;
        std::unique_ptr<TileInstancePool> tileInstancePool_;
        std::shared_ptr<std::function<void(uint8_t, Item*, ContainerChangeType)>> itemCallback_;
        std::shared_ptr<AudioResourceResult> itemBreakSFXResult_ = nullptr;
        AudioManager* audioManager_ = nullptr;
        /**
         * Update the component total count cache area.
         * 组件总数变更缓存区。
         * For example, the number of component A in each frame may change N times. Therefore, we establish a buffer zone to limit the number of change notifications received by the system to one per frame.
         * 例如每帧内A组件的数量可能会改变N次，所以我们建立一个缓存区，来限制系统收到的变更通知为每帧一次。
         */
        std::unordered_map<GameComponentTypeMessage, uint32_t> onComponentCountChangeBuffer_;
        //Is the game being saved
        //是否正在保存游戏
        bool saving_ = false;

        /**
         * Whether to enable the item dragging mode
         * 是否启用物品拖拽模式
         */
        bool dragMode_ = false;

        /**
         * Whether it is running or not, if false, it indicates that the game has been paused.
         * 是否正在运行中，为false则表示游戏已被暂停。
         */
        bool running = true;


        /**
         * OnChunkTileChange
         * 当区块内的瓦片改变时
         * @param chunk chunk 区块
         * @param tile tile 瓦片
         * @param layerType layerType 图层类型
         * @param index index 索引
         */
        void OnChunkTileChange(Chunk* chunk, const std::shared_ptr<Tile>& tile, TileLayerType layerType,
                               int index) const;

        void UpdateTileLight(const Chunk* chunk, TileLayerType layerType, int index) const;

        /**
         * Update the lighting for the entire chunk.
         * 更新整个区块的光照。
         * @param chunk
         */
        void UpdateChunkLight(const Chunk* chunk) const;

        /**
     * Initialize the player
     * 初始化玩家
     * This method will load the player data from the disk and then supplement the player's components after the loading process.
     * 这个方法将从磁盘加载玩家数据，并在加载后补充玩家的组件。
     */
        void InitPlayer(const ResourceRef& resourceRef);

        /**
    * Initialize the hotbar
    * 初始化快捷栏
    */
        void InitHotbar(ItemContainer* itemContainer) const;

        /**
         * Initialize inventory view(Bag)
         * 初始化库存视图（背包）
         */
        void InitInventory(ItemContainer* itemContainer) const;

        void OnWatchedComponentChanged(GameComponentTypeMessage type, uint32_t count);

        void NotifySystemsOfComponentChange(GameComponentTypeMessage gameComponentType, uint32_t count);

        uint32_t CreateOrLoadPlayer(const ResourceRef& resourceRef);

        void InitPlayerInventory(uint32_t playerEntity);

        void OnPlayerItemChanged(uint8_t index, Item* item, ContainerChangeType changeType, uint32_t playerEntity);

        void HandleItemBreak(Item* item, uint32_t playerEntity);

        void DropComposableItemAbilities(ComposableItem* composableItem);

        void NotifyActiveSystems(GameComponentTypeMessage gameComponentType, uint32_t count) const;

        void NotifyInactiveSystems(GameComponentTypeMessage gameComponentType, uint32_t count);

    public:
        ~WorldContext();

        /**
       * PushGuiSystemType
       * 推送系统
       * @param systemType
       */
        void PushGuiSystemType(GameSystemType systemType);

        /**
         * PushPersistentGuiSystem
         * 推送不可关闭的系统
         * @param systemType
         */
        void PushPersistentGuiSystem(GameSystemType systemType);

        void PopGuiSystemType();

        GameSystemType GetGuiSystemType() const;


        [[nodiscard]] EntityManager* GetEntityManager() const;

        [[nodiscard]] EntityShortCut* GetEntityShortCut() const;

        [[nodiscard]] bool IsDragMode() const;

        void SetDragMode(bool dragMode);


        [[nodiscard]] TerrainResult* GetTerrainData(const TileVector2D& position);

        /**
* GetOrCreateTerrainData
* 创建或查找地形数据。
* If the terrain already exists, then return the terrain data; if the terrain does not exist, then create a blank set of data.
* 如果地形已存在，那么返回地形数据，如果地形不存在，那么创建一份空白数据。
* @return
*/
        [[nodiscard]] TerrainResult* GetOrCreateTerrainData(const TileVector2D& position);


        /**
         * Is the game running
         * 游戏是否正在运行中
         * @return
         */
        [[nodiscard]] bool IsRuning() const;


        [[nodiscard]] std::vector<GameSystemType> GetAllActiveSystemType() const;


        /**
         *Set the running
         * 设置运行状态
         * @param run
         */
        void SetRuning(bool run);

        Saves* GetSaves() const;

        MapManifest* GetMapManifest() const;

        /**
         * GetChunks
         * 获取区块
         * @return
         */
        std::unordered_map<TileVector2D, Chunk*, Vector2DIHash>* GetAllChunks();


        std::unordered_map<TileVector2D, TerrainResult*, Vector2DIHash>* GetTerrainResults();

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

        [[nodiscard]] ChunkGenerator* GetChunkGenerator() const;

        /**
         * Unload Chunk
         * 卸载区块
         * @param position position 位置
         */
        void UnloadChunkAt(const TileVector2D& position);


        /**
         * GetChunk
         * 获取指定位置的区块。
         * @param chunkVertex chunkVertex 区块顶点位置
         * @return
         */
        Chunk* GetChunk(TileVector2D chunkVertex);

        [[nodiscard]] TileInstancePool* GetTileInstancePool() const;

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
        void SaveEntity(EntityItemMessage* entityItemMessage, GameEntityID entityId) const;

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

        /**
         * Is there a GUI system that can be closed currently being displayed?
         * 是否有可关闭的GUI系统正在显示中。
         * @return
         */
        bool HasAnyModalGuiOpen() const;

        bool HandleEvent(const SDL_Event& event) const;

        void Update(float delta) const;

        bool OnBackPressed();

        void Render(SDL_Renderer* renderer) const;

        void RenderImGui(SDL_Renderer* renderer) const;

        void OnFrameStart();

        void InitSystem();

        void OnConfigChanged(const Config* config);


        [[nodiscard]] LightBuffer* GetLightingBuffer() const;


        /**
         * SaveGame
         * 保存游戏
         */
        void SaveGame();

        /**
         * Does the entity ID point to a blank entity?
         * 实体id是否指向空白实体
         * @param id
         * @return
         */
        [[nodiscard]] static bool IsEmptyEntityId(uint32_t id);

        /**
         * Get World Seed
         * 获取世界种子号
         * @return int 种子号
         */
        [[nodiscard]] int GetWorldSeed() const;

        long startTime_ = 0;

        explicit WorldContext(AppContext* appContext, MapManifest* mapManifest, Saves* saves);

        void OnWindowSizeChanged(const int& width, const int& height) const;

        [[nodiscard]] AppContext* GetAppContext() const;

        [[nodiscard]] b2WorldId GetWorldId() const;
    };
}
