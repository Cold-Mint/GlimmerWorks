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

#include <unordered_map>

#include "LightMask.h"
#include "LightSource.h"
#include "TileLightData.h"
#include "core/math/Color.h"
#include "core/math/Vector2DIHash.h"


namespace glimmer {
    class TileVector2D;

    /**
     * LightBuffer
     * 光照缓冲
     *
     * Stores per-tile lighting (sources, masks, contributions and final color)
     * and propagates light using an 8-directional flood fill instead of the
     * previous ray-casting approach.
     * 存储逐瓦片光照（光源、遮罩、贡献与最终颜色），并采用 8 方向泛洪
     * 传播光照，取代此前的射线方案。
     */
    class LightBuffer {
        struct DynamicLightEntry {
            TileVector2D position;
            TileLayerType layer;
            std::unique_ptr<LightSource> lightSource;
        };

        std::unordered_map<TileVector2D, std::unique_ptr<TileLightData>, Vector2DIHash> tileLightData_;

        //Dynamic (mobile) light sources keyed by an arbitrary id (e.g. entity id).
        //动态（移动）光源，以任意 id（如实体 id）为键。
        std::unordered_map<uint64_t, DynamicLightEntry> dynamicLights_;

        /**
         * Monotonic counter bumped by every mutating operation. The renderer
         * compares it frame to frame to know when the cached light map
         * texture must be rebuilt (dirty tracking).
         * 每次修改操作都会递增的单调计数器。渲染器逐帧比较它，
         * 以判断缓存的光照贴图纹理是否需要重建（脏标记跟踪）。
         */
        uint64_t revision_ = 0;

        //The highest opaque Ground-layer tile y for each column, used to
        //derive sky visibility (ambient light only reaches tiles above it).
        //每列最高的不透明地面层瓦片 y，用于推导天空可见度（环境光只到达其上的瓦片）。
        std::unordered_map<int, int> columnSkyTopY_;

        //Whether batch mode is active (chunk load suppresses per-tile rebuilds).
        //批量模式是否激活（区块加载时抑制逐瓦片重算）。
        bool batching_ = false;
        bool batchDirty_ = false;

        TileLightData &GetOrCreate(const TileVector2D &position);

        void SetLightFromSource(const LightSource &source, TileLayerType layerType);

        void ClearLightFromSource(const LightSource &source, TileLayerType layerType);

        void SetLightContributionAt(const TileVector2D &position, TileLayerType layerType, const LightSource &source);

        void ClearLightContributionAt(const TileVector2D &position, TileLayerType layerType, const LightSource &source);

        void RebuildAllLight();

        void RecalculateColumnSkyTopY(int x);

    public:
        void SetSideLightMask(TileVector2D position, TileLayerType layerType, std::unique_ptr<LightMask> sideLightMask);

        void SetBackLightMask(TileVector2D position, TileLayerType layerType, std::unique_ptr<LightMask> backLightMask);

        void ClearSideLightMask(const TileVector2D &position, TileLayerType layerType);

        void ClearBackLightMask(const TileVector2D &position, TileLayerType layerType);

        // Clear mask data only without light re-propagation (safe for stale data removal)
        // 仅清除掩码数据，不重新传播光线（安全用于清除过期数据）
        void ClearSideLightMaskOnly(const TileVector2D &position, TileLayerType layerType);

        void ClearBackLightMaskOnly(const TileVector2D &position, TileLayerType layerType);

        void ClearTileLightData(const TileVector2D &position);

        [[nodiscard]] const TileLightData *GetTileLightData(const TileVector2D &position) const;

        void SetLightSource(TileVector2D position, TileLayerType layerType, std::unique_ptr<LightSource> lightSource);

        void ClearLightSource(TileVector2D position, TileLayerType layerType);

        const Color *GetFinalLightColor(TileVector2D position) const;

        /**
         * SetDynamicLight
         * 设置/更新一个动态（移动）光源。若 id 已存在且位置/光源参数变化，
         * 会先清除旧贡献再设置新贡献（原子移动，不残留）。
         * @param id id 光源唯一标识（如实体 id）
         * @param position position 光源所在瓦片
         * @param layerType layerType 图层
         * @param lightSource lightSource 光源数据
         */
        void SetDynamicLight(uint64_t id, TileVector2D position, TileLayerType layerType,
                             std::unique_ptr<LightSource> lightSource);

        /**
         * RemoveDynamicLight
         * 移除指定 id 的动态光源。
         * @param id id 光源唯一标识
         */
        void RemoveDynamicLight(uint64_t id);

        /**
         * SetTileOpaque
         * 设置指定瓦片在指定图层是否阻挡光线。当数值变化时会触发一次全量重算。
         * @param position position 瓦片世界坐标
         * @param layerType layerType 图层类型
         * @param opaque opaque 是否阻挡
         */
        void SetTileOpaque(TileVector2D position, TileLayerType layerType, bool opaque);

        /**
         * BeginBatch
         * 进入批量模式，抑制 SetTileOpaque 触发的逐次重算，直到 EndBatch。
         */
        void BeginBatch();

        /**
         * EndBatch
         * 退出批量模式；若期间发生了 opaque 变化，则统一重算一次。
         */
        void EndBatch();

        /**
         * GetSkyVisibility
         * 获取指定瓦片的天空可见度（1 = 露天，0 = 被上方不透明瓦片遮挡）。
         * 用于决定环境光（天光）是否照射到该瓦片。
         * @param position position 瓦片世界坐标
         */
        [[nodiscard]] float GetSkyVisibility(const TileVector2D &position) const;

        /**
         * @return The current revision counter. Any change to the buffered
         * light data (sources, masks, contributions) increments it.
         * 当前修订计数器。任何对光照缓冲数据（光源、遮罩、贡献）的修改都会使其递增。
         */
        [[nodiscard]] uint64_t GetRevision() const {
            return revision_;
        }
    };
}
