//
// Created by coldmint on 2026/2/26.
//

#ifndef GLIMMERWORKS_PRELOADCOLORS_H
#define GLIMMERWORKS_PRELOADCOLORS_H
#include <string>

#include "SDL3/SDL_pixels.h"


namespace glimmer {
    struct ColorResource;
    class ResourceLocator;
    /**
     * Preloaded colors
     * 预加载的颜色
     * The commonly used color values used within the game engine.
     * 用于游戏引擎内部常用的颜色值。
     */
    struct PreloadColors {
        struct Console {
            SDL_Color titleColor;
            SDL_Color keywordColor;
            SDL_Color textColor;
            SDL_Color backgroundColor;
        };

        SDL_Color defaultTextColor;
        Console console;


        void LoadAllColors(const ResourceLocator *resourceLocator);

    private:
        /**
         * Load color
         * 加载颜色
         * Only supports loading the colors of Core data.
         * 仅支持加载Core数据的颜色。
         * @param resourceLocator 资源定位器
         * @param key 路径
         * @return
         */
        static ColorResource *LoadColor(const ResourceLocator *resourceLocator, const std::string &key);
    };
}


#endif //GLIMMERWORKS_PRELOADCOLORS_H
