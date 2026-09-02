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
        std::unordered_map<TileVector2D, std::unique_ptr<TileLightData>, Vector2DIHash> tileLightData_;

        /**
         * Monotonic counter bumped by every mutating operation. The renderer
         * compares it frame to frame to know when the cached light map
         * texture must be rebuilt (dirty tracking).
         * 每次修改操作都会递增的单调计数器。渲染器逐帧比较它，
         * 以判断缓存的光照贴图纹理是否需要重建（脏标记跟踪）。
         */
        uint64_t revision_ = 0;

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
         * @return The current revision counter. Any change to the buffered
         * light data (sources, masks, contributions) increments it.
         * 当前修订计数器。任何对光照缓冲数据（光源、遮罩、贡献）的修改都会使其递增。
         */
        [[nodiscard]] uint64_t GetRevision() const {
            return revision_;
        }
    };
}
