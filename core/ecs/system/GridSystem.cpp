//
// Created by Cold-Mint on 2025/10/29.
//

#include "GridSystem.h"
#include "../../log/LogCat.h"

void glimmer::GridSystem::Update(float delta)
{
    // 网格系统主要负责渲染，更新逻辑可以为空
}

void glimmer::GridSystem::Render(SDL_Renderer* renderer)
{
    // 保存当前渲染器的颜色设置
    Uint8 currentR, currentG, currentB, currentA;
    SDL_GetRenderDrawColor(renderer, &currentR, &currentG, &currentB, &currentA);
    // 按照用户要求：网格大小50*50像素，颜色为蓝色1px，起点在0,0，单元格数量为100*100
    const int startX = 0;
    const int startY = 0;
    const int width = 100; // 单元格数量
    const int height = 100; // 单元格数量
    const int gridSizeX = 50; // 每个单元格的像素宽度
    const int gridSizeY = 50; // 每个单元格的像素高度

    // 设置蓝色
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

    // 设置线宽
    // SDL3中设置线宽的API可能不同，这里使用1px宽度

    // 计算实际像素范围
    const int pixelWidth = width * gridSizeX;
    const int pixelHeight = height * gridSizeY;

    // 绘制垂直线
    for (int x = startX; x <= pixelWidth; x += gridSizeX)
    {
        SDL_RenderLine(renderer, x, startY, x, startY + pixelHeight);
    }

    // 绘制水平线
    for (int y = startY; y <= pixelHeight; y += gridSizeY)
    {
        SDL_RenderLine(renderer, startX, y, startX + pixelWidth, y);
    }


    // 恢复之前的渲染颜色
    SDL_SetRenderDrawColor(renderer, currentR, currentG, currentB, currentA);
}

std::string glimmer::GridSystem::GetName()
{
    return "glimmer.GridSystem";
}

bool glimmer::GridSystem::ShouldActivate()
{
    // 当有GridComponent组件时激活系统
    return worldContext->HasComponentType(std::type_index(typeid(GridComponent)));
}
