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

namespace glimmer {
    class WorldContext;
    class ChunkManager;
    class TerrainManager;
    class ChunkGenerator;
    class ChunkLoader;
    class WeatherManager;
    struct DimensionResource;

    /**
     * Dimension
     * 维度
     * A dimension holds its own world generation, chunk storage, terrain and time state.
     * 维度持有独立的世界生成、区块存储、地形和时间状态。
     */
    class Dimension {
        WorldContext *worldContext_ = nullptr;

        /**
         * The id of the dimension ("packId:resourceId"). Used for biome matching.
         * 维度Id（packId:resourceId），用于生物群系匹配。
         */
        std::string dimensionId_;

        /**
         * The folder name of the dimension inside the save ("packId_resourceId").
         * 维度在存档内的目录名（packId_resourceId）。
         */
        std::string dimensionFolderName_;

        DimensionResource *dimensionResource_ = nullptr;

        /**
         * Current time of day in the range [0, 1). 0 = morning, 0.5 = midnight, 1 wraps to 0 (next morning).
         * 当前时间（0..1）。0代表清晨，0.5代表午夜，1回绕到0（次日清晨）。
         */
        float timeOfDay_ = 0.0F;

        /**
         * Time flow speed. 0 disables the day/night cycle.
         * 时间流动速度。0则禁用昼夜循环。
         */
        float timeFlowSpeed_ = 1.0F;

        /**
         * Initial time on first entry (0..1).
         * 首次进入维度的时间起点（0..1）。
         */
        float initialTime_ = 0.0F;

        std::unique_ptr<ChunkManager> chunkManager_;
        std::unique_ptr<TerrainManager> terrainManager_;
        std::unique_ptr<ChunkGenerator> chunkGenerator_;
        std::unique_ptr<ChunkLoader> chunkLoader_;
        std::unique_ptr<WeatherManager> weatherManager_;

    public:
        Dimension(WorldContext *worldContext, DimensionResource *dimensionResource);

        ~Dimension();

        /**
         * Init
         * 初始化维度（创建各管理器并恢复/设置时间）。
         */
        void Init();

        /**
         * SaveTime
         * 将当前时间写入维度清单。
         */
        void SaveTime() const;

        [[nodiscard]] const std::string &GetDimensionId() const;

        [[nodiscard]] const std::string &GetDimensionFolderName() const;

        [[nodiscard]] DimensionResource *GetDimensionResource() const;

        [[nodiscard]] ChunkManager *GetChunkManager() const;

        [[nodiscard]] TerrainManager *GetTerrainManager() const;

        [[nodiscard]] ChunkGenerator *GetChunkGenerator() const;

        [[nodiscard]] ChunkLoader *GetChunkLoader() const;

        [[nodiscard]] WeatherManager *GetWeatherManager() const;

        /**
         * GetTimeOfDay
         * 获取当前时间（0..1）。
         */
        [[nodiscard]] float GetTimeOfDay() const;

        /**
         * SetTimeOfDay
         * 设置当前时间（0..1），自动回绕到 [0,1)。
         */
        void SetTimeOfDay(float time);

        /**
         * AdvanceTime
         * 推进时间。timeFlowSpeed 为 0 时时间冻结。
         * @param delta delta 上一帧耗时（秒）
         * @param dayLengthSeconds dayLengthSeconds 一整天对应的现实时长（秒）
         */
        void AdvanceTime(float delta, float dayLengthSeconds);

        [[nodiscard]] float GetTimeFlowSpeed() const;

        [[nodiscard]] float GetInitialTime() const;
    };
}
