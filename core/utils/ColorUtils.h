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
        static SDL_Color LerpColor(SDL_Color from, SDL_Color to, float percent);

        /**
         * AverageColors
         * 平均颜色
         * @param colors
         * @return
         */
        static SDL_Color AverageColors(const std::vector<SDL_Color> &colors);

        /**
         * 计算光线穿过遮挡块后的最终颜色
         * @param lightColor 入射光线颜色（发光颜色）
         * @param occlusionColor 遮挡块颜色（a=0不遮挡，a=255完全遮挡）
         * @return 最终穿透后的光照颜色
         */
        static SDL_Color ApplyOcclusion(SDL_Color lightColor, SDL_Color occlusionColor);

        /**
        * 对颜色进行亮度衰减（光照专用）
        * @param color 原始颜色
        * @param lightPercent 光照百分比（0.0 = 完全衰减，1.0 = 不衰减）
        * @return 衰减后的颜色
        */
        static SDL_Color DecayColor(SDL_Color color, float lightPercent);
    };
}


#endif //GLIMMERWORKS_COLORUTILS_H
