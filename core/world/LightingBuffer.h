//
// Created by coldmint on 2026/4/15.
//

#ifndef GLIMMERWORKS_LIGHTINGBUFFER_H
#define GLIMMERWORKS_LIGHTINGBUFFER_H

#include "LightMask.h"
#include "LightSource.h"
#include "PreloadColors.h"
#include "TraverseAction.h"


namespace glimmer {
    class LightingBuffer {
        std::unordered_map<const TileVector2D, std::unordered_map<TileLayerType, std::unique_ptr<LightSource> >,
            Vector2DIHash> lightSources_ = {};
        std::unordered_map<const TileVector2D, std::unordered_map<TileLayerType, std::unique_ptr<SDL_Color> >,
            Vector2DIHash> layerLightColors_ = {};
        std::unordered_map<const TileVector2D, std::unique_ptr<SDL_Color>, Vector2DIHash> totalLightColor_ = {};
        std::unordered_map<const TileVector2D, std::unordered_map<TileLayerType, std::unique_ptr<LightMask> >,
            Vector2DIHash>
        lightMasks_ = {};
        WorldContext *worldContext_ = nullptr;


        /**
         * 应用光照传播（添加光源时，为瓦片设置光照）
         * @param lightSource
         * @param current
         * @param next
         * @return
         */
        TraverseAction ApplyLightPropagation(const LightSource *lightSource, TileVector2D current,
                                             TileVector2D next);

        /**
         * 清除光照传播（移除光源时，清理瓦片光照）
         * @param lightSource
         * @param current
         * @param next
         * @return
         */
        TraverseAction ClearLightPropagation(const LightSource *lightSource, TileVector2D current,
                                             TileVector2D next);

        std::unique_ptr<SDL_Color> ComputeTotalLightColorFromLayers(TileVector2D position);

        /**
         * Update the light source within the specified radius around a certain position.
         * 更新某个位置周围指定半径的光源。
         *
         * Trigger a recalculation.
         * 触发重新计算。
         * @param layerType layerType 图层类型
         * @param center center 原点
         * @param radius radius 半径
         */
        void UpdateAllLightsInRadius(TileLayerType layerType, TileVector2D center, int radius);

    public:
        explicit LightingBuffer(WorldContext *worldContext);

        /**
         * AddLightMask
         * 添加光源遮照
         * @param lightMask
         */
        void AddLightMask(std::unique_ptr<LightMask> lightMask);

        void RemoveLightMask(TileLayerType layerType, const TileVector2D &position);

        void RemoveAllLightMask(const TileVector2D &position);

        /**
         * AddLightSource
         * 添加光源
         * @param lightSource
         */
        void AddLightSource(std::unique_ptr<LightSource> lightSource);

        const SDL_Color *GetTotalLightColor(TileVector2D position);

        const SDL_Color *GetLayerLightColor(TileVector2D position, TileLayerType layerType);

        const SDL_Color *GetLayerLightMaskColor(TileVector2D position, TileLayerType layerType);


        /**
         * RemoveLightSource
         * 移除光源
         * @param layerType layerType 图层类型
         * @param position position 位置
         */
        void RemoveLightSource(TileLayerType layerType, const TileVector2D &position);

        /**
         * RemoveAllLightSources
         * 移除所有光源
         * @param position
         */
        void RemoveAllLightSources(const TileVector2D &position);
    };
}


#endif //GLIMMERWORKS_LIGHTINGBUFFER_H
