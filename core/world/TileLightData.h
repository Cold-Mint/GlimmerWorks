//
// Created by coldmint on 2026/4/24.
//

#ifndef GLIMMERWORKS_LIGHTDATA_H
#define GLIMMERWORKS_LIGHTDATA_H
#include <memory>
#include <unordered_map>
#include <vector>

#include "LightContribution.h"
#include "LightMask.h"
#include "generator/TileLayerType.h"
#include "core/math/Color.h"


namespace glimmer {
    /**
     * TileLightData
     * 瓦片光照数据
     */
    class TileLightData {
        std::unordered_map<TileLayerType, std::vector<std::unique_ptr<LightContribution> > > lightContributions_;
        std::unordered_map<TileLayerType, std::unique_ptr<LightSource> > lightSourceData_;
        std::unordered_map<TileLayerType, std::unique_ptr<LightMask> > lightMaskData_;
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

        [[nodiscard]] const std::unordered_map<TileLayerType, std::vector<std::unique_ptr<LightContribution> > > *
        GetLightContributions() const;


        [[nodiscard]] const std::unordered_map<TileLayerType, std::unique_ptr<LightSource> > *GetLightSources() const;

        [[nodiscard]] const std::unordered_map<TileLayerType, std::unique_ptr<LightMask> > *GetLightMasks() const;

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
         * 设置遮照（不触发总颜色计算）
         * @param layerType layerType 图层类型
         * @param lightMask lightMask 光源遮照
         */
        void SetLightMask(TileLayerType layerType, std::unique_ptr<LightMask> lightMask);

        [[nodiscard]] const LightMask *GetLightMask(TileLayerType layerType);

        [[nodiscard]] const LightSource *GetLightSource(TileLayerType layerType);

        /**
         * ClearLightMask(Does not trigger the overall color calculation)
         * 清除遮照（不触发总颜色计算）
         * @param layerType layerType 图层类型
         */
        void ClearLightMask(TileLayerType layerType);

        /**
         * ClearLightSource(Does not trigger the overall color calculation)
         * 清理光源（不触发总颜色计算）
         * @param layerType layerType 图层类型
         */
        void ClearLightSource(TileLayerType layerType);

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
         * @param rayIndex rayIndex 射线索引
         */
        void ClearLightContribution(TileLayerType layerType, const LightSource *lightSource, int rayIndex);
    };
}


#endif //GLIMMERWORKS_LIGHTDATA_H
