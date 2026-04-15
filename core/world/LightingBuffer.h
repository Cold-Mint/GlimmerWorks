//
// Created by coldmint on 2026/4/15.
//

#ifndef GLIMMERWORKS_LIGHTINGBUFFER_H
#define GLIMMERWORKS_LIGHTINGBUFFER_H
#include <vector>

#include "LightSource.h"
#include "PreloadColors.h"
#include "TraverseAction.h"


namespace glimmer {
    class LightingBuffer {
        std::vector<std::unique_ptr<LightSource> > lightSources_ = {};
        std::unordered_map<TileVector2D, SDL_Color, Vector2DIHash> lightColors_ = {};
        WorldContext *worldContext_ = nullptr;

        TraverseAction StepCallback(TileVector2D current,
                                    TileVector2D next);

    public:
        explicit LightingBuffer(WorldContext *worldContext);

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
         * @param position position 位置
         */
        void RemoveLightSource(const TileVector2D &position);
    };
}


#endif //GLIMMERWORKS_LIGHTINGBUFFER_H
