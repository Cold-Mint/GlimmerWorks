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
         * 加法混合
         * @param firstColor
         * @param secondColor
         * @return
         */
        static SDL_FColor AdditiveBlend(SDL_FColor firstColor, SDL_FColor secondColor);

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
        static SDL_FColor DecayColor(SDL_FColor color, float lightPercent);

        /**
         * ColorToFColor
         * 颜色转为浮点型颜色。
         * @param sdlColor
         * @return
         */
        static SDL_FColor ColorToFColor(const SDL_Color &sdlColor);

        static SDL_Color FColorToColorRaw(const SDL_FColor &sdlFColor);

        static SDL_Color FColorToColorToneMapped(const SDL_FColor &sdlFColor, float exposure);

        /**
    * @brief Reinhard 色调映射算法（HDR → LDR）
    *
    * 作用：将无限大的浮点亮度（光照强度）压缩到 0~1 范围内
    * 解决问题：多光源叠加后不会变成纯白色，保留高光层次
    *
    * 核心原理：y = (exposure * x) / (1 + exposure * x)
    * 不管输入 x 多大（1/2/5/10），输出永远无限接近 1，不会顶死
    *
    * @param x 输入的浮点亮度（可以是 RGB 颜色 或 A 通道强度）
    * @param exposure 曝光值（整体亮度调节旋钮，越大画面越亮）
    * @return 压缩后的 0~1 之间的亮度值
    */
        static float ToneMapReinhard(float x, float exposure);
    };
}


#endif //GLIMMERWORKS_COLORUTILS_H
