//
// Created by coldmint on 2026/4/5.
//

#ifndef GLIMMERWORKS_COLORUTILS_H
#define GLIMMERWORKS_COLORUTILS_H
#include <vector>

#include "SDL3/SDL_pixels.h"


namespace glimmer {
    class ColorUtils {
    public:
        /**
         * Obtain the mixing value of a color.
         * 获取一个颜色的混色值。
         * @param from 起始颜色。
         * @param to 终点颜色。
         * @param percent 输入1对应终点颜色，0对应起始颜色，其他值介于两者之间的颜色。
         * @return
         */
        static SDL_Color LerpColor(SDL_Color from, SDL_Color to,float percent);

        static SDL_Color AverageColors(const std::vector<SDL_Color>& colors);
    };
}


#endif //GLIMMERWORKS_COLORUTILS_H
