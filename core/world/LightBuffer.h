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

#include <set>
#include <unordered_map>

#include "DynamicLightEntry.h"
#include "LightMask.h"
#include "LightSource.h"
#include "TileLightData.h"
#include "core/config/Constants.h"
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

        //For each column, the sorted y of Ground-layer tiles that block the
        //Downward (sky) light. Sky transmittance at a tile is the product of
        //(1 - occlusion) of every such tile above it, replacing the previous
        //single-ceiling + depth falloff model with continuous transmittance.
        //每列中阻挡天光（Downward）的 Ground 层瓦片 y（升序）。某瓦片的天光透射率
        //等于其上方所有此类瓦片 (1 - 挡光强度) 的连乘，以此取代原先的单天花板 +
        //深度衰减模型，实现连续透射率。
        std::unordered_map<int, std::set<int> > columnSkyOccluders_;

        //Whether any static light data (tile sources, masks or ambient color)
        //changed since the last flush. Static changes trigger a full recompute
        //(RebuildAllLight) in Flush(); dynamic lights are updated incrementally
        //and do not set this flag.
        //自上次 flush 以来是否有静态光照数据（瓦片光源、遮罩或环境光颜色）变化。
        //静态变化在 Flush() 中触发一次全量重算（RebuildAllLight）；动态光源走
        //增量更新，不置此标记。
        bool staticDirty_ = false;

        //Whether only the ambient light color changed since the last flush. This
        //triggers an ambient-only rebuild (RebuildAmbientLight) instead of a full
        //re-propagation of every point light source.
        //自上次 flush 以来是否仅环境光颜色发生变化。仅触发环境光重建
        //（RebuildAmbientLight），而非重新传播所有点光源。
        bool ambientDirty_ = false;

        //Ambient light sources, updated by SetLightColor.
        //backLightSource_: light coming from the background layer (-Z).
        //skyLightSource_:  light coming from above (+Y).
        //环境光源，由 SetLightColor 更新。
        //backLightSource_：来自背景层（-Z）的屏幕光。
        //skyLightSource_：来自上方（+Y）的天光。
        LightSource backLightSource_{LightDirection::Backward, Color{}};
        LightSource skyLightSource_{LightDirection::Downward, Color{}};

        TileLightData &GetOrCreate(const TileVector2D &position);

        void SetLightFromSource(const LightSource &source, TileLayerType layerType);

        void ClearLightFromSource(const LightSource &source, TileLayerType layerType);

        /**
         * SetDynamicLightFromSource
         * 传播一个动态点光源：与 SetLightFromSource 相同，但对本次传播新建的
         * 瓦片即时注入环境光贡献（否则这些瓦片缺少背光/天光）。用于增量更新。
         */
        void SetDynamicLightFromSource(const LightSource &source, TileLayerType layerType);

        bool SetLightContributionAt(const TileVector2D &position, TileLayerType layerType, const LightSource &source,
                                    float accumulated);

        void ClearLightContributionAt(const TileVector2D &position, TileLayerType layerType, const LightSource &source);

        void SetAmbientLightContributionAt(const TileVector2D &position, TileLayerType layerType,
                                           const LightSource &source,
                                           std::unique_ptr<Color> lightColor);

        void RebuildAllLight();

        /**
         * RebuildAmbientLight
         * 重建环境光贡献。清除由环境光源产生的贡献，并根据当前背光/天光颜色、
         * 背光遮罩与天光透射率重新注入每个瓦片的环境光贡献。
         */
        void RebuildAmbientLight();

        /**
         * InjectAmbientLightAt
         * 向单个瓦片注入环境光贡献（背光 + 天光，逐图层）。供 RebuildAmbientLight
         * 与动态光源增量传播复用。
         */
        void InjectAmbientLightAt(const TileVector2D &position, TileLightData &tileData);

        /**
         * MarkLightDirty
         * 标记静态光照状态已变化，需要重新传播光线。仅置静态脏标记，
         * 实际重算延迟到 Flush()。
         */
        void MarkLightDirty();

        /**
         * GetLightBlockingStrength
         * 获取某瓦片在指定图层、指定光照方向上的挡光强度（0~1）。
         * 背光（Backward）查背光遮照，点光（Radial）/天光（Downward）查侧面遮照。
         * @param position position 瓦片世界坐标
         * @param layerType layerType 图层类型
         * @param direction direction 光照方向
         * @return 0~1 的挡光强度；瓦片不存在或无遮罩时返回 0
         */
        [[nodiscard]] float GetLightBlockingStrength(const TileVector2D &position, TileLayerType layerType,
                                                     LightDirection direction) const;

        /**
         * ComputeAmbientLightColor
         * 计算一个没有任何光照数据（光源/遮罩）的瓦片的环境光颜色。
         * 背光无条件注入（空瓦片没有背光遮罩），天光按天光透射率注入。
         * @param position position 瓦片世界坐标
         * @return 该空瓦片应得到的最终环境光颜色；无任何环境光时返回黑色。
         */
        [[nodiscard]] Color ComputeAmbientLightColor(const TileVector2D &position) const;

        /**
         * UpdateColumnSkyOccluder
         * 根据某列 Ground 层瓦片的挡光状态变化，增量维护天光遮挡索引。
         * @param position position 瓦片世界坐标
         * @param nowBlocks nowBlocks 该瓦片当前是否阻挡天光
         */
        void UpdateColumnSkyOccluder(const TileVector2D &position, bool nowBlocks);

    public:
        /**
         * SetLightMask
         * 设置某瓦片在指定图层、指定光照方向上的光线遮罩。
         * 背光（Backward）存为背光遮照；点光/天光（Radial/Downward）归一化为侧面遮照。
         * @param position position 瓦片世界坐标
         * @param layerType layerType 图层类型
         * @param direction direction 光照方向
         * @param lightMask lightMask 光线遮罩
         */
        void SetLightMask(const TileVector2D &position, TileLayerType layerType, LightDirection direction,
                          std::unique_ptr<LightMask> lightMask);

        /**
         * ClearLightMask
         * 清除某瓦片在指定图层、指定光照方向上的光线遮罩。
         */
        void ClearLightMask(const TileVector2D &position, TileLayerType layerType, LightDirection direction);

        void ClearTileLightData(const TileVector2D &position);

        [[nodiscard]] const TileLightData *GetTileLightData(const TileVector2D &position) const;

        void SetLightSource(const TileVector2D &position, TileLayerType layerType,
                            std::unique_ptr<LightSource> lightSource);

        void ClearLightSource(const TileVector2D &position, TileLayerType layerType);

        [[nodiscard]] Color GetFinalLightColor(const TileVector2D &position) const;

        /**
         * SetDynamicLight
         * 设置/更新一个动态（移动）光源。若 id 已存在且位置/光源参数不变，
         * 则跳过；否则立即增量更新其贡献（清除旧区域、传播新区域并补注新建
         * 瓦片的环境光），不触发全量重算。
         * @param id id 光源唯一标识（如实体 id）
         * @param position position 光源所在瓦片
         * @param layerType layerType 图层
         * @param lightSource lightSource 光源数据
         */
        void SetDynamicLight(uint64_t id, const TileVector2D &position, TileLayerType layerType,
                             std::unique_ptr<LightSource> lightSource);

        /**
         * RemoveDynamicLight
         * 移除指定 id 的动态光源，并立即增量清除其贡献。
         * @param id id 光源唯一标识
         */
        void RemoveDynamicLight(uint64_t id);

        /**
         * Flush
         * 若自上次刷新以来存在静态光照修改（静态脏标记），则执行一次全量重算
         * （传播所有光源 + 重建环境光），并递增修订号；若仅有环境光颜色变化，
         * 则只重建环境光。动态光源已增量更新，无需在此处理。每帧在渲染读取
         * 光照结果前调用一次即可合并当帧所有静态更新。
         */
        void Flush();

        /**
         * SetLightColor
         * 设置光照颜色。值变化时仅置环境光脏标记，随下一次 Flush() 触发
         * 环境光重建与修订号递增，从而重建光照贴图（不重新传播点光源）。
         * @param backLight backLight 背光颜色（背景层，来自 -Z）
         * @param skyLight skyLight 天光颜色（来自 +Y）
         */
        void SetLightColor(const Color &backLight, const Color &skyLight);

        /**
         * GetSkyTransmittance
         * 获取指定瓦片的天光透射率（0~1），等于其上方所有阻挡天光的瓦片
         * (1 - 挡光强度) 的连乘。
         * @param position position 瓦片世界坐标
         * @return 0~1 的天光透射率
         */
        [[nodiscard]] float GetSkyTransmittance(const TileVector2D &position) const;

        /**
         * GetColumnSkyTopY
         * 获取指定列最高的阻挡天光的地面瓦片 y，用于调试显示。
         * @param x x 列坐标
         * @return 若该列存在阻挡天光的地面瓦片则返回其 y，否则返回 WORLD_MIN_Y - 1
         */
        [[nodiscard]] int GetColumnSkyTopY(int x) const;

        /**
         * @return The current revision counter. Any change to the buffered
         * light data (sources, masks, contributions) increments it.
         * 当前修订计数器。任何对光照缓冲数据（光源、遮罩、贡献）的修改都会使其递增。
         */
        [[nodiscard]] uint64_t GetRevision() const;

        /**
         * GetBackLightColor
         * 获取当前背光（背景层，-Z）环境色。
         */
        [[nodiscard]] const Color *GetBackLightColor() const;

        /**
         * GetSkyLightColor
         * 获取当前天光（上方，+Y）环境色。
         */
        [[nodiscard]] const Color *GetSkyLightColor() const;

        /**
         * GetDynamicLights
         * 获取全部动态（移动）光源，用于调试显示。
         */
        [[nodiscard]] const std::unordered_map<uint64_t, DynamicLightEntry> *GetDynamicLights() const;
    };
}
