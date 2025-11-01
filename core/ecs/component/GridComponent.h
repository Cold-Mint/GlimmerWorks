//
// Created by Cold-Mint on 2025/10/29.
//

#ifndef GLIMMERWORKS_GRIDCOMPONENT_H
#define GLIMMERWORKS_GRIDCOMPONENT_H
#include "../GameComponent.h"
#include "../../math/Vector2D.h"
#include "SDL3/SDL_stdinc.h"

namespace glimmer
{
    class GridComponent : public GameComponent
    {
    public:
        // 网格起点坐标
        WorldVector2D startPoint;

        // 网格尺寸
        int width;
        int height;

        // 网格大小
        int gridSizeX;
        int gridSizeY;

        // 网格颜色
        Uint8 r;
        Uint8 g;
        Uint8 b;
        Uint8 a;

        // 线宽
        int lineWidth;

        GridComponent() :
            startPoint(WorldVector2D(0, 0)),
            width(100),
            height(100),
            gridSizeX(50),
            gridSizeY(50),
            r(0), g(0), b(255), a(255),
            lineWidth(1)
        {
        }

        GridComponent(WorldVector2D start, int w, int h, int gridX, int gridY, Uint8 red, Uint8 green, Uint8 blue,
                      Uint8 alpha, int lineW) :
            startPoint(start),
            width(w),
            height(h),
            gridSizeX(gridX),
            gridSizeY(gridY),
            r(red), g(green), b(blue), a(alpha),
            lineWidth(lineW)
        {
        }
    };
}

#endif //GLIMMERWORKS_GRIDCOMPONENT_H
