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

#include "LightContribution.h"
#include "LightMask.h"
#include "core/math/Color.h"
#include "generator/TileLayerType.h"


namespace glimmer {
    /**
     * TileLightData
     * 瓦片光照数据
     */
    class TileLightData {
        std::unordered_map<TileLayerType, std::vector<std::unique_ptr<LightContribution> > > lightContributions_;
        std::unordered_map<TileLayerType, std::unique_ptr<LightSource> > lightSourceData_;
        std::unordered_map<TileLayerType, std::unordered_map<LightDirection, std::unique_ptr<LightMask> > > lightMaskData_;
        std::unique_ptr<Color> finalLightColor_;

        /**
         * ComputeFinalLightColor
         * 计算总颜色。
         */
        [[nodiscard]] std::unique_ptr<Color> ComputeFinalLightColor();

    public:
        /**
         * Set the contribution of light(Triggering the overall color calculation)
         * 设置光照贡献（触发总颜色计算）
         * @param layerType layerType 图层类型
         * @param contribution contribution 贡献
         */
        void SetLightContribution(TileLayerType layerType, std::unique_ptr<LightContribution> contribution);

        /**
         * ClearAllLightContributions
         * 清空全部光照贡献与最终颜色（不触发计算）。
         */
        void ClearAllLightContributions();

        [[nodiscard]] const std::unordered_map<TileLayerType, std::vector<std::unique_ptr<LightContribution> > > *
        GetLightContributions() const;


        [[nodiscard]] const std::unordered_map<TileLayerType, std::unique_ptr<LightSource> > *GetLightSources() const;

        /**
         * GetLightMasks
         * 获取按图层与方向组织的光线遮罩。背光遮照（Backward）与侧面遮照
         * （Downward，点光与天光共用）各占一个方向键。
         */
        [[nodiscard]] const std::unordered_map<TileLayerType, std::unordered_map<LightDirection, std::unique_ptr<LightMask> > > *
        GetLightMasks() const;

        /**
         * GetLightContribution
         * 获取某个图层下的光照贡献
         * @param layerType layerType 图层类型
         * @param targetLightSource 目标光源。
         * @return
         */
        [[nodiscard]] const LightContribution *GetLightContribution(TileLayerType layerType,
                                                                    const LightSource *targetLightSource);

        /**
         * GetLightContributionVector
         * 获取某个图层下的光照贡献列表
         * @param layerType  layerType 图层类型
         * @return
         */
        [[nodiscard]] std::vector<const LightContribution *> GetLightContributionVector(TileLayerType layerType);

        /**
         * SetLightSource(Does not trigger the overall color calculation)
         * 设置光源（不触发总颜色计算）
         * @param layerType layerType 图层类型
         * @param lightSource lightSource 光源
         */
        void SetLightSource(TileLayerType layerType, std::unique_ptr<LightSource> lightSource);

        /**
         * SetLightMask(Does not trigger the overall color calculation)
         * 设置光线遮罩（不触发总颜色计算）。
         * @param layerType layerType 图层类型
         * @param direction direction 遮照作用的光照方向（Backward=背光，Downward/Radial=侧面遮照）
         * @param lightMask lightMask 光线遮罩
         */
        void SetLightMask(TileLayerType layerType, LightDirection direction, std::unique_ptr<LightMask> lightMask);

        [[nodiscard]] const LightMask *GetLightMask(TileLayerType layerType, LightDirection direction);

        [[nodiscard]] const LightSource *GetLightSource(TileLayerType layerType);

        void ClearLightMask(TileLayerType layerType, LightDirection direction);

        /**
         * ClearLightSource(Does not trigger the overall color calculation)
         * 清理光源（不触发总颜色计算）
         * @param layerType layerType 图层类型
         */
        void ClearLightSource(TileLayerType layerType);

        /**
         * GetLightBlockingStrength
         * 获取该瓦片在指定图层、指定光照方向上的挡光强度（0~1，1 = 完全挡光）。
         * 由对应方向遮罩 alpha 推导：挡光强度 = a/255。无遮罩时返回 0（不挡光）。
         * @param layerType layerType 图层类型
         * @param direction direction 光照方向
         * @return 0~1 的挡光强度
         */
        [[nodiscard]] float GetLightBlockingStrength(TileLayerType layerType, LightDirection direction) const;

        /**
         * GetFinalLightColor
         * 获取最终的光照。
         * @return
         */
        [[nodiscard]] const Color *GetFinalLightColor() const;


        /**
         * ClearLightContribution(Triggering the overall color calculation)
         * 清除光照贡献（触发总颜色计算）
         * @param layerType  layerType 图层类型
         * @param lightSource lightSource 光源指针
         */
        void ClearLightContribution(TileLayerType layerType, const LightSource *lightSource);
    };
}
