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
#include <string>
#include <unordered_map>
#include <vector>

#include "LightBuffer.h"
#include "core/saves/Saves.h"
#include "box2d/id.h"
#include "core/ecs/EcsTypes.h"
#include "core/ecs/EntityManager.h"
#include "core/ecs/EntityShortCut.h"

#include "generator/ChunkGenerator.h"
#include "generator/ChunkLoader.h"
#include "src/saves/entity_item.pb.h"

namespace glimmer {
    class Scene;
    class ParallaxBackgroundComponent;
    class TileInstancePool;
    class ChunkManager;
    class TerrainManager;
    class SystemScheduler;
    class PlayerContext;
    class Dimension;
    struct DimensionResource;
    struct ResourceRef;
    struct LightKeyframe;
    struct SkyColorKeyframe;

    /**
     * GameEntity has been restricted to be accessed directly only within the WorldContext. uint32_t is provided externally.
     * GameEntity 已被限制为仅在WorldContext内部直接访问。对外提供uint32_t。
     */
    class WorldContext {
        /**
         * World Seed
         * 世界种子
         */
        int worldSeed_;

        /**
        * Game saves
        * 游戏存档
        */
        Saves *saves_;

        b2WorldId worldId_ = b2_nullWorldId;
        MapManifest *mapManifest_ = nullptr;
        AppContext *appContext_;
        std::unique_ptr<EntityManager> entityManager_;
        std::unique_ptr<EntityShortCut> entityShortCut_;

        /**
         * All dimensions of this world, keyed by dimension id ("packId:resourceId").
         * 该世界中的所有维度，以维度Id为键。
         */
        std::unordered_map<std::string, std::unique_ptr<Dimension> > dimensions_;

        /**
         * The currently active dimension.
         * 当前活动的维度。
         */
        Dimension *currentDimension_ = nullptr;

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
         * Real-world duration of a full in-game day, in seconds.
         * 一整天对应的现实时长（秒）。
         */
        float dayLengthSeconds_ = 600.0F;

        long startTime_ = 0;
        std::unique_ptr<SystemScheduler> systemScheduler_;
        std::unique_ptr<PlayerContext> playerContext_;

        /**
         * GetOrCreateDimension
         * 获取或创建指定维度资源的运行时维度。
         * @param dimensionResource dimensionResource 维度资源
         * @return
         */
        Dimension *GetOrCreateDimension(DimensionResource *dimensionResource);

    public:
        ~WorldContext();


        WorldContext(AppContext *appContext, MapManifest *mapManifest, Saves *saves);

        [[nodiscard]] EntityManager *GetEntityManager() const;

        [[nodiscard]] EntityShortCut *GetEntityShortCut() const;

        [[nodiscard]] Saves *GetSaves() const;

        [[nodiscard]] MapManifest *GetMapManifest() const;

        [[nodiscard]] AppContext *GetAppContext() const;

        [[nodiscard]] b2WorldId GetWorldId() const;

        [[nodiscard]] int GetWorldSeed() const;

        /**
         * GetCurrentDimension
         * 获取当前维度。
         */
        [[nodiscard]] Dimension *GetCurrentDimension() const;

        /**
         * GetCurrentDimensionId
         * 获取当前维度的Id。
         */
        [[nodiscard]] std::string GetCurrentDimensionId() const;

        /**
         * GetAmbientLightKeyframes
         * 获取当前维度的环境光关键帧；无当前维度时返回默认关键帧。
         */
        [[nodiscard]] const std::vector<LightKeyframe> &GetAmbientLightKeyframes() const;

        /**
         * GetSkyColorKeyframes
         * 获取当前维度的天空颜色关键帧；无当前维度时返回默认关键帧。
         */
        [[nodiscard]] const std::vector<SkyColorKeyframe> &GetSkyColorKeyframes() const;

        /**
         * GetElapsedDays
         * 获取此存档经历的游戏天数（世界级）。
         */
        [[nodiscard]] long GetElapsedDays() const;

        /**
         * GetMoonPhase
         * 获取当前月相（0..MOON_PHASE_COUNT-1）。
         */
        [[nodiscard]] uint8_t GetMoonPhase() const;

        /**
         * OnDayAdvanced
         * 当某个维度的时间回绕（1→0）时调用，使世界天数 +1。
         */
        void OnDayAdvanced();

        /**
         * GetWeatherIntensity
         * 获取当前维度的天气强度。
         */
        [[nodiscard]] float GetWeatherIntensity() const;

        /**
         * SwitchDimension
         * 切换到指定维度。会保存当前维度时间、卸载当前维度区块并加载目标维度。
         * @param dimensionRef dimensionRef 目标维度引用
         */
        void SwitchDimension(const ResourceRef &dimensionRef);

        /**
         * GetTimeOfDay
         * 获取当前维度的时间（0..1）。
         */
        [[nodiscard]] float GetTimeOfDay() const;

        /**
         * SetTimeOfDay
         * 设置当前维度的时间（0..1），自动环绕到 [0,1)。
         * @param time time 时间
         */
        void SetTimeOfDay(float time);

        /**
         * AdvanceTime
         * 推进当前维度的时间（暂停或时间流动速度为0时不流逝）。
         * @param delta delta 上一帧耗时（秒）
         */
        void AdvanceTime(float delta);

        /**
         * GetDayLengthSeconds
         * 获取一天对应的现实时长（秒）。
         */
        [[nodiscard]] float GetDayLengthSeconds() const;

        [[nodiscard]] ChunkGenerator *GetChunkGenerator() const;

        [[nodiscard]] ChunkLoader *GetChunkLoader() const;

        [[nodiscard]] ChunkManager *GetChunkManager() const;

        [[nodiscard]] TerrainManager *GetTerrainManager() const;

        [[nodiscard]] SystemScheduler *GetSystemScheduler() const;

        [[nodiscard]] PlayerContext *GetPlayerContext() const;

        [[nodiscard]] bool IsRuning() const;

        void SetRuning(bool run);

        [[nodiscard]] bool IsDragMode() const;

        void SetDragMode(bool dragMode);

        void SaveEntity(EntityItemMessage *entityItemMessage, GameEntityID entityId) const;

        void SaveGame();

        [[nodiscard]] static bool IsEmptyEntityId(uint32_t id);

        [[nodiscard]] LightBuffer *GetLightingBuffer() const;

        [[nodiscard]] TileInstancePool *GetTileInstancePool() const;
    };
}
