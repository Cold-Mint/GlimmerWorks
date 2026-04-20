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
        std::unordered_map<TileVector2D, std::array<std::unique_ptr<LightSource>, Count>,
            Vector2DIHash> lightSources_ = {};
        std::unordered_map<TileVector2D, SDL_Color, Vector2DIHash> lightColors_ = {};
        std::unordered_map<TileVector2D, std::array<std::unique_ptr<LightMask>, Count>, Vector2DIHash>
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

    public:
        explicit LightingBuffer(WorldContext *worldContext);

        /**
         * AddLightMask
         * 添加光源遮照
         * @param lightMask
         */
        void AddLightMask(std::unique_ptr<LightMask> lightMask);

        void RemoveLightMask(TileVector2D position);

        /**
         * AddLightSource
         * 添加光源
         * @param lightSource
         */
        void AddLightSource(std::unique_ptr<LightSource> lightSource);

        /**
         * GetLightColor
         * 获取光照颜色
         * @param position
         * @return
         */
        SDL_Color GetLightColor(TileVector2D position);

        /**
         * RemoveLightSource
         * 移除光源
         * @param layerType layerType 图层类型
         * @param position position 位置
         */
        void RemoveLightSource(TileLayerType layerType,const TileVector2D &position);
    };
}


#endif //GLIMMERWORKS_LIGHTINGBUFFER_H
