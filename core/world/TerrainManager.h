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
#include <unordered_set>

#include "core/ecs/EcsTypes.h"
#include "core/math/Vector2DIHash.h"
#include "generator/TerrainResult.h"

namespace glimmer
{
    class WorldContext;

    /**
     * TerrainManager
     * 地形管理器，负责地形数据的生成、缓存和卸载。
     * 从 WorldContext 拆分而来。
     */
    class TerrainManager
    {
        std::unordered_map<TileVector2D, std::unique_ptr<TerrainResult>, Vector2DIHash> terrainTileData_;
        std::unordered_map<TileVector2D, TerrainResult*, Vector2DIHash> terrainTileDataCache_;
        std::unordered_set<TileVector2D, Vector2DIHash> processedTerrainTiles_;
        WorldContext* worldContext_ = nullptr;

    public:
        explicit TerrainManager(WorldContext* worldContext);

        [[nodiscard]] TerrainResult* GetTerrainData(const TileVector2D& position);

        /**
        * GetOrCreateTerrainData
        * 创建或查找地形数据。
        * If the terrain already exists, then return the terrain data; if the terrain does not exist, then create a blank set of data.
        * 如果地形已存在，那么返回地形数据，如果地形不存在，那么创建一份空白数据。
        * @return
        */
        [[nodiscard]] TerrainResult* GetOrCreateTerrainData(const TileVector2D& position);

        [[nodiscard]] std::unordered_map<TileVector2D, TerrainResult*, Vector2DIHash>* GetTerrainResults();

        /**
         * LoadTerrainAt
         * 加载地形
         * @param position position 位置
         */
        void LoadTerrainAt(TileVector2D position);

        void UnloadTerrainAt(TileVector2D position);
    };
}
